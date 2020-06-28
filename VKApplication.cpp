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

	if ( false == createSwapChain() )
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

	auto extensions						= getRequiredExtensions();
	createInfo.enabledExtensionCount	= static_cast<uint32_t>( extensions.size() );
	createInfo.ppEnabledExtensionNames	= extensions.data();

	createInfo.enabledLayerCount		= 0;
	createInfo.pNext					= nullptr;

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
	QueueFamilyIndices indices	= findQueueFamilies( device );

	bool extensionsSupported	= checkDeviceExtensionSupport( device );

	bool swapChainAdequate = false;
	if ( true == extensionsSupported) 
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate		= ( false == swapChainSupport._formats.empty() ) && ( false == swapChainSupport._presentModes.empty() );
	}

	return indices.isComplete() && ( true == extensionsSupported ) && ( true == swapChainAdequate );
}

bool VKApplication::checkDeviceExtensionSupport( const VkPhysicalDevice device ) const noexcept
{
	uint32_t extensionCount = 0;

	vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );

	std::vector<VkExtensionProperties> availableExtensions( extensionCount );
	vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, availableExtensions.data() );

	std::set<std::string> requiredExtensions( deviceExtensions.begin(), deviceExtensions.end() );

	for ( const auto& extension : availableExtensions )
	{
		requiredExtensions.erase( extension.extensionName );
	}

	return requiredExtensions.empty();
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

std::vector<const char*> VKApplication::getRequiredExtensions( void ) const noexcept
{
	uint32_t glfwExtensionCount		= 0;
	const char** glfwExtensions		= nullptr;
	glfwExtensions					= glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

	std::vector<const char*> extensions( glfwExtensions, glfwExtensions + glfwExtensionCount );

	return extensions;
}

SwapChainSupportDetails VKApplication::querySwapChainSupport( const VkPhysicalDevice device ) const noexcept
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, _surface, &details._capabilities );

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR( device, _surface, &formatCount, nullptr );

	if ( 0 != formatCount )
	{
		details._formats.resize( formatCount );
		vkGetPhysicalDeviceSurfaceFormatsKHR( device, _surface, &formatCount, details._formats.data() );
	}

	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR( device, _surface, &presentModeCount, nullptr );

	if ( 0 != presentModeCount )
	{
		details._presentModes.resize( presentModeCount );
		vkGetPhysicalDeviceSurfacePresentModesKHR( device, _surface, &presentModeCount, details._presentModes.data() );
	}

	return details;
}

VkSurfaceFormatKHR VKApplication::chooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& availableFormats ) const noexcept
{
	for ( const auto& availableFormat : availableFormats )
	{
		if ( ( VK_FORMAT_B8G8R8A8_SRGB == availableFormat.format ) && 
			 ( VK_COLOR_SPACE_SRGB_NONLINEAR_KHR == availableFormat.colorSpace ) )
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR VKApplication::chooseSwapPresentMode( const std::vector<VkPresentModeKHR>& availablePresentModes ) const noexcept
{
	for ( const auto& availablePresentMode : availablePresentModes )
	{
		if ( VK_PRESENT_MODE_MAILBOX_KHR == availablePresentMode ) 
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VKApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities) const noexcept
{
	if ( UINT32_MAX != capabilities.currentExtent.width ) 
	{
		return capabilities.currentExtent;
	}

	VkExtent2D actualExtent = { WIDTH, HEIGHT };

	actualExtent.width = std::max( capabilities.minImageExtent.width, std::min( capabilities.maxImageExtent.width, actualExtent.width ) );
	actualExtent.height = std::max( capabilities.minImageExtent.height, std::min( capabilities.maxImageExtent.height, actualExtent.height ) );

	return actualExtent;
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
	createInfo.enabledExtensionCount			= static_cast<uint32_t>( deviceExtensions.size() );
	createInfo.ppEnabledExtensionNames			= deviceExtensions.data();
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

bool VKApplication::createSwapChain( void ) noexcept
{
	SwapChainSupportDetails swapChainSupport	= querySwapChainSupport( _physicalDevice );

	VkSurfaceFormatKHR surfaceFormat			= chooseSwapSurfaceFormat( swapChainSupport._formats );
	VkPresentModeKHR presentMode				= chooseSwapPresentMode( swapChainSupport._presentModes );
	VkExtent2D extent							= chooseSwapExtent( swapChainSupport._capabilities );

	uint32_t imageCount							= swapChainSupport._capabilities.minImageCount + 1;
	if ( ( 0 < swapChainSupport._capabilities.maxImageCount ) && 
		 ( swapChainSupport._capabilities.maxImageCount < imageCount ) )
	{
		imageCount = swapChainSupport._capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType							= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface							= _surface;

	createInfo.minImageCount					= imageCount;
	createInfo.imageFormat						= surfaceFormat.format;
	createInfo.imageColorSpace					= surfaceFormat.colorSpace;
	createInfo.imageExtent						= extent;
	createInfo.imageArrayLayers					= 1;
	createInfo.imageUsage						= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices					= findQueueFamilies( _physicalDevice );
	uint32_t queueFamilyIndices[]				= { indices._graphicsFamily.value(), indices._presentFamily.value() };

	if ( indices._graphicsFamily != indices._presentFamily )
	{
		createInfo.imageSharingMode				= VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount		= 2;
		createInfo.pQueueFamilyIndices			= queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode				= VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform						= swapChainSupport._capabilities.currentTransform;
	createInfo.compositeAlpha					= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode						= presentMode;
	createInfo.clipped							= VK_TRUE;

	createInfo.oldSwapchain						= VK_NULL_HANDLE;


	if ( VK_SUCCESS != vkCreateSwapchainKHR( _device, &createInfo, nullptr, &_swapchain ) ) 
	{
		return false;
	}

	vkGetSwapchainImagesKHR( _device, _swapchain, &imageCount, nullptr );
	_swapChainImages.resize( imageCount );
	vkGetSwapchainImagesKHR( _device, _swapchain, &imageCount, _swapChainImages.data() );

	_swapChainImageFormat						= surfaceFormat.format;
	_swapChainExtent							= extent;

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
	vkDestroySwapchainKHR( _device, _swapchain, nullptr );
	vkDeviceWaitIdle( _device );

	vkDestroyDevice( _device, nullptr );
	vkDestroySurfaceKHR( _vkInstance, _surface, nullptr );
	vkDestroyInstance( _vkInstance, nullptr );

	glfwDestroyWindow( _window );
	glfwTerminate();
}
