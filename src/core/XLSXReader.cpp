#include "XLSXReader.hpp"
#include "Logger.hpp"

namespace StudentPicker {

XLSXReader::XLSXReader() 
    : m_sheetIndex(0) {
}

XLSXReader::~XLSXReader() {
}

bool XLSXReader::readFile(const QString& filePath) {
    m_data.clear();
    m_headers.clear();
    m_lastError.clear();
    
    // IMPLEMENTASI SEMENTARA
    // Karena Qt tidak memiliki built-in XLSX reader
    
    // OPSI: Untuk implementasi lengkap, gunakan library QtXlsx
    // Tambahkan di CMakeLists.txt:
    // find_package(QXlsx REQUIRED)
    // target_link_libraries(StudentPicker QXlsx::QXlsx)
    
    // Untuk saat ini, return error dan minta user convert ke CSV
    m_lastError = "XLSX format is not yet fully supported.\n"
                  "Please convert your file to CSV format.\n\n"
                  "How to convert:\n"
                  "1. Open your XLSX file in Excel/LibreOffice\n"
                  "2. File > Save As\n"
                  "3. Choose 'CSV (Comma delimited)' format\n"
                  "4. Save and import the CSV file instead";
    
    Logger::warn("XLSX file attempted but not supported:", filePath);
    Logger::info("Please convert to CSV format");
    
    return false;
}

QVector<QVariantMap> XLSXReader::getData() const {
    return m_data;
}

QStringList XLSXReader::getHeaders() const {
    return m_headers;
}

QString XLSXReader::getLastError() const {
    return m_lastError;
}

void XLSXReader::setSheetName(const QString& sheetName) {
    m_sheetName = sheetName;
}

void XLSXReader::setSheetIndex(int index) {
    m_sheetIndex = index;
}

} // namespace StudentPicker