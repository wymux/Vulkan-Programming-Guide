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
	printf("Nomber of devices: %lu", device_count);
	VkPhysicalDevice *devices = malloc(device_count * sizeof(VkPhysicalDevice));
	result = vkEnumeratePhysicalDevices(instance, &device_count, devices);
	if (result != VK_SUCCESS) {
		perror("Error: vkEnumeratePhysicalDevices()");
		return -1;
	}
	
	return 0;
}
