#include <stdlib.h>
#include <stdio.h>

#include <vulkan/vulkan.h>

int
main()
{
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
	
	VkInstance instance;
	VkResult result = VK_SUCCESS;
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
	VkPhysicalDevice *devices = malloc(device_count * sizeof(VkPhysicalDevice));
	result = vkEnumeratePhysicalDevices(instance, &device_count, devices);
	if (result != VK_SUCCESS) {
		perror("Error: vkEnumeratePhysicalDevices()");
		return -1;
	}

	VkPhysicalDeviceProperties physical_properties;
	vkGetPhysicalDeviceProperties(devices[0], &physical_properties);
	printf("Device Name: %s %Lu %Lu %Lu\n",
	       physical_properties.deviceName,
	       physical_properties.apiVersion,
	       physical_properties.vendorID,
		physical_properties.deviceID);

	VkPhysicalDeviceFeatures physical_features;
	vkGetPhysicalDeviceFeatures(devices[0], &physical_features);

	VkPhysicalDeviceMemoryProperties physical_memory;
	vkGetPhysicalDeviceMemoryProperties(devices[0], &physical_memory);
	printf("Memory Properties: %Lu %Lu\n",
	       physical_memory.memoryTypeCount,
	       physical_memory.memoryHeapCount);

	uint32_t queue_family;
	vkGetPhysicalDeviceQueueFamilyProperties(
		devices[0], &queue_family, NULL);
	VkQueueFamilyProperties *family_properties =
		malloc(queue_family * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(
		devices[0], &queue_family, family_properties
		);
	
	return 0;
}
