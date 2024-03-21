#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

void *wymux_allocation(void *pUserData, size_t size, size_t alignment,
		       VkSystemAllocationScope allocationScope)
{
	void *pMemory = malloc(size);
	printf("Allocation: size = %zu, alignment = %zu, scope = %d\n", size,
	       alignment, allocationScope);
	return pMemory;
}

void wymux_free(void *pUserData, void *pMemory)
{
	printf("Free: pMemory = %p\n", pMemory);
	free(pMemory);
}

void *wymux_reallocation(void *pUserData, void *pOriginal, size_t size,
			 size_t alignment,
			 VkSystemAllocationScope allocationScope)
{
	void *pMemory = realloc(pOriginal, size);
	printf("Reallocation: pOriginal = %p, size = %zu, alignment = %zu, scope = %d\n",
	       pOriginal, size, alignment, allocationScope);
	return pMemory;
}

int enumerate_extensions(VkExtensionProperties **vk_extension_properties,
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

	*vk_extension_properties =
		malloc(sizeof(VkExtensionProperties) * (*extension_count));
	if (!(*vk_extension_properties)) {
		fprintf(stderr, "Failed to allocate memory for extensions\n");
		return 0;
	}

	res = vkEnumerateInstanceExtensionProperties(NULL, extension_count,
						     *vk_extension_properties);
	if (res != VK_SUCCESS) {
		fprintf(stderr,
			"Failed to enumerate extensions: Error code: %d\n",
			res);
		free(*vk_extension_properties);
		return 0;
	}

	return 1;
}

int enumerate_layers(VkLayerProperties **vk_layer_properties,
		     uint32_t *layer_count)
{
	VkResult res = VK_SUCCESS;

	res = vkEnumerateInstanceLayerProperties(layer_count, NULL);
	if (res != VK_SUCCESS) {
		fprintf(stderr,
			"Failed to acquire layer count. Error code: %d\n", res);
		return 0;
	}

	*vk_layer_properties =
		malloc(sizeof(VkLayerProperties) * (*layer_count));
	if (!(*vk_layer_properties)) {
		fprintf(stderr, "Failed to allocate memory for layers\n");
		return 0;
	}

	res = vkEnumerateInstanceLayerProperties(layer_count,
						 *vk_layer_properties);
	if (res != VK_SUCCESS) {
		fprintf(stderr, "Failed to enumerate layers. Error code: %d\n",
			res);
		free(*vk_layer_properties);
		return 0;
	}
	return 1;
}

void print_extensions(uint32_t extension_count,
		      VkExtensionProperties *vk_extension_properties)
{
	for (uint32_t i = 0; i < extension_count; i++) {
		printf("- %s (version %d)\n",
		       (vk_extension_properties)[i].extensionName,
		       (vk_extension_properties)[i].specVersion);
	}
}

void print_layers(uint32_t layer_count, VkLayerProperties *vk_layer_properties)
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

int main()
{
	VkAllocationCallbacks allocationCallbacks = { 0 };
	allocationCallbacks.pUserData = NULL;
	allocationCallbacks.pfnAllocation = wymux_allocation;
	allocationCallbacks.pfnReallocation = wymux_reallocation;
	allocationCallbacks.pfnFree = wymux_free;

	VkExtensionProperties *vk_extension_properties = NULL;
	uint32_t extension_count = 0;
	if (enumerate_extensions(&vk_extension_properties, &extension_count)) {
		printf("Available Vulkan Extensions:\n");
		print_extensions(extension_count, vk_extension_properties);
	}

	VkLayerProperties *vk_layer_properties = NULL;
	uint32_t instance_layer_count = 0;
	if (enumerate_layers(&vk_layer_properties, &instance_layer_count)) {
		printf("Available Vulkan Layers:\n");
		print_layers(instance_layer_count, vk_layer_properties);
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

	res = vkCreateInstance(&vk_instance_create_info, NULL, &vk_instance);

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

	for (uint32_t i = 0; i < physical_device_count; i++) {
		printf("Device %d:\n", i);
		print_physical_device_properties(vk_physical_devices[i]);
	}

	free(vk_physical_devices);
	free(vk_extension_properties);
	free(vk_layer_properties);
	vkDestroyInstance(vk_instance, NULL);
}
