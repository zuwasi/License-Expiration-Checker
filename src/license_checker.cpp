#include "license_checker.h"
#include "file_utils.h"
#include "date_utils.h"
#include <iostream>
#include <algorithm>
#include <regex>

bool LicenseInfo::is_expired() const {
    return expiration_date < DateUtils::now();
}

int LicenseInfo::days_until_expiration() const {
    return DateUtils::days_between(DateUtils::now(), expiration_date);
}

LicenseChecker::LicenseChecker() = default;
LicenseChecker::~LicenseChecker() = default;

void LicenseChecker::add_license_path(const std::string& path) {
    if (FileUtils::file_exists(path)) {
        license_paths_.push_back(path);
    } else {
        std::cerr << "Warning: Path does not exist: " << path << std::endl;
    }
}

std::vector<LicenseInfo> LicenseChecker::scan_licenses() {
    std::vector<LicenseInfo> licenses;
    
    for (const auto& path : license_paths_) {
        if (FileUtils::is_directory(path)) {
            // Scan directory for license files
            auto files = FileUtils::get_files_in_directory(path);
            for (const auto& file : files) {
                auto ext = FileUtils::get_file_extension(file);
                // Look for common license file extensions
                if (ext == ".lic" || ext == ".license" || ext == ".xml" || 
                    ext == ".json" || ext == ".txt" || ext == ".key") {
                    auto license = parse_license_file(file);
                    if (license) {
                        licenses.push_back(*license);
                    }
                }
            }
        } else {
            // Single file
            auto license = parse_license_file(path);
            if (license) {
                licenses.push_back(*license);
            }
        }
    }
    
    return licenses;
}

std::vector<LicenseInfo> LicenseChecker::get_expiring_licenses(int days_ahead) {
    auto all_licenses = scan_licenses();
    std::vector<LicenseInfo> expiring;
    
    auto cutoff_date = DateUtils::add_days(DateUtils::now(), days_ahead);
    
    std::copy_if(all_licenses.begin(), all_licenses.end(), 
                 std::back_inserter(expiring),
                 [cutoff_date](const LicenseInfo& license) {
                     return license.expiration_date <= cutoff_date && 
                            !license.is_expired();
                 });
    
    return expiring;
}

std::vector<LicenseInfo> LicenseChecker::get_expired_licenses() {
    auto all_licenses = scan_licenses();
    std::vector<LicenseInfo> expired;
    
    std::copy_if(all_licenses.begin(), all_licenses.end(),
                 std::back_inserter(expired),
                 [](const LicenseInfo& license) {
                     return license.is_expired();
                 });
    
    return expired;
}

void LicenseChecker::print_report(const std::vector<LicenseInfo>& licenses) {
    if (licenses.empty()) {
        std::cout << "No licenses to display." << std::endl;
        return;
    }
    
    std::cout << std::endl;
    for (const auto& license : licenses) {
        std::cout << "Product: " << license.product_name << std::endl;
        std::cout << "License Key: " << license.license_key << std::endl;
        std::cout << "Expiration: " << DateUtils::format_date(license.expiration_date);
        
        if (license.is_expired()) {
            std::cout << " (EXPIRED " << std::abs(license.days_until_expiration()) << " days ago)";
        } else {
            int days = license.days_until_expiration();
            if (days <= 30) {
                std::cout << " (expires in " << days << " days)";
            }
        }
        
        std::cout << std::endl;
        std::cout << "File: " << license.file_path << std::endl;
        std::cout << std::string(50, '-') << std::endl;
    }
}

std::optional<LicenseInfo> LicenseChecker::parse_license_file(const std::string& file_path) {
    try {
        std::string content = FileUtils::read_file(file_path);
        if (content.empty()) {
            return std::nullopt;
        }
        
        auto ext = FileUtils::get_file_extension(file_path);
        
        if (ext == ".xml") {
            return parse_xml_license(content, file_path);
        } else if (ext == ".json") {
            return parse_json_license(content, file_path);
        } else {
            return parse_text_license(content, file_path);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error parsing license file " << file_path << ": " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<LicenseInfo> LicenseChecker::parse_xml_license(const std::string& content, const std::string& file_path) {
    LicenseInfo license;
    license.file_path = file_path;
    
    // Simple regex-based XML parsing (for demo purposes)
    std::regex product_regex(R"(<product[^>]*>([^<]+)</product>)", std::regex::icase);
    std::regex key_regex(R"(<key[^>]*>([^<]+)</key>)", std::regex::icase);
    std::regex expiry_regex(R"(<expir[^>]*>([^<]+)</expir)", std::regex::icase);
    
    std::smatch match;
    
    if (std::regex_search(content, match, product_regex)) {
        license.product_name = match[1].str();
    } else {
        license.product_name = "Unknown Product";
    }
    
    if (std::regex_search(content, match, key_regex)) {
        license.license_key = match[1].str();
    } else {
        license.license_key = "Unknown Key";
    }
    
    if (std::regex_search(content, match, expiry_regex)) {
        auto date = DateUtils::parse_date(match[1].str());
        if (date) {
            license.expiration_date = *date;
            return license;
        }
    }
    
    return std::nullopt;
}

std::optional<LicenseInfo> LicenseChecker::parse_json_license(const std::string& content, const std::string& file_path) {
    LicenseInfo license;
    license.file_path = file_path;
    
    // Simple regex-based JSON parsing (for demo purposes)
    std::regex product_regex(R"("product"\s*:\s*"([^"]+)")", std::regex::icase);
    std::regex key_regex(R"("(?:key|license)"\s*:\s*"([^"]+)")", std::regex::icase);
    std::regex expiry_regex(R"("(?:expir|expires)"\s*:\s*"([^"]+)")", std::regex::icase);
    
    std::smatch match;
    
    if (std::regex_search(content, match, product_regex)) {
        license.product_name = match[1].str();
    } else {
        license.product_name = "Unknown Product";
    }
    
    if (std::regex_search(content, match, key_regex)) {
        license.license_key = match[1].str();
    } else {
        license.license_key = "Unknown Key";
    }
    
    if (std::regex_search(content, match, expiry_regex)) {
        auto date = DateUtils::parse_date(match[1].str());
        if (date) {
            license.expiration_date = *date;
            return license;
        }
    }
    
    return std::nullopt;
}

std::optional<LicenseInfo> LicenseChecker::parse_text_license(const std::string& content, const std::string& file_path) {
    LicenseInfo license;
    license.file_path = file_path;
    
    // Look for common patterns in text files
    std::regex product_regex(R"((?:product|software|application)\s*[:\-=]\s*([^\r\n]+))", std::regex::icase);
    std::regex key_regex(R"((?:license|key|serial)\s*[:\-=]\s*([^\r\n]+))", std::regex::icase);
    std::regex expiry_regex(R"((?:expir|expires|valid until|end date)\s*[:\-=]\s*([^\r\n]+))", std::regex::icase);
    
    std::smatch match;
    
    if (std::regex_search(content, match, product_regex)) {
        license.product_name = match[1].str();
    } else {
        license.product_name = FileUtils::get_file_extension(file_path).empty() ? 
                              file_path : file_path.substr(0, file_path.find_last_of('.'));
    }
    
    if (std::regex_search(content, match, key_regex)) {
        license.license_key = match[1].str();
    } else {
        license.license_key = "Not found in file";
    }
    
    if (std::regex_search(content, match, expiry_regex)) {
        auto date = DateUtils::parse_date(match[1].str());
        if (date) {
            license.expiration_date = *date;
            return license;
        }
    }
    
    // If no expiration found, use file modification time + 1 year as estimate
    license.expiration_date = DateUtils::add_days(
        FileUtils::get_file_modification_time(file_path), 365);
    
    return license;
}
