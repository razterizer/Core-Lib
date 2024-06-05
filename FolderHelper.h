//
//  FolderHelper.h
//  Core
//
//  Created by Rasmus Anthin on 2024-05-29.
//

#pragma once
#include "StringHelper.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <filesystem>


namespace folder
{

#ifdef _WIN32
  // Function to get the directory of the executable
  std::string get_exe_dir()
  {
    char buffer[MAX_PATH];
    // Get the path of the executable
    DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    if (length == 0)
    {
      std::cerr << "ERROR in folder::get_exe_dir() : Unable to fetch exe dir." << std::endl;
      return "";
    }

    std::string path(buffer, length);
    // Find the last backslash in the path
    size_t pos = path.find_last_of("\\/");
    if (pos != std::string::npos)
    {
      // Return the directory part
      return path.substr(0, pos);
    }
    // Return the entire path if no backslash is found (unlikely case)
    return path;
  }
#endif

  std::string get_pwd()
  {
    return std::filesystem::current_path();
  }
  
  char get_path_separator()
  {
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
  }
  
  std::vector<std::string> split_path(const std::string& path)
  {
    return str::tokenize(path, { '/', '\\' });
  }
  
  std::string join_path(const std::vector<std::string>& path_parts)
  {
    size_t num_parts = path_parts.size();
    if (num_parts == 0)
      return "";
    std::string ret = path_parts[0];
    for (size_t i = 1; i < num_parts; ++i)
      ret += get_path_separator() + path_parts[i];
    return ret;
  }
  
  std::string format_path(const std::string& path)
  {
    return join_path(split_path(path));
  }
  
  std::pair<std::string, std::string> split_file_path(const std::string& file_path)
  {
    auto idx = file_path.find_last_of("/\\");
    if (idx != std::string::npos)
      return { file_path.substr(0, idx), file_path.substr(idx + 1) };
    // Here we assume that we passed a filename without path as argument.
    return { "", file_path };
  }
  
  std::string join_file_path(const std::pair<std::string, std::string>& path_and_filename)
  {
    return path_and_filename.first + get_path_separator() + path_and_filename.second;
  }
  
  std::pair<std::string, std::string> split_filename_ext(const std::string& filename)
  {
    auto idx = filename.find_last_of(".");
    if (idx != std::string::npos)
      return { filename.substr(0, idx), filename.substr(idx + 1) }; // filename & ext.
    // Here we assume that we passed a filename (path) without extension as argument.
    return { filename, "" };
  }
  
  std::string join_filename_ext(const std::pair<std::string, std::string>& basefilename_and_ext)
  {
    return basefilename_and_ext.first + '.' + basefilename_and_ext.second;
  }

}
