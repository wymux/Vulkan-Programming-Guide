#include <stdlib.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

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
	buffer_create_info.flags = 0;
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

	vkGetPhysicalDeviceFormatProperties(devices[0], NULL,
					    &format_properties);

	result = vkDeviceWaitIdle(device);
	if (result != VK_SUCCESS) {
		perror("vkDeviceWaitIdle()");
		return -1;
	}

	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);

	return 0;
}
