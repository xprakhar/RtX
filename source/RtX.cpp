#include "RtX.h"

GLFWwindow* Window;
VkInstance Instance;
VkDebugUtilsMessengerEXT DebugMessenger;
VkSurfaceKHR Surface;
VkPhysicalDevice PhysicalDevice;

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallbackFunc(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageTypes,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
) {

	std::stringstream message;

	message << string_VkDebugUtilsMessageSeverityFlagBitsEXT(messageSeverity) << ": "
		<< string_VkDebugUtilsMessageTypeFlagsEXT(messageTypes) << ":\n";
	message << std::string("\t") << "messageIDName   = <" << pCallbackData->pMessageIdName << ">\n";
	message << std::string("\t") << "messageIdNumber = " << pCallbackData->messageIdNumber << "\n";
	message << std::string("\t") << "message         = <" << pCallbackData->pMessage << ">\n";
	if (0 < pCallbackData->queueLabelCount) {
		message << std::string("\t") << "Queue Labels:\n";
		for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++)
		{
			message << std::string("\t\t") << "labelName = <" << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
		}
	}
	if (0 < pCallbackData->cmdBufLabelCount) {
		message << std::string("\t") << "CommandBuffer Labels:\n";
		for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++)
		{
			message << std::string("\t\t") << "labelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
		}
	}
	if (0 < pCallbackData->objectCount) {
		message << std::string("\t") << "Objects:\n";
		for (uint32_t i = 0; i < pCallbackData->objectCount; i++)
		{
			message << std::string("\t\t") << "Object " << i << "\n";
			message << std::string("\t\t\t") << "objectType   = " << string_VkObjectType(pCallbackData->pObjects[i].objectType) << "\n";
			message << std::string("\t\t\t") << "objectHandle = " << pCallbackData->pObjects[i].objectHandle << "\n";
			if (pCallbackData->pObjects[i].pObjectName)
			{
				message << std::string("\t\t\t") << "objectName   = <" << pCallbackData->pObjects[i].pObjectName << ">\n";
			}
		}
	}

	std::cerr << message.str() << std::endl;

	return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& info) {
	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	info.pNext = nullptr;
	info.flags = 0;
	info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
	info.pfnUserCallback = debugCallbackFunc;
	info.pUserData = nullptr;
}

VkInstance createInstance() {

	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pApplicationName = "RtX";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkDebugUtilsMessengerCreateInfoEXT messengerInfo;
	populateDebugMessengerCreateInfo(messengerInfo);

	uint32_t extensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
	std::vector<const char*> extensionNames{ glfwExtensions, glfwExtensions + extensionCount };
	
	extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	std::vector<const char*> layerNames{ "VK_LAYER_KHRONOS_validation" };
	
	VkInstanceCreateInfo instanceInfo;
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = &messengerInfo;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = static_cast<uint32_t>(layerNames.size());
	instanceInfo.ppEnabledLayerNames = layerNames.data();
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());
	instanceInfo.ppEnabledExtensionNames = extensionNames.data();
	
	VkInstance instance;
	VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &instance));

	return instance;
}

bool isDeviceSuitable(VkPhysicalDevice) {
	return true;
}

VkPhysicalDevice selectPhysicalDevice() {

	uint32_t deviceCount;
	VK_CHECK(vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr));
	assert(deviceCount >= 0 && "An error occurred while enumerating physical devices.");
	std::vector<VkPhysicalDevice> devices(deviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(Instance, &deviceCount, devices.data()));

	for (const auto device : devices) {
		if (isDeviceSuitable(device)) {
			return device;
		}
	}

	return VK_NULL_HANDLE;
}

void initializeRtX() {
	
	assert(glfwInit() != GLFW_FALSE && "Failed to initialize glfw.");
	assert(glfwVulkanSupported() != GLFW_FALSE && "Failed to find a suitable vulkan loader.");

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	Window = glfwCreateWindow(1280, 720, "RtX", nullptr, nullptr);
	assert(Window != nullptr && "Failed create native window.");

	VK_CHECK(volkInitialize());
	
	Instance = createInstance();
	volkLoadInstanceOnly(Instance);

	VkDebugUtilsMessengerCreateInfoEXT messengerInfo;
	populateDebugMessengerCreateInfo(messengerInfo);
	VK_CHECK(vkCreateDebugUtilsMessengerEXT(Instance, &messengerInfo, nullptr, &DebugMessenger));

	VK_CHECK(glfwCreateWindowSurface(Instance, Window, nullptr, &Surface));

	PhysicalDevice = selectPhysicalDevice();
}

void terminateRtX() {
	vkDestroyInstance(Instance, nullptr);
	glfwDestroyWindow(Window);
	glfwTerminate();
}

int main() {
	initializeRtX();

	while (!glfwWindowShouldClose(Window)) {
		glfwPollEvents();
	}

	terminateRtX();

	return 0;
}
