//
//  TextIO.h
//  Core
//
//  Created by Rasmus Anthin on 2024-05-24.
//

#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <vector>


namespace TextIO
{

  bool read_file(const std::string& file_path, std::vector<std::string>& lines)
  {
    if (!std::filesystem::exists(file_path) || !std::filesystem::is_regular_file(file_path))
    {
      std::cerr << "Error: File does not exist" << std::endl;
      return false;
    }
    
    std::ifstream file(file_path);
    
    if (!file.is_open())
    {
      std::cerr << "Error: Unable to open file \"" << file_path << "\"!" << std::endl;
      file.close();
      return false;
    }
    
    if (file.eof())
    {
      std::cout << "Error: End of file reached." << std::endl;
      file.close();
      return false;
    }
    if (file.fail())
    {
      std::cerr << "Error: Non-fatal I/O error occurred." << std::endl;
      file.close();
      return false;
    }
    if (file.bad())
    {
      std::cerr << "Error: Fatal I/O error occurred." << std::endl;
      file.close();
      return false;
    }
    
    std::string l;
    
    while (std::getline(file, l))
      lines.emplace_back(l);

    file.close();
    
    return true;
  }
  
  bool write_file(const std::string& file_path, const std::vector<std::string>& lines)
  {
    std::ofstream file(file_path);
    
    if (!file.is_open())
    {
      std::cerr << "Error: Unable to open file \"" << file_path << "\"!" << std::endl;
      file.close();
      return false;
    }
    
    if (file.eof())
    {
      std::cout << "Error: End of file reached." << std::endl;
      file.close();
      return false;
    }
    if (file.fail())
    {
      std::cerr << "Error: Non-fatal I/O error occurred." << std::endl;
      file.close();
      return false;
    }
    if (file.bad())
    {
      std::cerr << "Error: Fatal I/O error occurred." << std::endl;
      file.close();
      return false;
    }
    
    for (const auto& l : lines)
      file << l << '\n';
    
    file.close();
    
    return true;
  }

}
