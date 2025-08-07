#include <iostream>
#include <string>
#include <vector>
#include "license_checker.h"

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options] <path1> [path2] ...\n";
    std::cout << "Options:\n";
    std::cout << "  -d, --days <num>     Check licenses expiring within <num> days (default: 30)\n";
    std::cout << "  -e, --expired        Show only expired licenses\n";
    std::cout << "  -a, --all            Show all licenses\n";
    std::cout << "  -h, --help           Show this help message\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << program_name << " /Applications\n";
    std::cout << "  " << program_name << " --days 60 ~/Licenses\n";
    std::cout << "  " << program_name << " --expired /usr/local/licenses\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    std::vector<std::string> paths;
    int days_ahead = 30;
    bool show_expired_only = false;
    bool show_all = false;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            return 0;
        } else if (arg == "-e" || arg == "--expired") {
            show_expired_only = true;
        } else if (arg == "-a" || arg == "--all") {
            show_all = true;
        } else if (arg == "-d" || arg == "--days") {
            if (i + 1 < argc) {
                try {
                    days_ahead = std::stoi(argv[++i]);
                } catch (const std::exception&) {
                    std::cerr << "Error: Invalid number for days: " << argv[i] << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Error: --days requires a number argument" << std::endl;
                return 1;
            }
        } else if (arg[0] != '-') {
            paths.push_back(arg);
        } else {
            std::cerr << "Error: Unknown option: " << arg << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }

    if (paths.empty()) {
        std::cerr << "Error: No paths specified" << std::endl;
        print_usage(argv[0]);
        return 1;
    }

    try {
        LicenseChecker checker;

        // Add all paths to checker
        for (const auto& path : paths) {
            checker.add_license_path(path);
        }

        std::vector<LicenseInfo> licenses_to_show;

        if (show_expired_only) {
            licenses_to_show = checker.get_expired_licenses();
            std::cout << "=== EXPIRED LICENSES ===" << std::endl;
        } else if (show_all) {
            licenses_to_show = checker.scan_licenses();
            std::cout << "=== ALL LICENSES ===" << std::endl;
        } else {
            licenses_to_show = checker.get_expiring_licenses(days_ahead);
            std::cout << "=== LICENSES EXPIRING WITHIN " << days_ahead << " DAYS ===" << std::endl;
        }

        if (licenses_to_show.empty()) {
            std::cout << "No licenses found matching the criteria." << std::endl;
        } else {
            checker.print_report(licenses_to_show);
        }

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
