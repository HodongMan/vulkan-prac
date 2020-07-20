#include "pch.h"

#include "VKApplication.h"
#include "File.h"


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

	if ( false == createImageViews() )
	{
		return false;
	}

	if ( false == createRenderPass() )
	{
		return false;
	}

	if ( false == createGraphicsPipeline() )
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

bool VKApplication::createImageViews( void ) noexcept
{
	_swapChainImageViews.resize( _swapChainImages.size() );
	const int count = static_cast<int>( _swapChainImages.size() );

	for ( int ii = 0; ii < count; ++ii)
	{
		VkImageViewCreateInfo createInfo{};
		
		createInfo.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image							= _swapChainImages[ii];
		createInfo.viewType							= VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format							= _swapChainImageFormat;
		createInfo.components.r						= VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g						= VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b						= VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a						= VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel	= 0;
		createInfo.subresourceRange.levelCount		= 1;
		createInfo.subresourceRange.baseArrayLayer	= 0;
		createInfo.subresourceRange.layerCount		= 1;

		if ( VK_SUCCESS != vkCreateImageView( _device, &createInfo, nullptr, &_swapChainImageViews[ii] ) ) 
		{
			return false;
		}
	}

	return true;
}

bool VKApplication::createRenderPass( void ) noexcept
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format				= _swapChainImageFormat;
	colorAttachment.samples				= VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp				= VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp				= VK_ATTACHMENT_STORE_OP_STORE;

	colorAttachment.stencilLoadOp		= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp		= VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colorAttachment.initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout			= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentReference{};
	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint			= VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount		= 1;
	subpass.pColorAttachments			= &colorAttachmentReference;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount		= 1;
	renderPassInfo.pAttachments			= &colorAttachment;
	renderPassInfo.subpassCount			= 1;
	renderPassInfo.pSubpasses			= &subpass;

	if ( VK_SUCCESS != vkCreateRenderPass( _device, &renderPassInfo, nullptr, &_renderPass ) )
	{
		return false;
	}

	return true;
}

bool VKApplication::createGraphicsPipeline( void ) noexcept
{
	auto vertShaderCode				= File::readFile( "./vert.spv" );
	auto fragShaderCode				= File::readFile( "./frag.spv" );

	VkShaderModule vertShaderModule = createShaderModule( vertShaderCode );
	VkShaderModule fragShaderModule = createShaderModule( fragShaderCode );

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType		= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage		= VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module		= vertShaderModule;
	vertShaderStageInfo.pName		= "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType		= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage		= VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module		= fragShaderModule;
	fragShaderStageInfo.pName		= "main";

	VkPipelineShaderStageCreateInfo shaderStages[]	= { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType							= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount	= 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType								= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology							= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable			= VK_FALSE;

	VkViewport viewport{};
	viewport.x										= 0.0f;
	viewport.y										= 0.0f;
	viewport.width									= static_cast<float>( _swapChainExtent.width );
	viewport.height									= static_cast<float>( _swapChainExtent.height );
	viewport.minDepth								= 0.0f;
	viewport.maxDepth								= 1.0f;

	VkRect2D scissor{};
	scissor.offset									= { 0, 0 };
	scissor.extent									= _swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType								= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount						= 1;
	viewportState.pViewports						= &viewport;
	viewportState.scissorCount						= 1;
	viewportState.pScissors							= &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType								= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable						= VK_FALSE;
	rasterizer.rasterizerDiscardEnable				= VK_FALSE;
	rasterizer.polygonMode							= VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth							= 1.0f;
	rasterizer.cullMode								= VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace							= VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable						= VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType								= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable				= VK_FALSE;
	multisampling.rasterizationSamples				= VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask				= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable				= VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType								= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable						= VK_FALSE;
	colorBlending.logicOp							= VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount					= 1;
	colorBlending.pAttachments						= &colorBlendAttachment;
	colorBlending.blendConstants[0]					= 0.0f;
	colorBlending.blendConstants[1]					= 0.0f;
	colorBlending.blendConstants[2]					= 0.0f;
	colorBlending.blendConstants[3]					= 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType						= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount				= 0;
	pipelineLayoutInfo.pushConstantRangeCount		= 0;

	if ( VK_SUCCESS != vkCreatePipelineLayout( _device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) ) 
	{
		return false;
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};

	pipelineInfo.sType								= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount							= 2;
	pipelineInfo.pStages							= shaderStages;

	pipelineInfo.pVertexInputState					= &vertexInputInfo;
	pipelineInfo.pInputAssemblyState				= &inputAssembly;
	pipelineInfo.pViewportState						= &viewportState;
	pipelineInfo.pRasterizationState				= &rasterizer;
	pipelineInfo.pMultisampleState					= &multisampling;
	pipelineInfo.pDepthStencilState					= nullptr; // Optional
	pipelineInfo.pColorBlendState					= &colorBlending;
	pipelineInfo.pDynamicState						= nullptr; // Optional

	pipelineInfo.layout								= _pipelineLayout;

	pipelineInfo.renderPass							= _renderPass;
	pipelineInfo.subpass							= 0;

	pipelineInfo.basePipelineHandle					= VK_NULL_HANDLE;

	if ( VK_SUCCESS != vkCreateGraphicsPipelines( _device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline ) ) 
	{
		return false;
	}

	vkDestroyShaderModule( _device, fragShaderModule, nullptr );
	vkDestroyShaderModule( _device, vertShaderModule, nullptr );

	return true;
}

VkShaderModule VKApplication::createShaderModule( const std::vector<char>& code ) const noexcept
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType			= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize			= code.size();
	createInfo.pCode			= reinterpret_cast<const uint32_t*>( code.data() );

	VkShaderModule shaderModule;
	if ( VK_SUCCESS != vkCreateShaderModule( _device, &createInfo, nullptr, &shaderModule ) )
	{
		return shaderModule;
	}

	return shaderModule;
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
	vkDestroyPipelineLayout( _device, _pipelineLayout, nullptr );
	vkDestroyRenderPass( _device, _renderPass, nullptr );

	for ( auto imageView : _swapChainImageViews)
	{
		vkDestroyImageView( _device, imageView, nullptr );
	}

	vkDestroySwapchainKHR( _device, _swapchain, nullptr );
	vkDeviceWaitIdle( _device );

	vkDestroyDevice( _device, nullptr );
	vkDestroySurfaceKHR( _vkInstance, _surface, nullptr );
	vkDestroyInstance( _vkInstance, nullptr );

	vkDestroyPipeline( _device, _graphicsPipeline, nullptr );
	vkDestroyPipelineLayout( _device, _pipelineLayout, nullptr );

	glfwDestroyWindow( _window );
	glfwTerminate();
}
