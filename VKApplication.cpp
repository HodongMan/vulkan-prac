#include "VKApplication.h"


VKApplication::VKApplication( void )
	: _window{ nullptr }
	, _vkInstance{ nullptr }
	, _physicalDevice{ VK_NULL_HANDLE  }
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
	if ( false == createVKInstance() )
	{
		return false;
	}

	if ( false == pickPhysicalDevice() )
	{
		return false;
	}

	if ( false == createLogicalDevice() )
	{
		return false;
	}

	return true;
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

bool VKApplication::pickPhysicalDevice( void ) noexcept
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices( _vkInstance, &deviceCount, nullptr );

	if ( 0 == deviceCount )
	{
		return false;
	}

	std::vector<VkPhysicalDevice> devices( deviceCount );
	vkEnumeratePhysicalDevices( _vkInstance, &deviceCount, devices.data() );

	for ( const auto& device : devices )
	{
		if ( true == isDeviceSuitable( device ) )
		{
			_physicalDevice = device;
			break;
		}
	}

	if ( VK_NULL_HANDLE == _physicalDevice )
	{
		return false;
	}
}

bool VKApplication::isDeviceSuitable( const VkPhysicalDevice device ) const noexcept
{
	QueueFamilyIndices indices = findQueueFamilies( device );

	return indices.isComplete();
}

QueueFamilyIndices VKApplication::findQueueFamilies( const VkPhysicalDevice device ) const noexcept
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, nullptr );

	std::vector<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );
	vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, queueFamilies.data() );

	int ii = 0;
    for ( const auto& queueFamily : queueFamilies )
	{
        if ( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
		{
            indices._graphicsFamily = ii;
        }

        if ( true == indices.isComplete() )
		{
            break;
        }

        ii++;
    }

	return indices;
}

bool VKApplication::createLogicalDevice( void ) noexcept
{
	QueueFamilyIndices indices = findQueueFamilies( _physicalDevice );
	
	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex	= indices._graphicsFamily.value();
	queueCreateInfo.queueCount			= 1;

	const float queuePriority			= 1.0f;
	queueCreateInfo.pQueuePriorities	= &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos		= &queueCreateInfo;
	createInfo.queueCreateInfoCount		= 1;
	createInfo.pEnabledFeatures			= &deviceFeatures;
	createInfo.enabledExtensionCount	= 0;
	createInfo.enabledLayerCount		= 0;

	if ( VK_SUCCESS != vkCreateDevice( _physicalDevice, &createInfo, nullptr, &_device ) )
	{
		return false;
	}

	vkGetDeviceQueue( _device, indices._graphicsFamily.value(), 0, &_graphicsQueue );

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

void VKApplication::runLoop( void ) const noexcept
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
