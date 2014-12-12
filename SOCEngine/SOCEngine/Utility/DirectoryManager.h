
#pragma once

#include <string>
#include <vector>

namespace Utility
{
	class DirectoryManager
	{
	public:
		enum DirectoryType {Resource = 0, Shader};
		
	private:
		std::vector<std::string> _paths;

	public:
		DirectoryManager(void);
		~DirectoryManager(void);

	public:
		const std::string& GetPath(DirectoryType type) const;
		const std::string FullPathforFileName(const std::string& fileName, DirectoryType type);
		bool isAbsolutePath(const std::string& fileName);
	};
}

