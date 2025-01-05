#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <sstream>

using namespace std;

// Function to parse a date in "DD-MMM-YYYY" format to a tm struct.
tm parseDate(const string& dateStr) {
    tm date = {};
    istringstream ss(dateStr);
    ss >> get_time(&date, "%d-%b-%Y");
    if (ss.fail()) {
        cerr << "Error: Failed to parse date string." << endl;
    }
    return date;
}

// Function to format a tm struct as a string.
string formatDate(const tm& date) {
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%d-%b-%Y", &date);
    return string(buffer);
}

// Function to calculate the difference in days between two dates.
int daysUntilExpiration(const tm& futureDate) {
    auto now = chrono::system_clock::now();
    time_t nowTime = chrono::system_clock::to_time_t(now);
    tm today = {};
    if (localtime_s(&today, &nowTime) != 0) {
        cerr << "Error: Failed to get local time." << endl;
        return -1;
    }

    time_t future = mktime(const_cast<tm*>(&futureDate));
    double secondsDiff = difftime(future, mktime(&today));
    return static_cast<int>(secondsDiff / (60 * 60 * 24));
}

// Function to scan the license file and find the earliest expiration date.
void scanLicenseFile(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file at " << filePath << endl;
        return;
    }

    string line;
    regex expirationRegex("\\d{2}-[A-Za-z]{3}-\\d{4}");
    regex featureRegex("LICENSE\\s+([a-zA-Z0-9_-]+)");
    vector<tuple<string, string, int>> warnings; // Tuple: feature name, expiration date, days remaining
    tm earliestExpiration = {};
    bool firstDate = true;

    while (getline(file, line)) {
        smatch expirationMatch;
        smatch featureMatch;
        if (regex_search(line, expirationMatch, expirationRegex)) {
            string dateStr = expirationMatch.str(0);
            tm expirationDate = parseDate(dateStr);

            int daysRemaining = daysUntilExpiration(expirationDate);

            string featureName = "Unknown Feature";
            if (regex_search(line, featureMatch, featureRegex)) {
                featureName = featureMatch.str(1);
            }

            warnings.push_back({ featureName, dateStr, daysRemaining });

            if (firstDate || difftime(mktime(&expirationDate), mktime(&earliestExpiration)) < 0) {
                earliestExpiration = expirationDate;
                firstDate = false;
            }
        }
    }

    file.close();

    cout << "\nEarliest Expiration Date: " << formatDate(earliestExpiration) << endl;
    for (const auto& warning : warnings) {
        cout << "Feature: " << get<0>(warning) << " - License will expire on " << get<1>(warning) << " in " << get<2>(warning) << " days." << endl;
    }
}

// Function to store the file path permanently in a configuration file.
void storeFilePath(const string& filePath) {
    ofstream configFile("config.txt");
    if (configFile.is_open()) {
        configFile << filePath;
        configFile.close();
    }
    else {
        cerr << "Error: Unable to write to config file." << endl;
    }
}

// Function to load the file path from the configuration file.
string loadFilePath() {
    ifstream configFile("config.txt");
    string filePath;
    if (configFile.is_open()) {
        getline(configFile, filePath);
        configFile.close();
    }
    return filePath;
}

// Function to delete the stored file path.
void deleteStoredFilePath() {
    if (filesystem::exists("config.txt")) {
        if (filesystem::remove("config.txt")) {
            cout << "Stored file path has been deleted successfully." << endl;
        }
        else {
            cerr << "Error: Unable to delete the stored file path." << endl;
        }
    }
    else {
        cout << "No stored file path found to delete." << endl;
    }
}

int main() {
    string filePath;
    bool validFilePath = false;
    char userChoice;

    cout << "Do you want to delete the stored file path? (y/n): ";
    cin >> userChoice;
    if (userChoice == 'y' || userChoice == 'Y') {
        deleteStoredFilePath();
    }

    while (!validFilePath) {
        // Load file path from config, if available.
        filePath = loadFilePath();

        if (filePath.empty()) {
            cout << "Enter the license file path: ";
            cin >> filePath;
            storeFilePath(filePath);
        }
        else {
            cout << "Using stored file path: " << filePath << endl;
        }

        if (!filesystem::exists(filePath)) {
            cerr << "Warning: No license file was found at the recorded location." << endl;
            cout << "Please enter a new license file path: ";
            cin >> filePath;
            storeFilePath(filePath);
            continue; // Retry loop after user input
        }
        else {
            validFilePath = true;
        }
    }

    // Scan the license file.
    scanLicenseFile(filePath);

    return 0;
}
