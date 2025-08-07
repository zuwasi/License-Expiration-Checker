#pragma once

#include <chrono>
#include <string>
#include <optional>

namespace DateUtils {
    // Parse date from various string formats
    std::optional<std::chrono::system_clock::time_point> parse_date(const std::string& date_str);
    
    // Format date to string
    std::string format_date(const std::chrono::system_clock::time_point& time_point);
    
    // Get current time
    std::chrono::system_clock::time_point now();
    
    // Calculate days between two time points
    int days_between(const std::chrono::system_clock::time_point& from,
                    const std::chrono::system_clock::time_point& to);
    
    // Add days to time point
    std::chrono::system_clock::time_point add_days(const std::chrono::system_clock::time_point& time_point,
                                                   int days);
}
