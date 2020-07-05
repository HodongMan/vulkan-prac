#pragma once

class File
{
public:
	static std::vector<char> readFile( const std::string& fileName ) noexcept;
};