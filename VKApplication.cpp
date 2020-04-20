#include "VKApplication.h"


VKApplication::VKApplication( void )
	: _window{ nullptr }
	, _vkInstance{ nullptr }
{

}

VKApplication::~VKApplication( void )
{

}

void VKApplication::run( void ) noexcept
{
	initializeWindow();
	initializeVKApplication();
	runLoop();
	clean();
}

bool VKApplication::initializeVKApplication( void ) noexcept
{
	return createVKInstance();
}

bool VKApplication::createVKInstance( void ) noexcept
{
	VkApplicationInfo appInfo{};

	appInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName	= "Hello Vulkan";
	appInfo.applicationVersion	= VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName			= "No Engine";
	appInfo.engineVersion		= VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion			= VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType			= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = nullptr;
	glfwExtensions				= glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

	createInfo.enabledExtensionCount	= glfwExtensionCount;
	createInfo.ppEnabledExtensionNames	= glfwExtensions;

	createInfo.enabledLayerCount		= 0;

	if ( VK_SUCCESS != vkCreateInstance( &createInfo, nullptr, &_vkInstance ) )
	{
		return false;
	}

	return true;
}

void VKApplication::initializeWindow( void ) noexcept
{
	const uint32_t WIDTH	= 800;
	const uint32_t HEIGHT	= 600;

	glfwInit();

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	_window = glfwCreateWindow( WIDTH, HEIGHT, "Vulkan", nullptr, nullptr );
}

void VKApplication::runLoop( void ) noexcept
{
	while ( !glfwWindowShouldClose( _window ) )
	{
		glfwPollEvents();
	}
}

void VKApplication::clean( void ) noexcept
{
	vkDestroyInstance( _vkInstance, nullptr );

	glfwDestroyWindow( _window );

	glfwTerminate();
}
