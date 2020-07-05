#include "pch.h"

#include "File.h"

std::vector<char> File::readFile( const std::string& fileName ) noexcept
{
	std::ifstream file( fileName, std::ios::ate | std::ios::binary );

	if ( false == file.is_open() )
	{
		return std::vector<char>();
	}

	const size_t fileSize = static_cast<size_t>( file.tellg() );
	std::vector<char> buffer( fileSize );

	file.seekg( 0 );
	file.read( buffer.data(), fileSize );

	file.close();

	return buffer;
}
