#pragma once

#include <string>

namespace File
{
static const std::string PATH_SEPARATOR_STR =
#ifdef _WIN32
  "\\";
#else
  "/";
#endif

static const char PATH_SEPARATOR = PATH_SEPARATOR_STR[0];

std::string GetContainingFolder(const std::string& fpath);

std::string AppendPath(const std::string& path, const std::string& fpath);
}