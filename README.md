# License Expiration Checker for macOS

A cross-platform C++ application for monitoring software license expiration dates. This version is optimized for macOS but also works on Linux and Windows.

## Features

- **Multi-format Support**: Parses XML, JSON, and text-based license files
- **Flexible Scanning**: Monitor individual files or entire directories
- **Expiration Alerts**: Find licenses expiring within a specified timeframe
- **Cross-platform**: Works on macOS, Linux, and Windows
- **Command-line Interface**: Easy to use and scriptable

## Supported License File Formats

- **XML**: `<product>`, `<key>`, `<expiration>` tags
- **JSON**: `"product"`, `"license"/"key"`, `"expires"/"expiration"` fields  
- **Text**: Key-value pairs with common keywords
- **File Extensions**: `.lic`, `.license`, `.xml`, `.json`, `.txt`, `.key`

## Building

### Prerequisites

- **macOS**: Xcode Command Line Tools or Xcode
- **Linux**: GCC 7+ or Clang 6+
- **Windows**: Visual Studio 2017+ or MinGW-w64
- **CMake**: Version 3.16 or higher

### Build Instructions

```bash
# Clone or create the project directory
mkdir license-checker-mac
cd license-checker-mac

# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build the project
make

# Install (optional)
sudo make install
```

### macOS-specific Build

```bash
# Using Xcode generator
cmake -G Xcode ..
open LicenseExpirationChecker.xcodeproj

# Or build directly
cmake --build . --config Release
```

## Usage

### Basic Usage

```bash
# Check licenses in /Applications (30-day default)
./license_checker /Applications

# Check specific directory with custom timeframe
./license_checker --days 60 ~/Licenses

# Check only expired licenses
./license_checker --expired /usr/local/licenses

# Show all licenses found
./license_checker --all ~/Documents/Licenses
```

### Command Line Options

- `-d, --days <num>`: Check licenses expiring within `<num>` days (default: 30)
- `-e, --expired`: Show only expired licenses
- `-a, --all`: Show all licenses found
- `-h, --help`: Display help message

### Examples

```bash
# Monitor system-wide licenses
./license_checker /Applications /usr/local /opt

# Daily check for soon-to-expire licenses
./license_checker --days 7 ~/Licenses

# Weekly expired license audit
./license_checker --expired /Applications /usr/local

# Generate full license inventory
./license_checker --all / > license_inventory.txt
```

## Sample License File Formats

### XML Format
```xml
<?xml version="1.0"?>
<license>
    <product>Adobe Creative Suite</product>
    <key>XXXX-XXXX-XXXX-XXXX</key>
    <expiration>2024-12-31</expiration>
</license>
```

### JSON Format
```json
{
    "product": "Microsoft Office",
    "license": "XXXXX-XXXXX-XXXXX-XXXXX-XXXXX",
    "expires": "2024-12-31T23:59:59Z"
}
```

### Text Format
```
Product: JetBrains IntelliJ IDEA
License Key: XXXXXXXXXXXXXXXXXX
Expires: December 31, 2024
```

## Integration

### Cron Job (Automated Monitoring)
```bash
# Add to crontab for daily checks
0 9 * * * /usr/local/bin/license_checker --days 30 /Applications
```

### Script Integration
```bash
#!/bin/bash
# Check for expiring licenses and send alert
EXPIRING=$(./license_checker --days 7 /Applications)
if [ ! -z "$EXPIRING" ]; then
    echo "$EXPIRING" | mail -s "Licenses Expiring Soon" admin@company.com
fi
```

## Troubleshooting

### Common Issues

1. **Permission Denied**: Use `sudo` or check file permissions
2. **No Licenses Found**: Verify file extensions and formats are supported
3. **Date Parsing Errors**: Ensure dates are in supported formats

### Debugging

```bash
# Enable verbose output (if implemented)
./license_checker --verbose /path/to/licenses

# Check specific file
./license_checker /path/to/specific/license.xml
```

## Development

### Project Structure
```
license-checker-mac/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── license_checker.h
│   ├── file_utils.h
│   └── date_utils.h
└── src/
    ├── main.cpp
    ├── license_checker.cpp
    ├── file_utils.cpp
    └── date_utils.cpp
```

### Adding New License Formats

1. Extend the parser in `license_checker.cpp`
2. Add new regex patterns or parsing logic
3. Update the file extension detection
4. Add test cases

## License

This project is provided as-is for educational and practical use.
