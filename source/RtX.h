#pragma once

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif // VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#ifndef VOLK_IMPLEMENTATION
#define VOLK_IMPLEMENTATION
#endif // VOLK_IMPLEMENTATION
#include <volk/volk.h>

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif // GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VK_CHECK(operation) \
 do { \
	VkResult _result_ = operation; \
	if (_result_ != VK_SUCCESS) { \
		std::cerr << "Operation Failed " << #operation << std::endl;\
		std::cerr << "Error code : " << _result_ << std::endl;\
		std::cerr << "Error Description : " << string_VkResult(_result_) << std::endl;\
		std::cerr << "File : " << __FILE__ << std::endl;\
		std::cerr << "Line : " << __LINE__ << std:: endl;\
	} \
 } while (false)

#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>