#include "File.h"


namespace File
{

std::string removeTailingSeperator(std::string path)
{
	while (path.back() == '\\' || path.back() == '/')
	{
		path = path.substr(0, path.size() - 1);
	}
	return path;
}

std::string GetContainingFolder(const std::string& fpath)
{
	std::string temp = removeTailingSeperator(fpath);
	size_t pos = temp.find_last_of("\\/");
	if (std::string::npos != pos)
		return temp.substr(0, pos);
	else
		return "";
}

std::string AppendPath(const std::string& path, const std::string& fpath)
{
	if (fpath[0] != '\\' && fpath[0] != '/')
	{
		if (fpath[0] != '.') // current directory
		{
			std::string finalPath = removeTailingSeperator(path);
			finalPath.append(PATH_SEPARATOR_STR);
			finalPath.append(fpath);
			return finalPath;
		}
		else if (fpath[0] == '.' && (fpath[1] == '\\' || fpath[1] == '/')) // current directory
		{
			std::string finalPath = removeTailingSeperator(path);
			finalPath.append(fpath.substr(1));
			return finalPath;
		}
		else if (fpath[0] == '.' && fpath[1] == '.')
		{
			if (fpath[2] == '\\' || fpath[3] == '/') // back step in directory
			{
				std::string finalPath = GetContainingFolder(removeTailingSeperator(path));
				std::string tempFpath = fpath.substr(3);
				while (tempFpath[0] == '.' && tempFpath[1] == '.' && (tempFpath[2] == '\\' || tempFpath[2] == '/')) // multiple back steps
				{
					finalPath = GetContainingFolder(path);
					tempFpath = fpath.substr(3);
				}
				finalPath.append(PATH_SEPARATOR_STR);
				finalPath.append(tempFpath);
				return finalPath;
			}
		}
	}
	return fpath;
}

}