#include "pch.h"

#include "VKApplication.h"
#include "File.h"
#include "Vertex.h"

const int MAX_FRAMES_IN_FLIGHT = 2;


VKApplication::VKApplication( void )
	: _window{ nullptr }
	, _vkInstance{ nullptr }
	, _physicalDevice{ VK_NULL_HANDLE  }
	, _currentFrame{ 0 }
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

	if ( false == createFramebuffers() )
	{
		return false;
	}

	if ( false == createCommandPool() )
	{
		return false;
	}

	if ( false == createVertexBuffer() )
	{
		return false;
	}

	if ( false == createIndexBuffer() )
	{
		return false;
	}

	if ( false == createCommandBuffers() )
	{
		return false;
	}

	if ( false == createSyncObjects() )
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

	int width				= 0;
	int height				= 0;

	glfwGetFramebufferSize( _window, &width, &height );

	VkExtent2D actualExtent = 
	{
		static_cast<uint32_t>( width ),
		static_cast<uint32_t>( height )
	};

	actualExtent.width		= std::max( capabilities.minImageExtent.width, std::min( capabilities.maxImageExtent.width, actualExtent.width ) );
	actualExtent.height		= std::max( capabilities.minImageExtent.height, std::min( capabilities.maxImageExtent.height, actualExtent.height ) );

	return actualExtent;
}

uint32_t VKApplication::findMemoryType( uint32_t typeFilter, VkMemoryPropertyFlags properties ) const noexcept
{
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties( _physicalDevice, &memoryProperties );

	for ( uint32_t ii = 0; ii < memoryProperties.memoryTypeCount; ++ii ) 
	{
		if ( ( typeFilter & ( 1 << ii ) ) && 
			 ( memoryProperties.memoryTypes[ii].propertyFlags & properties ) == properties ) 
		{
			return ii;
		}
	}

	return -1;
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


	if ( VK_SUCCESS != vkCreateSwapchainKHR( _device, &createInfo, nullptr, &_swapChain ) ) 
	{
		return false;
	}

	vkGetSwapchainImagesKHR( _device, _swapChain, &imageCount, nullptr );
	_swapChainImages.resize( imageCount );
	vkGetSwapchainImagesKHR( _device, _swapChain, &imageCount, _swapChainImages.data() );

	_swapChainImageFormat						= surfaceFormat.format;
	_swapChainExtent							= extent;

	return true;
}

bool VKApplication::recreateSwapChain( void ) noexcept
{
	int width		= 0;
	int height		= 0;
	glfwGetFramebufferSize( _window, &width, &height );
	while ( ( 0 == width ) || ( 0 == height ) ) 
	{
		glfwGetFramebufferSize( _window, &width, &height );
		glfwWaitEvents();
	}

	vkDeviceWaitIdle( _device );

	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandBuffers();

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
	
	VkVertexInputBindingDescription bindingDescription						= Vertex::getBindingDescription();
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions	= Vertex::getAttributeDescriptions();
	
	vertexInputInfo.vertexBindingDescriptionCount	= 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions		= &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions	= attributeDescriptions.data();

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

bool VKApplication::createFramebuffers( void ) noexcept
{
	const int swapChainImageViewSize = static_cast<int>( _swapChainImageViews.size() );

	_swapChainFramebuffers.resize( swapChainImageViewSize );

	for ( int ii = 0; ii < swapChainImageViewSize; ++ii )
	{
		VkImageView attachments[] = 
		{
			_swapChainImageViews[ii]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType						= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass					= _renderPass;
		framebufferInfo.attachmentCount				= 1;
		framebufferInfo.pAttachments				= attachments;
		framebufferInfo.width						= _swapChainExtent.width;
		framebufferInfo.height						= _swapChainExtent.height;
		framebufferInfo.layers						= 1;

		if ( VK_SUCCESS !=vkCreateFramebuffer( _device, &framebufferInfo, nullptr, &_swapChainFramebuffers[ii] ) ) 
		{
			return false;
		}
	}

	return true;
}

bool VKApplication::createCommandPool( void ) noexcept
{
	QueueFamilyIndices queueFamilyIndices	= findQueueFamilies( _physicalDevice );

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType							= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex				= queueFamilyIndices._graphicsFamily.value();
	poolInfo.flags							= 0; // Optional

	if ( VK_SUCCESS != vkCreateCommandPool( _device, &poolInfo, nullptr, &_commandPool ) )
	{
		return false;
	}

	return true;
}

bool VKApplication::createCommandBuffers( void ) noexcept
{
	_commandBuffers.resize( _swapChainFramebuffers.size() );

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType								= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool						= _commandPool;
	allocInfo.level								= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount				= static_cast<uint32_t>( _commandBuffers.size() );

	if ( VK_SUCCESS != vkAllocateCommandBuffers( _device, &allocInfo, _commandBuffers.data() ) ) 
	{
		return false;
	}

	const int commandBufferSize					= static_cast<int>( _commandBuffers.size() );

	for ( int ii = 0; ii < commandBufferSize; ++ii )
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType							= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags							= 0; // Optional
		beginInfo.pInheritanceInfo				= nullptr; // Optional

		if ( VK_SUCCESS != vkBeginCommandBuffer( _commandBuffers[ii], &beginInfo ) ) 
		{
			return false;
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType					= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass				= _renderPass;
		renderPassInfo.framebuffer				= _swapChainFramebuffers[ii];
		renderPassInfo.renderArea.offset		= { 0, 0 };
		renderPassInfo.renderArea.extent		= _swapChainExtent;

		VkClearValue clearColor					= { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount			= 1;
		renderPassInfo.pClearValues				= &clearColor;

		vkCmdBeginRenderPass( _commandBuffers[ii], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

		vkCmdBindPipeline( _commandBuffers[ii], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline );

		VkBuffer vertexBuffers[] = { _vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers( _commandBuffers[ii], 0, 1, vertexBuffers, offsets );

		vkCmdBindIndexBuffer( _commandBuffers[ii], _indexBuffer, 0, VK_INDEX_TYPE_UINT16 );

		vkCmdDrawIndexed( _commandBuffers[ii], static_cast<uint32_t>( indices.size() ), 1, 0, 0, 0 );

		vkCmdEndRenderPass( _commandBuffers[ii] );

		if ( VK_SUCCESS != vkEndCommandBuffer( _commandBuffers[ii]) ) 
		{
			return false;
		}
	}

	return true;
}

bool VKApplication::createSyncObjects( void ) noexcept
{
	_imageAvailableSemaphores.resize( MAX_FRAMES_IN_FLIGHT );
	_renderFinishedSemaphores.resize( MAX_FRAMES_IN_FLIGHT );
	_inFlightFences.resize( MAX_FRAMES_IN_FLIGHT );
	_imagesInFlight.resize( _swapChainImages.size(), VK_NULL_HANDLE );


	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType								= VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType									= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	for ( int ii = 0; ii < MAX_FRAMES_IN_FLIGHT; ++ii ) 
	{
		if ( ( VK_SUCCESS != vkCreateSemaphore( _device, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[ii] ) ) ||
			 ( VK_SUCCESS != vkCreateSemaphore( _device, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[ii] ) ) || 
			 ( VK_SUCCESS != vkCreateFence( _device, &fenceInfo, nullptr, &_inFlightFences[ii] ) ) )
		{
			return false;
		}
	}

	return true;
}

bool VKApplication::createVertexBuffer( void ) noexcept
{
	const VkDeviceSize bufferSize = static_cast<VkDeviceSize>( sizeof( vertices[0] ) * vertices.size() );
	
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

	void* data = nullptr;
	
	vkMapMemory( _device, stagingBufferMemory, 0, bufferSize, 0, &data );
	memcpy( data, vertices.data(), static_cast<size_t>( bufferSize ) );
	vkUnmapMemory( _device, stagingBufferMemory );

	createBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory );

	copyBuffer( stagingBuffer, _vertexBuffer, bufferSize );

	vkDestroyBuffer( _device, stagingBuffer, nullptr );
	vkFreeMemory( _device, stagingBufferMemory, nullptr );

	return true;
}

bool VKApplication::createIndexBuffer( void ) noexcept
{
	const VkDeviceSize bufferSize = static_cast<VkDeviceSize>( sizeof( indices[0] ) * indices.size() );
	
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	
	createBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

	void* data = nullptr;
	vkMapMemory( _device, stagingBufferMemory, 0, bufferSize, 0, &data );
	memcpy( data, indices.data(), static_cast<size_t>( bufferSize ) );
	vkUnmapMemory( _device, stagingBufferMemory );

	createBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory );

	copyBuffer( stagingBuffer, _indexBuffer, bufferSize );

	vkDestroyBuffer( _device, stagingBuffer, nullptr );
	vkFreeMemory( _device, stagingBufferMemory, nullptr );

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

bool VKApplication::createBuffer( const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory ) noexcept
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size			= size;
	bufferInfo.usage		= usage;
	bufferInfo.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

	if ( VK_SUCCESS != vkCreateBuffer( _device, &bufferInfo, nullptr, &buffer ) )
	{
		return false;
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements( _device, buffer, &memRequirements );

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize	= memRequirements.size;
	allocInfo.memoryTypeIndex	= findMemoryType(memRequirements.memoryTypeBits, properties);

	if ( VK_SUCCESS != vkAllocateMemory( _device, &allocInfo, nullptr, &bufferMemory ) ) 
	{
		return false;
	}

	vkBindBufferMemory( _device, buffer, bufferMemory, 0 );

	return true;
}

void VKApplication::copyBuffer( VkBuffer srcBuffer, VkBuffer dstBuffer, const VkDeviceSize size ) noexcept
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool			= _commandPool;
	allocInfo.commandBufferCount	= 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers( _device, &allocInfo, &commandBuffer );

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags					= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer( commandBuffer, &beginInfo );

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset			= 0; // Optional
	copyRegion.dstOffset			= 0; // Optional
	copyRegion.size					= size;
	vkCmdCopyBuffer( commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion );

	vkEndCommandBuffer( commandBuffer );

	VkSubmitInfo submitInfo{};
	submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount	= 1;
	submitInfo.pCommandBuffers	= &commandBuffer;

	vkQueueSubmit( _graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE );
	vkQueueWaitIdle( _graphicsQueue );

	vkFreeCommandBuffers( _device, _commandPool, 1, &commandBuffer );
}

void VKApplication::initializeWindow( void ) noexcept
{
	const uint32_t WIDTH	= 800;
	const uint32_t HEIGHT	= 600;

	glfwInit();

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	_window = glfwCreateWindow( WIDTH, HEIGHT, "Vulkan", nullptr, nullptr );
	glfwSetWindowUserPointer( _window, this );
	glfwSetFramebufferSizeCallback( _window, framebufferResizeCallback );
}

void VKApplication::runLoop( void ) noexcept
{
	while ( !glfwWindowShouldClose( _window ) )
	{
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle( _device );
}

void VKApplication::drawFrame( void ) noexcept
{
	//vkWaitForFences( _device, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX );

	uint32_t imageIndex = 0;
	VkResult result = vkAcquireNextImageKHR( _device, _swapChain, UINT64_MAX, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex );
	if ( VK_ERROR_OUT_OF_DATE_KHR == result )
	{
		recreateSwapChain();
		return;
	}
	else if ( VK_SUCCESS != result && VK_SUBOPTIMAL_KHR != result )
	{
		return;
	}



	if ( VK_NULL_HANDLE !=  _imagesInFlight[imageIndex] ) 
	{
		vkWaitForFences( _device, 1, &_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX );
	}

	_imagesInFlight[imageIndex]				= _inFlightFences[_currentFrame];

	VkSubmitInfo submitInfo{};
	submitInfo.sType						= VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[]			= { _imageAvailableSemaphores[_currentFrame] };
	VkPipelineStageFlags waitStages[]		= { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount			= 1;
	submitInfo.pWaitSemaphores				= waitSemaphores;
	submitInfo.pWaitDstStageMask			= waitStages;

	submitInfo.commandBufferCount			= 1;
	submitInfo.pCommandBuffers				= &_commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[]			= { _renderFinishedSemaphores[_currentFrame] };
	submitInfo.signalSemaphoreCount			= 1;
	submitInfo.pSignalSemaphores			= signalSemaphores;

	vkResetFences( _device, 1, &_inFlightFences[_currentFrame] );

	if ( VK_SUCCESS != vkQueueSubmit( _graphicsQueue, 1, &submitInfo, _inFlightFences[_currentFrame] ) ) 
	{
		return;
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType						= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount			= 1;
	presentInfo.pWaitSemaphores				= signalSemaphores;

	VkSwapchainKHR swapChains[]				= { _swapChain };
	presentInfo.swapchainCount				= 1;
	presentInfo.pSwapchains					= swapChains;

	presentInfo.pImageIndices				= &imageIndex;

	result = vkQueuePresentKHR( _presentQueue, &presentInfo );
	if ( ( VK_ERROR_OUT_OF_DATE_KHR == result ) || 
		 ( VK_SUBOPTIMAL_KHR == result ) || 
		 ( true == _framebufferResized ) ) 
	{
		_framebufferResized = false;
		recreateSwapChain();
	}
	else if ( VK_SUCCESS != result ) 
	{
		return;
	}

	_currentFrame = ( _currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


void VKApplication::clean( void ) noexcept
{
	cleanupSwapChain();

	vkDestroyCommandPool( _device, _commandPool, nullptr );
	
	for ( int ii = 0; ii < MAX_FRAMES_IN_FLIGHT; ++ii ) 
	{
        vkDestroySemaphore( _device, _renderFinishedSemaphores[ii], nullptr );
        vkDestroySemaphore( _device, _imageAvailableSemaphores[ii], nullptr );
		vkDestroyFence( _device, _inFlightFences[ii], nullptr );
    }

	vkDestroyBuffer( _device, _indexBuffer, nullptr );
	vkFreeMemory( _device, _indexBufferMemory, nullptr );

	vkDestroyBuffer( _device, _vertexBuffer, nullptr );
    vkFreeMemory( _device, _vertexBufferMemory, nullptr );

	vkDestroyDevice( _device, nullptr );
	vkDestroySurfaceKHR( _vkInstance, _surface, nullptr );
	vkDestroyInstance( _vkInstance, nullptr );

	glfwDestroyWindow( _window );
	glfwTerminate();
}

void VKApplication::cleanupSwapChain( void ) noexcept
{
	const int swapChainFramebuffersSize = static_cast<int>( _swapChainFramebuffers.size() );
	const int swpaChainImageViewsSize	= static_cast<int>( _swapChainImageViews.size() );

	for ( int ii = 0; ii < swapChainFramebuffersSize; ++ii )
	{
		vkDestroyFramebuffer(_device, _swapChainFramebuffers[ii], nullptr );
	}

	vkFreeCommandBuffers( _device, _commandPool, static_cast<uint32_t>( _commandBuffers.size()), _commandBuffers.data() );
	vkDestroyPipeline( _device, _graphicsPipeline, nullptr );
	vkDestroyPipelineLayout( _device, _pipelineLayout, nullptr );
	vkDestroyRenderPass( _device, _renderPass, nullptr );

	for ( int ii = 0; ii < swpaChainImageViewsSize; ++ii )
	{
		vkDestroyImageView( _device, _swapChainImageViews[ii], nullptr );
	}

	vkDestroySwapchainKHR( _device, _swapChain, nullptr );
}

void VKApplication::framebufferResizeCallback( GLFWwindow * window, int width, int height ) noexcept
{
	VKApplication* app = reinterpret_cast<VKApplication*>( glfwGetWindowUserPointer( window ) );
	
	app->_framebufferResized = true;
}
