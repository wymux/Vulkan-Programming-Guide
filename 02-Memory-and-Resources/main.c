#include <stdlib.h>
#include <stdio.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

uint32_t chooseHeapFromFlags(VkMemoryRequirements *memoryRequirements,
			     VkMemoryPropertyFlags requiredFlags,
			     VkMemoryPropertyFlags preferredFlags,
			     const VkPhysicalDevice *physicalDevice);

static void *VKAPI_CALL vk_alloc(void *pUserData, size_t size, size_t alignment,
				 VkSystemAllocationScope allocationScope)
{
	return aligned_alloc(alignment, size);
}

static void *VKAPI_CALL vk_realloc(void *pUserData, void *pOriginal,
				   size_t size, size_t alignment,
				   VkSystemAllocationScope allocationScope)
{
	pOriginal = realloc(pOriginal, size);
	return pOriginal;
}

static void VKAPI_CALL vk_free(void *pUserData, void *pMemory)
{
	free(pMemory);
}

static void VKAPI_CALL vk_internal_alloc(
	void *pUserData, size_t size, VkInternalAllocationType allocationType,
	VkSystemAllocationScope allocationScope)
{
	printf("Internal allocation of size %zu, type %d, scope %d\n", size,
	       allocationType, allocationScope);
}

static void VKAPI_CALL vk_internal_free(void *pUserData, size_t size,
					VkInternalAllocationType allocationType,
					VkSystemAllocationScope allocationScope)
{
	printf("Size: %zu\n", size);
}

int main()
{
	VkResult result = VK_SUCCESS;
	VkApplicationInfo app_info;
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = NULL;
	app_info.pApplicationName = "Test";
	app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);

	VkInstanceCreateInfo create_info;
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pNext = NULL;
	create_info.pApplicationInfo = &app_info;
	create_info.enabledExtensionCount = 0;
	create_info.ppEnabledLayerNames = NULL;
	create_info.enabledExtensionCount = 0;
	create_info.ppEnabledExtensionNames = NULL;

	uint32_t instance_extension_count = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count,
					       NULL);
	if (result != VK_SUCCESS) {
		perror("vkEnumerateInstanceExtensionProperties()");
		return -1;
	}

	VkExtensionProperties *instance_extension_properties;
	if (instance_extension_count != 0) {
		const char **extension_names_arr;
		extension_names_arr =
			calloc(instance_extension_count, sizeof(char *));
		instance_extension_properties =
			calloc(instance_extension_count,
			       sizeof(VkExtensionProperties));
		vkEnumerateInstanceExtensionProperties(
			NULL, &instance_extension_count,
			instance_extension_properties);
		for (int i = 0; i < instance_extension_count; i++) {
			extension_names_arr[i] =
				malloc(VK_MAX_EXTENSION_NAME_SIZE);
			if (extension_names_arr[i] == NULL) {
				perror("malloc");
				return -1;
			}

			extension_names_arr[i] =
				(instance_extension_properties + i)
					->extensionName;
			create_info.enabledExtensionCount =
				instance_extension_count;
			create_info.ppEnabledExtensionNames =
				extension_names_arr;
			printf("Extension Count: %Lu\n"
			       "SpecVersion: %Lu\n"
			       "Extension Name: %s\n",
			       (instance_extension_properties +
				i * sizeof(vkEnumerateInstanceExtensionProperties))
				       ->specVersion,
			       (instance_extension_properties +
				i * sizeof(vkEnumerateInstanceExtensionProperties))
				       ->extensionName);
		}
	}

	VkAllocationCallbacks vk_allocator;
	vk_allocator.pUserData = NULL;
	vk_allocator.pfnAllocation = vk_alloc;
	vk_allocator.pfnReallocation = vk_realloc;
	vk_allocator.pfnFree = vk_free;
	vk_allocator.pfnInternalAllocation = vk_internal_alloc;
	vk_allocator.pfnInternalFree = vk_internal_free;

	VkInstance instance;
	result = vkCreateInstance(&create_info, &vk_allocator, &instance);
	if (result != VK_SUCCESS) {
		perror("Error: vkCreateInstance()");
		return -1;
	}

	uint32_t device_count;
	result = vkEnumeratePhysicalDevices(instance, &device_count, NULL);
	if (result != VK_SUCCESS) {
		perror("Error: vkEnumeratePhysicalDevices()");
		return -1;
	}
	printf("Number of devices: %lu\n", device_count);
	VkPhysicalDevice *devices =
		calloc(device_count, sizeof(VkPhysicalDevice));
	result = vkEnumeratePhysicalDevices(instance, &device_count, devices);
	if (result != VK_SUCCESS) {
		perror("Error: vkEnumeratePhysicalDevices()");
		return -1;
	}

	VkPhysicalDeviceProperties physical_properties;
	vkGetPhysicalDeviceProperties(devices[0], &physical_properties);
	printf("Device Name: %s %Lu %Lu %Lu Priority: %Lu\n",
	       physical_properties.deviceName, physical_properties.apiVersion,
	       physical_properties.vendorID, physical_properties.deviceID,
	       physical_properties.limits.discreteQueuePriorities);

	VkPhysicalDeviceFeatures physical_features;
	vkGetPhysicalDeviceFeatures(devices[0], &physical_features);

	VkPhysicalDeviceMemoryProperties physical_memory;
	vkGetPhysicalDeviceMemoryProperties(devices[0], &physical_memory);
	printf("Memory Properties: %Lu %Lu\n", physical_memory.memoryTypeCount,
	       physical_memory.memoryHeapCount);

	uint32_t queue_family;
	vkGetPhysicalDeviceQueueFamilyProperties(devices[0], &queue_family,
						 NULL);
	VkQueueFamilyProperties *family_properties =
		calloc(queue_family, sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(devices[0], &queue_family,
						 family_properties);
	printf("%Lu %d %Lu\n", family_properties->queueCount,
	       family_properties->queueFlags & VK_QUEUE_COMPUTE_BIT,
	       family_properties->timestampValidBits);

	VkDevice device;
	VkDeviceQueueCreateInfo queue_info = {};
	queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_info.flags = 0;
	queue_info.queueFamilyIndex = 0;
	queue_info.queueCount = 1;
	queue_info.pQueuePriorities = 0;

	VkDeviceCreateInfo device_info;
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = NULL;
	device_info.flags = 0;
	device_info.queueCreateInfoCount = 1;
	device_info.pQueueCreateInfos = &queue_info;
	device_info.enabledLayerCount = 0;
	device_info.enabledExtensionCount = 0;
	device_info.ppEnabledLayerNames = NULL;
	device_info.ppEnabledExtensionNames = NULL;
	device_info.pEnabledFeatures = &physical_features;

	result = vkCreateDevice(devices[0], &device_info, NULL, &device);
	if (result != VK_SUCCESS) {
		perror("vkCreateDevice()");
		return -1;
	}

	uint32_t property_count = 0;
	result = vkEnumerateInstanceLayerProperties(&property_count, NULL);
	if (property_count != 0) {
		VkLayerProperties *layer_properties =
			calloc(property_count, sizeof(VkLayerProperties));
		result = vkEnumerateInstanceLayerProperties(&property_count,
							    layer_properties);
		printf("specVersion: %Lu\n"
		       "implemantationVersion: %Lu\n"
		       "description: %s",
		       layer_properties->specVersion,
		       layer_properties->implementationVersion,
		       layer_properties->description);
	}
	if (result != VK_SUCCESS) {
		perror("vkEnumarateInstanceLayerProperties()");
		return -1;
	}

	uint32_t layer_count = 0;
	result = vkEnumerateDeviceLayerProperties(devices[0], &layer_count,
						  NULL);
	if (layer_count != 0) {
		VkLayerProperties *layer_properties =
			calloc(layer_count, sizeof(VkLayerProperties));
		result = vkEnumerateDeviceLayerProperties(
			devices[0], &layer_count, layer_properties);

		printf("specVersion: %Lu\n"
		       "implemantationVersion: %Lu\n"
		       "description: %s",
		       layer_properties->specVersion,
		       layer_properties->implementationVersion,
		       layer_properties->description);
	}

	if (result != VK_SUCCESS) {
		perror("vkEnumerateDeviceLayerProperties()");
		return -1;
	}

	PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
		(PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
			instance, "vkCreateDebugReportCallbackEXT");

	VkBufferCreateInfo buffer_create_info;
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.pNext = NULL;
	buffer_create_info.flags = VK_BUFFER_CREATE_SPARSE_BINDING_BIT;
	buffer_create_info.size = 1024 * 1024;
	buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
				   VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	buffer_create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
	buffer_create_info.queueFamilyIndexCount = 0;
	buffer_create_info.pQueueFamilyIndices = NULL;

	VkBuffer buffer = VK_NULL_HANDLE;
	result = vkCreateBuffer(device, &buffer_create_info, &vk_allocator,
				&buffer);
	if (result != VK_SUCCESS) {
		perror("vkCreateDBUffer()");
		return -1;
	}

	VkFormatFeatureFlags format_flags =
		VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
		VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
		VK_FORMAT_FEATURE_BLIT_DST_BIT;
	VkFormatProperties format_properties;
	format_properties.linearTilingFeatures = format_flags;
	format_properties.optimalTilingFeatures = format_flags;
	format_properties.bufferFeatures = format_flags;

	VkFormat vk_format = VK_FORMAT_R8G8B8A8_UNORM;
	vkGetPhysicalDeviceFormatProperties(devices[0], vk_format,
					    &format_properties);

	VkExtent3D vk_extent;
	vk_extent.width = 10;
	vk_extent.height = 1;
	vk_extent.depth = 1;

	VkImageFormatProperties image_format_properties;
	image_format_properties.maxExtent = vk_extent;
	VkImageUsageFlags vk_usage = VK_IMAGE_USAGE_STORAGE_BIT |
				     VK_IMAGE_USAGE_SAMPLED_BIT |
				     VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	VkImageCreateFlags vk_image_flags =
		VK_IMAGE_CREATE_SPARSE_BINDING_BIT |
		VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT |
		VK_IMAGE_CREATE_SPARSE_ALIASED_BIT;
	VkImageCreateInfo vk_image_create_info;
	vk_image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	vk_image_create_info.pNext = NULL;
	vk_image_create_info.flags = vk_image_flags;
	vk_image_create_info.imageType = VK_IMAGE_TYPE_1D;
	vk_image_create_info.format = vk_format;
	vk_image_create_info.extent = vk_extent;
	vk_image_create_info.mipLevels = 10;
	vk_image_create_info.arrayLayers =
		physical_properties.limits.maxImageArrayLayers;
	vk_image_create_info.samples = VK_SAMPLE_COUNT_4_BIT;
	vk_image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	vk_image_create_info.usage = vk_usage;
	vk_image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vk_image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VkImage image = VK_NULL_HANDLE;
	result = vkCreateImage(device, &vk_image_create_info, &vk_allocator,
			       &image);
	result = vkGetPhysicalDeviceImageFormatProperties(
		devices[0], vk_format, VK_IMAGE_TYPE_1D, VK_IMAGE_TILING_LINEAR,
		vk_usage, vk_image_flags, &image_format_properties);

	VkImageSubresource vk_image_subresource;
	vk_image_subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vk_image_subresource.mipLevel = 10;
	vk_image_subresource.arrayLayer = 0;

	VkSubresourceLayout vk_subresource_layout;

	vkGetImageSubresourceLayout(device, image, &vk_image_subresource,
				    &vk_subresource_layout);

	printf("Offset: %zu\n"
	       "Size: %zu\n"
	       "RowPitch: %zu\n"
	       "ArrayPitch: %zu\n"
	       "DepthPitch: %zu\n",
	       vk_subresource_layout.offset, vk_subresource_layout.rowPitch,
	       vk_subresource_layout.arrayPitch,
	       vk_subresource_layout.depthPitch);

	if (physical_features.textureCompressionBC == VK_TRUE)
		printf("Block Compessed format supported.\n");
	if (physical_features.textureCompressionETC2 == VK_TRUE)
		printf("ETC Compression format supported.\n");
	if (physical_features.textureCompressionASTC_LDR == VK_TRUE)
		printf("ASTC Compression supported.\n");

	VkBufferViewCreateInfo buffer_view_create_info;
	buffer_view_create_info.sType =
		VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
	buffer_view_create_info.pNext = NULL;
	buffer_view_create_info.flags = 0;
	buffer_view_create_info.buffer = buffer;
	buffer_view_create_info.format = vk_format;
	buffer_view_create_info.offset = 0;
	buffer_view_create_info.range =
		physical_properties.limits.maxTexelBufferElements;

	VkBufferView vk_buffer_view = VK_NULL_HANDLE;
	result = vkCreateBufferView(device, &buffer_view_create_info,
				    &vk_allocator, &vk_buffer_view);

	if (result != VK_SUCCESS) {
		perror("vkCreateBuffer()");
		return -1;
	}

	VkComponentMapping vk_component_mapping;
	vk_component_mapping.r = 0;
	vk_component_mapping.g = 0;
	vk_component_mapping.b = 0;
	vk_component_mapping.a = 0;

	VkImageSubresourceRange vk_image_subresource_range;
	vk_image_subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vk_image_subresource_range.baseMipLevel = 0;
	vk_image_subresource_range.levelCount = 1;
	vk_image_subresource_range.baseArrayLayer = 0;
	vk_image_subresource_range.layerCount = 1;

	VkImageViewCreateInfo image_view_create_info;
	image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	image_view_create_info.pNext = NULL;
	image_view_create_info.flags = 0;
	image_view_create_info.image = image;
	image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_1D;
	image_view_create_info.format = vk_format;
	image_view_create_info.components = vk_component_mapping;
	image_view_create_info.subresourceRange = vk_image_subresource_range;

	VkImageView vk_image_view = VK_NULL_HANDLE;
	result = vkCreateImageView(device, &image_view_create_info,
				   &vk_allocator, &vk_image_view);
	if (result != VK_SUCCESS) {
		perror("vkCreateImageView()");
		return -1;
	}

	VkMemoryAllocateInfo vk_memory_allocate_info;
	vk_memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vk_memory_allocate_info.pNext = NULL;
	vk_memory_allocate_info.allocationSize = 1024 * 1024;
	vk_memory_allocate_info.memoryTypeIndex =
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	VkDeviceMemory vk_device_memory;
	result = vkAllocateMemory(device, &vk_memory_allocate_info,
				  &vk_allocator, &vk_device_memory);

	VkDeviceSize device_size;
	vkGetDeviceMemoryCommitment(device, vk_device_memory, &device_size);
	void *data;

	result = vkMapMemory(device, vk_device_memory, 0, VK_WHOLE_SIZE, 0,
			     &data);
	if (result != VK_SUCCESS) {
		perror("vkMapMemory()");
		return -1;
	}

	/* VkMappedMemoryRange vk_mapped_memory_range = { 0 }; */
	/* vk_mapped_memory_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE; */
	/* vk_mapped_memory_range.pNext = NULL; */
	/* vk_mapped_memory_range.memory = vk_device_memory; */
	/* vk_mapped_memory_range.offset = 0; */
	/* vk_mapped_memory_range.size = VK_WHOLE_SIZE; */

	/* result = vkFlushMappedMemoryRanges(device, 10, &vk_mapped_memory_range); */

	/* if (result != VK_SUCCESS) { */
	/* 	perror("vkPlushMappedMemoryRanges()"); */
	/* 	return -1; */
	/* } */

	/* result = vkInvalidateMappedMemoryRanges(device, 10, */
	/* 					&vk_mapped_memory_range); */

	/* if (result != VK_SUCCESS) { */
	/* 	perror("vkInvalidateMappedMemoryRanges()"); */
	/* 	return -1; */
	/* } */

	VkMemoryRequirements vk_memory_requirements = { 0 };
	vk_memory_requirements.size = 1024 * 10;
	vk_memory_requirements.alignment = 1024;

	uint32_t heap = chooseHeapFromFlags(
		&vk_memory_requirements, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, devices);

	vkGetBufferMemoryRequirements(device, buffer, &vk_memory_requirements);
	vkGetImageMemoryRequirements(device, image, &vk_memory_requirements);

	result = vkBindBufferMemory(device, buffer, vk_device_memory,
				    device_size);
	if (result != VK_SUCCESS) {
		perror("vkBindBufferMemory()");
		return -1;
	}

	result =
		vkBindImageMemory(device, image, vk_device_memory, device_size);
	if (result != VK_SUCCESS) {
		perror("vkBindImageMemory()");
		return -1;
	}

	uint32_t sparse_memory_requirement_count = 0;
	vkGetImageSparseMemoryRequirements(
		device, image, &sparse_memory_requirement_count, NULL);

	VkSparseImageFormatProperties vk_sparse_image_format_properties = { 0 };
	vk_sparse_image_format_properties.aspectMask =
		VK_IMAGE_ASPECT_COLOR_BIT;
	vk_sparse_image_format_properties.imageGranularity = vk_extent;
	vk_sparse_image_format_properties.flags =
		VK_SPARSE_IMAGE_FORMAT_SINGLE_MIPTAIL_BIT;

	VkSparseImageMemoryRequirements vk_sparse_image_memory_requirements = {
		0
	};

	vk_sparse_image_memory_requirements.formatProperties =
		vk_sparse_image_format_properties;
	vkGetImageSparseMemoryRequirements(
		device, image, &sparse_memory_requirement_count,
		&vk_sparse_image_memory_requirements);

	//
	uint32_t sparse_image_property_count = 0;
	VkSparseImageFormatProperties vk_sparse;
	vkGetPhysicalDeviceSparseImageFormatProperties(
		devices[0], vk_format, VK_IMAGE_TYPE_1D, 0, vk_usage,
		VK_IMAGE_TILING_OPTIMAL, &sparse_image_property_count, NULL);

	vkUnmapMemory(device, vk_device_memory);
	vkFreeMemory(device, vk_device_memory, &vk_allocator);

	vkDestroyBuffer(device, buffer, &vk_allocator);
	vkDestroyBufferView(device, vk_buffer_view, &vk_allocator);
	vkDestroyImage(device, image, &vk_allocator);
	vkDestroyImageView(device, vk_image_view, &vk_allocator);

	result = vkDeviceWaitIdle(device);
	if (result != VK_SUCCESS) {
		perror("vkDeviceWaitIdle()");
		return -1;
	}

	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);

	return 0;
}

uint32_t chooseHeapFromFlags(VkMemoryRequirements *memoryRequirements,
			     VkMemoryPropertyFlags requiredFlags,
			     VkMemoryPropertyFlags preferredFlags,
			     const VkPhysicalDevice *physicalDevice)
{
	VkPhysicalDeviceMemoryProperties device_memory_properties;

	vkGetPhysicalDeviceMemoryProperties(physicalDevice[0],
					    &device_memory_properties);
	uint32_t selected_type = 0;
	uint32_t memory_type;
	for (memory_type = 0; memory_type < 32; ++memory_type) {
		if (memoryRequirements->memoryTypeBits & (1 << memory_type)) {
			const VkMemoryType type =
				device_memory_properties
					.memoryTypes[memory_type];
			if ((type.propertyFlags & preferredFlags) ==
			    preferredFlags) {
				selected_type = memory_type;
				break;
			}
		}
	}

	if (selected_type != ~0u) {
		for (memory_type = 0; memory_type < 32; ++memory_type) {
			if (memoryRequirements->memoryTypeBits &
			    (1 << memory_type)) {
				const VkMemoryType type =
					device_memory_properties
						.memoryTypes[memory_type];

				if ((type.propertyFlags & requiredFlags) ==
				    requiredFlags) {
					selected_type = memory_type;
					break;
				}
			}
		}
	}

	return selected_type;
}
