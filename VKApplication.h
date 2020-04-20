
#pragma once

#include "pch.h"

class VKApplication
{
public:

	VKApplication( void );
	~VKApplication( void );

	void			run( void ) noexcept;

private:

	void			initializeWindow( void ) noexcept;
	bool			initializeVKApplication( void ) noexcept;
	bool			createVKInstance( void ) noexcept;

	void			runLoop( void ) noexcept;
	void			clean( void ) noexcept;

	GLFWwindow*		_window;
	VkInstance		_vkInstance;
};
