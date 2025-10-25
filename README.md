# 🎓 Student Picker

A Qt6-based desktop application for random student selection with data management capabilities.

## Features

- 📥 Import student data from CSV files
- 🎲 Random student selection by class
- 📷 Photo upload and compression
- 💾 SQLite database storage
- 🎨 Clean Zen Light Mode UI
- 🔧 Cross-platform (Windows, macOS, Linux)

## Requirements

- Qt6 (Core, Gui, Widgets, Sql)
- CMake 3.16 or higher
- C++17 compatible compiler

## Building from Source

### Linux
```bash
# Install Qt6
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential

# Clone and build
git clone https://github.com/Blues24/Student-Picker-cpp.git
cd StudentPicker
mkdir build && cd build
cmake ..
make
./StudentPicker
```

### Windows
```bash
# Install Qt6 from https://www.qt.io/download
# Install CMake from https://cmake.org/download

# Build
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
StudentPicker.exe
```

### macOS
```bash
# Install Qt6
brew install qt@6 cmake

# Build
mkdir build && cd build
cmake ..
make
./StudentPicker.app/Contents/MacOS/StudentPicker
```

## CSV File Format

Your CSV file should have the following columns:
```csv
Name,StudentID,Class
John Doe,2024001,A1
Jane Smith,2024002,A1
Bob Johnson,2024003,B2
```

Required columns:
- `Name` - Student's full name
- `StudentID` - Unique student identifier
- `Class` - Class name

## Usage

1. **Import Data**: Click "Import CSV/XLSX" and select your data file
2. **Select Class**: Choose a class from the dropdown
3. **Pick Random**: Click "Pick Random Student" to randomly select
4. **Upload Photos**: Select a student and click "Upload Photo"

## Database Location

The application stores data in:
- **Linux**: `~/.local/share/StudentPicker/`
- **Windows**: `C:\Users\<Username>\AppData\Local\StudentPicker\`
- **macOS**: `~/Library/Application Support/StudentPicker/`

## License
AGPL-3.0

## Author

Blues24