#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

int enumerateextensions()
{
	uint32_t extension_count = 0;
	VkResult res = VK_SUCCESS;
	res = vkEnumerateInstanceExtensionProperties(NULL, &extension_count,
						     NULL);
	if (res != VK_SUCCESS) {
		fprintf(stderr,
			"Failed to acqoire extension count. Error code: %d\n",
			res);
		return 1;
	}

	VkExtensionProperties *vk_extension_properties =
		malloc(sizeof(VkExtensionProperties) * extension_count);
	if (!vk_extension_properties) {
		fprintf(stderr, "Failed to allocate memory for extansions\n");
		return 0;
	}

	res = vkEnumerateInstanceExtensionProperties(NULL, &extension_count,
						     vk_extension_properties);
	if (res != VK_SUCCESS) {
		fprintf(stderr,
			"Failed to enumerate extensions: Error code: %d\n",
			res);
		free(vk_extension_properties);
		return 0;
	}

	printf("Available Vulkan Extensions\n");
	for (uint32_t i = 0; i < extension_count; i++) {
		printf("- %s (version %d)\n",
		       vk_extension_properties[i].extensionName,
		       vk_extension_properties[i].specVersion);
	}

	free(vk_extension_properties);
	return 1;
}

int enumeratelayers()
{
	uint32_t layer_count = 0;
	VkResult res = VK_SUCCESS;

	res = vkEnumerateInstanceLayerProperties(&layer_count, NULL);
	VkLayerProperties *vk_layer_properties =
		malloc(sizeof(VkLayerProperties) * layer_count);
	if (res != VK_SUCCESS) {
		fprintf(stderr, "Failed to allocate memory\n");
		return 0;
	}

	res = vkEnumerateInstanceLayerProperties(&layer_count,
						 vk_layer_properties);
	if (res != VK_SUCCESS) {
		fprintf(stderr, "Failed to enumerate layers. Error code: %d\n",
			res);
		free(vk_layer_properties);
		return 0;
	}

	printf("Available Vulkan Layers:\n");
	for (uint32_t i = 0; i < layer_count; i++) {
		printf("- %s (version %d)\n", vk_layer_properties[i].layerName,
		       vk_layer_properties[i].specVersion);
		printf("  Description: %s\n",
		       vk_layer_properties[i].description);
	}

	free(vk_layer_properties);
	return 1;
}

int main()
{
	enumerateextensions();
	enumeratelayers();
	VkApplicationInfo vk_app_info = {};
	vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vk_app_info.pNext = NULL;
	vk_app_info.pApplicationName = "Vulkan Programming Guide";
	vk_app_info.applicationVersion = VK_MAKE_VERSION(1, 3, 0);
	vk_app_info.pEngineName = "NaN";
	vk_app_info.engineVersion = VK_MAKE_VERSION(1, 3, 0);
	vk_app_info.apiVersion = VK_MAKE_VERSION(1, 3, 0);

	const char *validation_layers[] = {
		"VK_LAYER_KHRONOS_validation",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_LUNARG_api_dump",
	};

	VkInstanceCreateInfo vk_instance_create_info = {};
	vk_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vk_instance_create_info.pNext = NULL;
	vk_instance_create_info.flags = 0;
	vk_instance_create_info.pApplicationInfo = &vk_app_info;
	vk_instance_create_info.enabledLayerCount = 0;
	vk_instance_create_info.ppEnabledLayerNames = NULL;
	vk_instance_create_info.enabledExtensionCount = 0;
	vk_instance_create_info.ppEnabledExtensionNames = NULL;

	VkResult res = VK_SUCCESS;
	VkInstance vk_instance = VK_NULL_HANDLE;

	res = vkCreateInstance(&vk_instance_create_info, NULL, &vk_instance);

	if (res != VK_SUCCESS) {
		fprintf(stderr, "Instance Creation Failure: error code: %d\n",
			res);
	}

	vkDestroyInstance(vk_instance, NULL);
}