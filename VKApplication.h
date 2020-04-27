
#pragma once

#include "pch.h"

struct QueueFamilyIndices
{
	std::optional<uint32_t> _graphicsFamily;

	bool isComplete( void ) noexcept
	{
		return _graphicsFamily.has_value();
	}
};

class VKApplication
{
public:

	VKApplication( void );
	~VKApplication( void );

	void			run( void ) noexcept;

private:

	void				initializeWindow( void ) noexcept;
	bool				initializeVKApplication( void ) noexcept;
	bool				createVKInstance( void ) noexcept;
	bool				pickPhysicalDevice( void ) noexcept;
	bool				isDeviceSuitable( const VkPhysicalDevice device ) const noexcept;
	QueueFamilyIndices	findQueueFamilies( const VkPhysicalDevice device ) const noexcept;;

	bool				createLogicalDevice( void ) noexcept;

	void				runLoop( void ) const noexcept;
	void				clean( void ) noexcept;

	GLFWwindow*			_window;
	VkInstance			_vkInstance;
	VkPhysicalDevice	_physicalDevice;
	VkDevice			_device;
	VkQueue				_graphicsQueue;


};
