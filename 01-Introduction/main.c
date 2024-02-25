#include <stdlib.h>
#include <stdio.h>

#include <vulkan/vulkan.h>

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
		perror("vkEnumerateInstance2wssssssExtensionProperties()");
		return -1;
	}
	if (instance_extension_count != 0) {
		VkExtensionProperties *instance_extension_properties =
			calloc(instance_extension_count,
			       sizeof(VkExtensionProperties));
		vkEnumerateInstanceExtensionProperties(
			NULL, &instance_extension_count,
			instance_extension_properties);
		for (int i = 0; i < instance_extension_count; i++)
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

	VkInstance instance;
	result = vkCreateInstance(&create_info, NULL, &instance);
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

	return 0;
}
