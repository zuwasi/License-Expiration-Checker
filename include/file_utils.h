#pragma once

#include <string>
#include <vector>

namespace FileUtils {
    // Check if file exists
    bool file_exists(const std::string& path);
    
    // Check if path is directory
    bool is_directory(const std::string& path);
    
    // Read entire file content
    std::string read_file(const std::string& path);
    
    // Get all files in directory with optional extension filter
    std::vector<std::string> get_files_in_directory(const std::string& directory, 
                                                   const std::string& extension = "");
    
    // Get file extension
    std::string get_file_extension(const std::string& path);
    
    // Get file modification time
    std::chrono::system_clock::time_point get_file_modification_time(const std::string& path);
}
