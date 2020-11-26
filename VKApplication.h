
#pragma once

#include "pch.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> deviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


struct QueueFamilyIndices
{
	std::optional<uint32_t> _graphicsFamily;
	std::optional<uint32_t> _presentFamily;

	bool isComplete( void ) noexcept
	{
		return _graphicsFamily.has_value() && _presentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR			_capabilities;
	std::vector<VkSurfaceFormatKHR>		_formats;
	std::vector<VkPresentModeKHR>		_presentModes;
};

class VKApplication
{
public:

	VKApplication( void );
	~VKApplication( void );

	void			run( void ) noexcept;

private:

	void					initializeWindow( void ) noexcept;
	bool					initializeVKApplication( void ) noexcept;
	bool					createVKInstance( void ) noexcept;
	bool					pickPhysicalDevice( void ) noexcept;
	bool					isDeviceSuitable( const VkPhysicalDevice device ) const noexcept;
	bool					checkDeviceExtensionSupport( const VkPhysicalDevice device ) const noexcept;

	QueueFamilyIndices			findQueueFamilies( const VkPhysicalDevice device ) const noexcept;
	std::vector<const char*>	getRequiredExtensions( void ) const noexcept;
	SwapChainSupportDetails		querySwapChainSupport( const VkPhysicalDevice device) const noexcept;
	VkSurfaceFormatKHR			chooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& availableFormats ) const noexcept;
	VkPresentModeKHR			chooseSwapPresentMode( const std::vector<VkPresentModeKHR>& availablePresentModes ) const noexcept;
	VkExtent2D					chooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities ) const noexcept;
	uint32_t					findMemoryType( uint32_t typeFilter, VkMemoryPropertyFlags properties ) const noexcept;

	bool						createLogicalDevice( void ) noexcept;
	bool						createSurface( void ) noexcept;
	bool						createSwapChain( void ) noexcept;
	bool						recreateSwapChain( void ) noexcept;
	bool						createImageViews( void ) noexcept;
	bool						createRenderPass( void ) noexcept;
	bool						createGraphicsPipeline( void ) noexcept;
	bool						createFramebuffers( void ) noexcept;
	bool						createCommandPool( void ) noexcept;
	bool						createCommandBuffers( void ) noexcept;
	bool						createSyncObjects( void ) noexcept;

	bool						createVertexBuffer( void ) noexcept;
	bool						createIndexBuffer( void ) noexcept;

	VkShaderModule				createShaderModule( const std::vector<char>& code ) const noexcept;
	
	bool						createBuffer( const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory ) noexcept;

	void						copyBuffer( VkBuffer srcBuffer, VkBuffer dstBuffer, const VkDeviceSize size ) noexcept;

	void						runLoop( void ) noexcept;
	void						drawFrame( void ) noexcept;
	
	void						clean( void ) noexcept;
	void						cleanupSwapChain( void ) noexcept;

	static void					framebufferResizeCallback( GLFWwindow* window, int width, int height ) noexcept;

	GLFWwindow*						_window;
	VkInstance						_vkInstance;
	VkPhysicalDevice				_physicalDevice;
	VkDevice						_device;

	VkQueue							_graphicsQueue;
	VkQueue							_presentQueue;

	VkSurfaceKHR					_surface;

	VkSwapchainKHR					_swapChain;
	std::vector<VkImage>			_swapChainImages;
	VkFormat						_swapChainImageFormat;
	VkExtent2D						_swapChainExtent;
	std::vector<VkImageView>		_swapChainImageViews;

	VkRenderPass					_renderPass;
	VkPipelineLayout				_pipelineLayout;
	VkPipeline						_graphicsPipeline;

	std::vector<VkFramebuffer>		_swapChainFramebuffers;

	VkCommandPool					_commandPool;
	std::vector<VkCommandBuffer>	_commandBuffers;

	std::vector<VkSemaphore>		_imageAvailableSemaphores;
	std::vector<VkSemaphore>		_renderFinishedSemaphores;
	std::vector<VkFence>			_inFlightFences;
	std::vector<VkFence>			_imagesInFlight;

	size_t							_currentFrame;
	bool							_framebufferResized;

	VkBuffer						_vertexBuffer;
	VkDeviceMemory					_vertexBufferMemory;

	VkBuffer						_indexBuffer;
	VkDeviceMemory					_indexBufferMemory;
};
