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

	if ( false == createSurface() )
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

	return true;
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

		VkBool32 isPresentSupport		= false;
		vkGetPhysicalDeviceSurfaceSupportKHR( device, ii, _surface, &isPresentSupport );

		if ( true == isPresentSupport )
		{
			indices._presentFamily = ii;
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

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.clear();

    std::set<uint32_t> uniqueQueueFamilies		= { indices._graphicsFamily.value(), indices._presentFamily.value() };
	
	const float queuePriority = 1.0f;
	for ( uint32_t queueFamily : uniqueQueueFamilies )
	{
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex	= queueFamily;
            queueCreateInfo.queueCount			= 1;
            queueCreateInfo.pQueuePriorities	= &queuePriority;
            queueCreateInfos.push_back( queueCreateInfo );
    }

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType							= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount				= static_cast<uint32_t>( queueCreateInfos.size() );
    createInfo.pQueueCreateInfos				= queueCreateInfos.data();

	createInfo.pEnabledFeatures					= &deviceFeatures;
	createInfo.enabledExtensionCount			= 0;
	createInfo.enabledLayerCount				= 0;

	if ( VK_SUCCESS != vkCreateDevice( _physicalDevice, &createInfo, nullptr, &_device ) )
	{
		return false;
	}

	vkGetDeviceQueue( _device, indices._graphicsFamily.value(), 0, &_graphicsQueue );
	vkGetDeviceQueue( _device, indices._presentFamily.value(), 0, &_presentQueue );

	return true;
}

bool VKApplication::createSurface( void ) noexcept
{
	if ( VK_SUCCESS != glfwCreateWindowSurface( _vkInstance, _window, nullptr, &_surface ) )
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

void VKApplication::runLoop( void ) const noexcept
{
	while ( !glfwWindowShouldClose( _window ) )
	{
		glfwPollEvents();
	}
}

void VKApplication::clean( void ) noexcept
{
	vkDeviceWaitIdle( _device );

	vkDestroyDevice( _device, nullptr );
	vkDestroySurfaceKHR( _vkInstance, _surface, nullptr );
	vkDestroyInstance( _vkInstance, nullptr );

	glfwDestroyWindow( _window );
	glfwTerminate();
}
