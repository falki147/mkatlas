#include "Platform.hpp"

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#define UNICODE
	
	#include <Windows.h>
	#include <codecvt>
	#include <algorithm>
	
	#include "Utils.hpp"
	
	static std::size_t findSeperator(const std::string& str) {
		auto pb = str.find_last_of('\\');
		auto ps = str.find_last_of('/');

		return ps == std::string::npos ? pb : std::max(pb, ps);
	}
	
	std::vector<std::string> glob(const std::string& str) {
		// Setup conversion
		std::wstring_convert<std::codecvt_utf8_utf16<unsigned short>, unsigned short> conv;

		// Get base directory
		auto seperator = findSeperator(str);
		auto baseDir = seperator == std::string::npos ? std::string() : str.substr(0, seperator + 1);

		// Convert to utf16
		auto strw = conv.from_bytes(str.data());

		// Find files
		std::vector<std::string> strings;

		WIN32_FIND_DATA data;

		auto h = FindFirstFile((LPCWSTR) strw.c_str(), &data);

		if (h == INVALID_HANDLE_VALUE) {
			if (GetLastError() == ERROR_FILE_NOT_FOUND)
				throw std::runtime_error(combine("file doesn't exist (\"", str, "\")"));
			else
				throw std::runtime_error(combine("invalid filename (\"", str, "\")"));
		}

		do {
			if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				strings.push_back(baseDir + conv.to_bytes((unsigned short*) data.cFileName));
		} while (FindNextFile(h, &data));

		FindClose(h);

		return strings;
	}

	std::string stripBase(const std::string& str) {
		auto seperator = findSeperator(str);
		return seperator == std::string::npos ? str : str.substr(seperator + 1);
	}

	std::string stripExtension(const std::string& str)  {
		auto dot = str.find_last_of('.');

		if (dot == std::string::npos)
			return str;

		auto seperator = findSeperator(str);

		if (seperator != std::string::npos && seperator > dot)
			return str;

		return str.substr(0, dot);
	}
#else
	#error Operating System is not supported!
#endif
