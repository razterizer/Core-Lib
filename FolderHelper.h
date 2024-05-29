//
//  FolderHelper.h
//  Core
//
//  Created by Rasmus Anthin on 2024-05-29.
//

#pragma once
#ifdef _WIN32
#include <windows.h>
#endif


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

}
