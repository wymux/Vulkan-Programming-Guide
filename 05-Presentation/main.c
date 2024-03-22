#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#define GPU_INDEX 0

void *wymux_allocation(void *pUserData, size_t size, size_t alignment,
		       VkSystemAllocationScope allocationScope)
{
	void *pMemory = malloc(size);
	return pMemory;
}

void wymux_free(void *pUserData, void *pMemory)
{
	free(pMemory);
}

void *wymux_reallocation(void *pUserData, void *pOriginal, size_t size,
			 size_t alignment,
			 VkSystemAllocationScope allocationScope)
{
	void *pMemory = realloc(pOriginal, size);
	return pMemory;
}

int enumerate_instance_extensions(VkExtensionProperties **vk_instance_extension_properties,
			 uint32_t *extension_count)
{
	VkResult res = VK_SUCCESS;

	res = vkEnumerateInstanceExtensionProperties(NULL, extension_count,
						     NULL);
	if (res != VK_SUCCESS) {
		fprintf(stderr,
			"Failed to acquire extension count. Error code: %d\n",
			res);
		return 0;
	}

	*vk_instance_extension_properties =
		malloc(sizeof(VkExtensionProperties) * (*extension_count));
	if (!(*vk_instance_extension_properties)) {
		fprintf(stderr, "Failed to allocate memory for extensions\n");
		return 0;
	}

	res = vkEnumerateInstanceExtensionProperties(NULL, extension_count,
						     *vk_instance_extension_properties);
	if (res != VK_SUCCESS) {
		fprintf(stderr,
			"Failed to enumerate extensions: Error code: %d\n",
			res);
		free(*vk_instance_extension_properties);
		return 0;
	}

	return 1;
}

int enumerate_instance_layers(VkLayerProperties **vk_instance_layer_properties,
		     uint32_t *layer_count)
{
	VkResult res = VK_SUCCESS;

	res = vkEnumerateInstanceLayerProperties(layer_count, NULL);
	if (res != VK_SUCCESS) {
		fprintf(stderr,
			"Failed to acquire layer count. Error code: %d\n", res);
		return 0;
	}

	*vk_instance_layer_properties =
		malloc(sizeof(VkLayerProperties) * (*layer_count));
	if (!(*vk_instance_layer_properties)) {
		fprintf(stderr, "Failed to allocate memory for layers\n");
		return 0;
	}

	res = vkEnumerateInstanceLayerProperties(layer_count,
						 *vk_instance_layer_properties);
	if (res != VK_SUCCESS) {
		fprintf(stderr, "Failed to enumerate layers. Error code: %d\n",
			res);
		free(*vk_instance_layer_properties);
		return 0;
	}
	return 1;
}

void print_device_extensions(uint32_t extension_count,
		      VkExtensionProperties *vk_extension_properties)
{
	for (uint32_t i = 0; i < extension_count; i++) {
		printf("- %s (version %d)\n",
		       (vk_extension_properties)[i].extensionName,
		       (vk_extension_properties)[i].specVersion);
	}
}

void print_device_layers(uint32_t layer_count, VkLayerProperties *vk_layer_properties)
{
	for (uint32_t i = 0; i < layer_count; i++) {
		printf("- %s (version %d)\n", vk_layer_properties[i].layerName,
		       vk_layer_properties[i].specVersion);
		printf("  Description: %s\n",
		       vk_layer_properties[i].description);
	}
}

int enable_instance_extensions(
	const VkExtensionProperties *vk_extension_properties,
	uint32_t vk_extension_count, const char *desired_extensions[],
	const uint32_t desired_extension_count,
	const char *enabled_extensions[])
{
	uint32_t enabled_extension_count = 0;

	for (uint32_t i = 0; i < desired_extension_count; i++) {
		for (uint32_t j = 0; j < vk_extension_count; j++) {
			if (strcmp(desired_extensions[i],
				   vk_extension_properties[j].extensionName) ==
			    0) {
				printf("Loop: %s\n",
				       vk_extension_properties[j].extensionName);
				enabled_extensions[enabled_extension_count++] =
					desired_extensions[i];
				break;
			}
		}
	}

	return enabled_extension_count;
}

int enable_instance_layers(const VkLayerProperties *vk_layer_properties,
			   uint32_t vk_layer_count,
			   const char *desired_layers[],
			   uint32_t desired_layer_count,
			   const char *enabled_layers[])
{
	uint32_t enabled_layer_count = 0;

	for (uint32_t i = 0; i < desired_layer_count; i++) {
		for (uint32_t j = 0; j < vk_layer_count; j++) {
			if (strcmp(desired_layers[i],
				   vk_layer_properties[j].layerName) == 0) {
				printf("Loop: %s\n",
				       vk_layer_properties[j].layerName);
				enabled_layers[enabled_layer_count++] =
					desired_layers[i];
				break;
			}
		}
	}

	return enabled_layer_count;
}

void print_physical_device_properties(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);

	printf("  Name: %s\n", properties.deviceName);
	printf("  API Version: %d.%d.%d\n",
	       VK_VERSION_MAJOR(properties.apiVersion),
	       VK_VERSION_MINOR(properties.apiVersion),
	       VK_VERSION_PATCH(properties.apiVersion));
	printf("  Driver Version: %d\n", properties.driverVersion);
	printf("  Vendor ID: %d\n", properties.vendorID);
	printf("  Device ID: %d\n", properties.deviceID);
	printf("  Device Type: %d\n", properties.deviceType);
	printf("\n");
}

void print_physical_device_features(VkPhysicalDevice vk_physical_device)
{
	VkPhysicalDeviceFeatures vk_physical_device_features;
	vkGetPhysicalDeviceFeatures(vk_physical_device,
				    &vk_physical_device_features);
	printf("Physical Device Features:\n");
	printf("robustBufferAccess: %d\n",
	       vk_physical_device_features.robustBufferAccess);
	printf("fullDrawIndexUint32: %d\n",
	       vk_physical_device_features.fullDrawIndexUint32);
	printf("imageCubeArray: %d\n",
	       vk_physical_device_features.imageCubeArray);
	printf("independentBlend: %d\n",
	       vk_physical_device_features.independentBlend);
	printf("geometryShader: %d\n",
	       vk_physical_device_features.geometryShader);
	printf("tessellationShader: %d\n",
	       vk_physical_device_features.tessellationShader);
	printf("sampleRateShading: %d\n",
	       vk_physical_device_features.sampleRateShading);
	printf("dualSrcBlend: %d\n", vk_physical_device_features.dualSrcBlend);
	printf("logicOp: %d\n", vk_physical_device_features.logicOp);
	printf("multiDrawIndirect: %d\n",
	       vk_physical_device_features.multiDrawIndirect);
	printf("drawIndirectFirstInstance: %d\n",
	       vk_physical_device_features.drawIndirectFirstInstance);
	printf("depthClamp: %d\n", vk_physical_device_features.depthClamp);
	printf("depthBiasClamp: %d\n",
	       vk_physical_device_features.depthBiasClamp);
	printf("fillModeNonSolid: %d\n",
	       vk_physical_device_features.fillModeNonSolid);
	printf("depthBounds: %d\n", vk_physical_device_features.depthBounds);
	printf("wideLines: %d\n", vk_physical_device_features.wideLines);
	printf("largePoints: %d\n", vk_physical_device_features.largePoints);
	printf("alphaToOne: %d\n", vk_physical_device_features.alphaToOne);
	printf("multiViewport: %d\n",
	       vk_physical_device_features.multiViewport);
	printf("samplerAnisotropy: %d\n",
	       vk_physical_device_features.samplerAnisotropy);
	printf("textureCompressionETC2: %d\n",
	       vk_physical_device_features.textureCompressionETC2);
	printf("textureCompressionASTC_LDR: %d\n",
	       vk_physical_device_features.textureCompressionASTC_LDR);
	printf("textureCompressionBC: %d\n",
	       vk_physical_device_features.textureCompressionBC);
	printf("occlusionQueryPrecise: %d\n",
	       vk_physical_device_features.occlusionQueryPrecise);
	printf("pipelineStatisticsQuery: %d\n",
	       vk_physical_device_features.pipelineStatisticsQuery);
	printf("vertexPipelineStoresAndAtomics: %d\n",
	       vk_physical_device_features.vertexPipelineStoresAndAtomics);
	printf("fragmentStoresAndAtomics: %d\n",
	       vk_physical_device_features.fragmentStoresAndAtomics);
	printf("shaderTessellationAndGeometryPointSize: %d\n",
	       vk_physical_device_features
		       .shaderTessellationAndGeometryPointSize);
	printf("shaderImageGatherExtended: %d\n",
	       vk_physical_device_features.shaderImageGatherExtended);
	printf("shaderStorageImageExtendedFormats: %d\n",
	       vk_physical_device_features.shaderStorageImageExtendedFormats);
	printf("shaderStorageImageMultisample: %d\n",
	       vk_physical_device_features.shaderStorageImageMultisample);
	printf("shaderStorageImageReadWithoutFormat: %d\n",
	       vk_physical_device_features.shaderStorageImageReadWithoutFormat);
	printf("shaderStorageImageWriteWithoutFormat: %d\n",
	       vk_physical_device_features.shaderStorageImageWriteWithoutFormat);
	printf("shaderUniformBufferArrayDynamicIndexing: %d\n",
	       vk_physical_device_features
		       .shaderUniformBufferArrayDynamicIndexing);
	printf("shaderSampledImageArrayDynamicIndexing: %d\n",
	       vk_physical_device_features
		       .shaderSampledImageArrayDynamicIndexing);
	printf("shaderStorageBufferArrayDynamicIndexing: %d\n",
	       vk_physical_device_features
		       .shaderStorageBufferArrayDynamicIndexing);
	printf("shaderStorageImageArrayDynamicIndexing: %d\n",
	       vk_physical_device_features
		       .shaderStorageImageArrayDynamicIndexing);
	printf("shaderClipDistance: %d\n",
	       vk_physical_device_features.shaderClipDistance);
	printf("shaderCullDistance: %d\n",
	       vk_physical_device_features.shaderCullDistance);
	printf("shaderFloat64: %d\n",
	       vk_physical_device_features.shaderFloat64);
	printf("shaderInt64: %d\n", vk_physical_device_features.shaderInt64);
	printf("shaderInt16: %d\n", vk_physical_device_features.shaderInt16);
	printf("shaderResourceResidency: %d\n",
	       vk_physical_device_features.shaderResourceResidency);
	printf("shaderResourceMinLod: %d\n",
	       vk_physical_device_features.shaderResourceMinLod);
	printf("sparseBinding: %d\n",
	       vk_physical_device_features.sparseBinding);
	printf("sparseResidencyBuffer: %d\n",
	       vk_physical_device_features.sparseResidencyBuffer);
	printf("sparseResidencyImage2D: %d\n",
	       vk_physical_device_features.sparseResidencyImage2D);
	printf("sparseResidencyImage3D: %d\n",
	       vk_physical_device_features.sparseResidencyImage3D);
	printf("sparseResidency2Samples: %d\n",
	       vk_physical_device_features.sparseResidency2Samples);
	printf("sparseResidency4Samples: %d\n",
	       vk_physical_device_features.sparseResidency4Samples);
	printf("sparseResidency8Samples: %d\n",
	       vk_physical_device_features.sparseResidency8Samples);
	printf("sparseResidency16Samples: %d\n",
	       vk_physical_device_features.sparseResidency16Samples);
	printf("sparseResidencyAliased: %d\n",
	       vk_physical_device_features.sparseResidencyAliased);
	printf("variableMultisampleRate: %d\n",
	       vk_physical_device_features.variableMultisampleRate);
	printf("inheritedQueries: %d\n",
	       vk_physical_device_features.inheritedQueries);
}

void print_memory_properties(VkPhysicalDevice vk_physical_device)
{
	VkPhysicalDeviceMemoryProperties vk_physical_device_memory_properties = {
		0
	};
	vkGetPhysicalDeviceMemoryProperties(
		vk_physical_device, &vk_physical_device_memory_properties);
	printf("Memory Properties:\n");
	printf("  Memory Type Count: %u\n",
	       vk_physical_device_memory_properties.memoryTypeCount);

	for (uint32_t i = 0;
	     i < vk_physical_device_memory_properties.memoryTypeCount; i++) {
		printf("  Memory Type %u:\n", i);
		printf("    Heap Index: %u\n",
		       vk_physical_device_memory_properties.memoryTypes[i]
			       .heapIndex);
		printf("    Property Flags:\n");

		if (vk_physical_device_memory_properties.memoryTypes[i]
			    .propertyFlags &
		    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			printf("      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT\n");
		if (vk_physical_device_memory_properties.memoryTypes[i]
			    .propertyFlags &
		    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			printf("      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT\n");
		if (vk_physical_device_memory_properties.memoryTypes[i]
			    .propertyFlags &
		    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
			printf("      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT\n");
		if (vk_physical_device_memory_properties.memoryTypes[i]
			    .propertyFlags &
		    VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
			printf("      VK_MEMORY_PROPERTY_HOST_CACHED_BIT\n");
		if (vk_physical_device_memory_properties.memoryTypes[i]
			    .propertyFlags &
		    VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
			printf("      VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT\n");
		if (vk_physical_device_memory_properties.memoryTypes[i]
			    .propertyFlags &
		    VK_MEMORY_PROPERTY_PROTECTED_BIT)
			printf("      VK_MEMORY_PROPERTY_PROTECTED_BIT\n");
	}

	printf("  Memory Heap Count: %u\n",
	       vk_physical_device_memory_properties.memoryHeapCount);

	for (uint32_t i = 0;
	     i < vk_physical_device_memory_properties.memoryHeapCount; i++) {
		printf("  Memory Heap %u:\n", i);
		printf("    Size: %llu\n",
		       (unsigned long long)vk_physical_device_memory_properties
			       .memoryHeaps[i]
			       .size);
		printf("    Flags:\n");

		if (vk_physical_device_memory_properties.memoryHeaps[i].flags &
		    VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			printf("      VK_MEMORY_HEAP_DEVICE_LOCAL_BIT\n");
		if (vk_physical_device_memory_properties.memoryHeaps[i].flags &
		    VK_MEMORY_HEAP_MULTI_INSTANCE_BIT)
			printf("      VK_MEMORY_HEAP_MULTI_INSTANCE_BIT\n");
	}
}

void print_queue_family_properties(VkPhysicalDevice physicalDevice)
{
	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,
						 &queue_family_count, NULL);

	VkQueueFamilyProperties *queue_family_properties =
		(VkQueueFamilyProperties *)malloc(
			sizeof(VkQueueFamilyProperties) * queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(
		physicalDevice, &queue_family_count, queue_family_properties);

	printf("Queue Family Properties:\n");
	for (uint32_t i = 0; i < queue_family_count; i++) {
		printf("  Queue Family %u:\n", i);
		printf("    Queue Count: %u\n",
		       queue_family_properties[i].queueCount);
		printf("    Queue Flags:\n");

		if (queue_family_properties[i].queueFlags &
		    VK_QUEUE_GRAPHICS_BIT)
			printf("      VK_QUEUE_GRAPHICS_BIT\n");
		if (queue_family_properties[i].queueFlags &
		    VK_QUEUE_COMPUTE_BIT)
			printf("      VK_QUEUE_COMPUTE_BIT\n");
		if (queue_family_properties[i].queueFlags &
		    VK_QUEUE_TRANSFER_BIT)
			printf("      VK_QUEUE_TRANSFER_BIT\n");
		if (queue_family_properties[i].queueFlags &
		    VK_QUEUE_SPARSE_BINDING_BIT)
			printf("      VK_QUEUE_SPARSE_BINDING_BIT\n");
		if (queue_family_properties[i].queueFlags &
		    VK_QUEUE_PROTECTED_BIT)
			printf("      VK_QUEUE_PROTECTED_BIT\n");

		printf("    Timestamp Valid Bits: %u\n",
		       queue_family_properties[i].timestampValidBits);
		printf("    Min Image Transfer Granularity:\n");
		printf("      Width: %u\n",
		       queue_family_properties[i]
			       .minImageTransferGranularity.width);
		printf("      Height: %u\n",
		       queue_family_properties[i]
			       .minImageTransferGranularity.height);
		printf("      Depth: %u\n",
		       queue_family_properties[i]
			       .minImageTransferGranularity.depth);
	}

	free(queue_family_properties);
}

int main()
{
	VkAllocationCallbacks wymux_callbacks = { 0 };
	wymux_callbacks.pUserData = NULL;
	wymux_callbacks.pfnAllocation = wymux_allocation;
	wymux_callbacks.pfnReallocation = wymux_reallocation;
	wymux_callbacks.pfnFree = wymux_free;
	wymux_callbacks.pfnInternalAllocation = NULL;
	wymux_callbacks.pfnInternalFree = NULL;

	VkExtensionProperties *vk_extension_properties = NULL;
	uint32_t extension_count = 0;
	if (enumerate_instance_extensions(&vk_extension_properties, &extension_count)) {
		printf("Available Vulkan Extensions:\n");
		print_device_extensions(extension_count, vk_extension_properties);
	}

	VkLayerProperties *vk_layer_properties = NULL;
	uint32_t instance_layer_count = 0;
	if (enumerate_instance_layers(&vk_layer_properties, &instance_layer_count)) {
		printf("Available Vulkan Layers:\n");
		print_device_layers(instance_layer_count, vk_layer_properties);
	}

	VkApplicationInfo vk_app_info = {};
	vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vk_app_info.pNext = NULL;
	vk_app_info.pApplicationName = "Vulkan Programming Guide";
	vk_app_info.applicationVersion = VK_MAKE_VERSION(1, 3, 0);
	vk_app_info.pEngineName = "NaN";
	vk_app_info.engineVersion = VK_MAKE_VERSION(1, 3, 0);
	vk_app_info.apiVersion = VK_MAKE_VERSION(1, 3, 0);

	const char *desired_instance_layers[] = {
		"VK_LAYER_KHRONOS_validation",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_LUNARG_api_dump", "VK_LAYER_NV_optimus"
	};

	const uint32_t desired_layer_count = sizeof(desired_instance_layers) /
					     sizeof(desired_instance_layers[0]);

	const char *enabled_instance_layers[desired_layer_count];
	uint32_t enabled_layer_count = enable_instance_layers(
		vk_layer_properties, instance_layer_count,
		desired_instance_layers, desired_layer_count,
		enabled_instance_layers);

	const char *desired_instance_extensions[] = {
		"VK_KHR_device_group_creation",
		"VK_KHR_display",
		"VK_KHR_external_fence_capabilities",
		"VK_KHR_external_memory_capabilities",
		"VK_KHR_external_semaphore_capabilities",
	};
	const uint32_t desired_extensions_count =
		sizeof(desired_instance_extensions) /
		sizeof(desired_instance_extensions[0]);
	const char *enabled_instance_extensions[desired_extensions_count];

	uint32_t enabled_extension_count = enable_instance_extensions(
		vk_extension_properties, extension_count,
		desired_instance_extensions, desired_extensions_count,
		enabled_instance_extensions);

	VkInstanceCreateInfo vk_instance_create_info = {};
	vk_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vk_instance_create_info.pNext = NULL;
	vk_instance_create_info.flags = 0;
	vk_instance_create_info.pApplicationInfo = &vk_app_info;
	vk_instance_create_info.enabledLayerCount = enabled_layer_count;
	vk_instance_create_info.ppEnabledLayerNames = enabled_instance_layers;
	vk_instance_create_info.enabledExtensionCount = enabled_extension_count;
	vk_instance_create_info.ppEnabledExtensionNames =
		enabled_instance_extensions;

	VkResult res = VK_SUCCESS;
	VkInstance vk_instance = VK_NULL_HANDLE;

	res = vkCreateInstance(&vk_instance_create_info, &wymux_callbacks,
			       &vk_instance);

	if (res != VK_SUCCESS) {
		fprintf(stderr, "Instance Creation Failure: error code: %d\n",
			res);
	}

	uint32_t physical_device_count = 0;
	vkEnumeratePhysicalDevices(vk_instance, &physical_device_count, NULL);
	VkPhysicalDevice *vk_physical_devices =
		malloc(sizeof(VkPhysicalDevice) * physical_device_count);
	vkEnumeratePhysicalDevices(vk_instance, &physical_device_count,
				   vk_physical_devices);

	/* Print all properties: no side effects */
	for (uint32_t i = 0; i < physical_device_count; i++) {
		printf("Device %d:\n", i);
		print_physical_device_properties(vk_physical_devices[i]);
		print_physical_device_features(vk_physical_devices[i]);
		print_memory_properties(vk_physical_devices[i]);
		print_queue_family_properties(vk_physical_devices[i]);
	}

	VkPhysicalDeviceFeatures vk_physical_device_features;
	vkGetPhysicalDeviceFeatures(vk_physical_devices[GPU_INDEX], &vk_physical_device_features);

	uint32_t queue_family_index = 0;

	VkDeviceQueueCreateInfo queue_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = queue_family_index,
		.queueCount = 1,
		.pQueuePriorities = (float[]){ 1.0f }
	};

	VkDeviceCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queue_create_info,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
		.enabledExtensionCount = 0,
		.ppEnabledExtensionNames = NULL,
		.pEnabledFeatures = &vk_physical_device_features,
	};
	
	free(vk_physical_devices);
	free(vk_extension_properties);
	free(vk_layer_properties);
	vkDestroyInstance(vk_instance, &wymux_callbacks);
}
