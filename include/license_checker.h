#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <optional>

struct LicenseInfo {
    std::string product_name;
    std::string license_key;
    std::chrono::system_clock::time_point expiration_date;
    std::string file_path;
    bool is_expired() const;
    int days_until_expiration() const;
};

class LicenseChecker {
public:
    LicenseChecker();
    ~LicenseChecker();

    // Add license file or directory to monitor
    void add_license_path(const std::string& path);
    
    // Scan all added paths for license files
    std::vector<LicenseInfo> scan_licenses();
    
    // Get licenses expiring within specified days
    std::vector<LicenseInfo> get_expiring_licenses(int days_ahead = 30);
    
    // Get expired licenses
    std::vector<LicenseInfo> get_expired_licenses();
    
    // Print license report
    void print_report(const std::vector<LicenseInfo>& licenses);

private:
    std::vector<std::string> license_paths_;
    
    // Parse different license file formats
    std::optional<LicenseInfo> parse_license_file(const std::string& file_path);
    std::optional<LicenseInfo> parse_xml_license(const std::string& content, const std::string& file_path);
    std::optional<LicenseInfo> parse_json_license(const std::string& content, const std::string& file_path);
    std::optional<LicenseInfo> parse_text_license(const std::string& content, const std::string& file_path);
};
