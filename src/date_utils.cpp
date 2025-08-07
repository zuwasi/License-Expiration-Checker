#include "date_utils.h"
#include <sstream>
#include <iomanip>
#include <regex>
#include <map>

namespace DateUtils {

std::optional<std::chrono::system_clock::time_point> parse_date(const std::string& date_str) {
    std::tm tm = {};
    
    // Common date formats to try
    std::vector<std::string> formats = {
        "%Y-%m-%d",           // 2024-12-31
        "%m/%d/%Y",           // 12/31/2024
        "%d/%m/%Y",           // 31/12/2024
        "%Y/%m/%d",           // 2024/12/31
        "%d-%m-%Y",           // 31-12-2024
        "%B %d, %Y",          // December 31, 2024
        "%b %d, %Y",          // Dec 31, 2024
        "%d %B %Y",           // 31 December 2024
        "%d %b %Y",           // 31 Dec 2024
        "%Y-%m-%d %H:%M:%S",  // 2024-12-31 23:59:59
        "%m/%d/%Y %H:%M:%S"   // 12/31/2024 23:59:59
    };
    
    for (const auto& format : formats) {
        std::istringstream ss(date_str);
        ss >> std::get_time(&tm, format.c_str());
        
        if (!ss.fail()) {
            auto time_t = std::mktime(&tm);
            if (time_t != -1) {
                return std::chrono::system_clock::from_time_t(time_t);
            }
        }
    }
    
    // Try parsing ISO 8601 format manually
    std::regex iso_regex(R"((\d{4})-(\d{2})-(\d{2})(?:T(\d{2}):(\d{2}):(\d{2}))?(?:Z|[+-]\d{2}:\d{2})?)", 
                        std::regex::icase);
    std::smatch match;
    
    if (std::regex_search(date_str, match, iso_regex)) {
        tm.tm_year = std::stoi(match[1].str()) - 1900;
        tm.tm_mon = std::stoi(match[2].str()) - 1;
        tm.tm_mday = std::stoi(match[3].str());
        
        if (match[4].matched) {
            tm.tm_hour = std::stoi(match[4].str());
            tm.tm_min = std::stoi(match[5].str());
            tm.tm_sec = std::stoi(match[6].str());
        }
        
        auto time_t = std::mktime(&tm);
        if (time_t != -1) {
            return std::chrono::system_clock::from_time_t(time_t);
        }
    }
    
    // Try parsing numeric timestamp (seconds since epoch)
    try {
        long long timestamp = std::stoll(date_str);
        if (timestamp > 0 && timestamp < 4000000000LL) { // Reasonable range
            return std::chrono::system_clock::from_time_t(timestamp);
        }
    } catch (const std::exception&) {
        // Not a number, continue
    }
    
    return std::nullopt;
}

std::string format_date(const std::chrono::system_clock::time_point& time_point) {
    auto time_t = std::chrono::system_clock::to_time_t(time_point);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::chrono::system_clock::time_point now() {
    return std::chrono::system_clock::now();
}

int days_between(const std::chrono::system_clock::time_point& from,
                const std::chrono::system_clock::time_point& to) {
    auto duration = to - from;
    return std::chrono::duration_cast<std::chrono::hours>(duration).count() / 24;
}

std::chrono::system_clock::time_point add_days(const std::chrono::system_clock::time_point& time_point,
                                              int days) {
    return time_point + std::chrono::hours(24 * days);
}

} // namespace DateUtils
