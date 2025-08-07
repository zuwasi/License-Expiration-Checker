#include "file_utils.h"
#include <fstream>
#include <filesystem>
#include <sstream>

namespace FileUtils {

bool file_exists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool is_directory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> get_files_in_directory(const std::string& directory, 
                                               const std::string& extension) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string file_path = entry.path().string();
                
                if (extension.empty() || get_file_extension(file_path) == extension) {
                    files.push_back(file_path);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // Directory might not be accessible, skip it
    }
    
    return files;
}

std::string get_file_extension(const std::string& path) {
    std::filesystem::path file_path(path);
    return file_path.extension().string();
}

std::chrono::system_clock::time_point get_file_modification_time(const std::string& path) {
    try {
        auto file_time = std::filesystem::last_write_time(path);
        
        // Convert file_time to system_clock time_point
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            file_time - std::filesystem::file_time_type::clock::now() + 
            std::chrono::system_clock::now());
            
        return sctp;
    } catch (const std::exception&) {
        return std::chrono::system_clock::now();
    }
}

} // namespace FileUtils
