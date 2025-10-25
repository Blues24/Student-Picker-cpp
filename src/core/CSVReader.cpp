#include "CSVReader.hpp"
#include "Logger.hpp"
#include <QFile>
#include <QTextStream>

namespace StudentPicker {

CSVReader::CSVReader() 
    : m_delimiter(','), m_hasHeader(true) {
}

CSVReader::~CSVReader() {
}

bool CSVReader::readFile(const QString& filePath) {
    m_data.clear();
    m_headers.clear();
    m_lastError.clear();
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = "Cannot open file: " + filePath;
        Logger::error(m_lastError);
        return false;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    
    bool firstLine = true;
    int lineNumber = 0;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineNumber++;
        
        if (line.isEmpty()) {
            continue;
        }
        
        QStringList fields = parseLine(line);
        
        // First line adalah header
        if (firstLine && m_hasHeader) {
            m_headers = fields;
            firstLine = false;
            Logger::info("CSV Headers:", fields.join(", "));
            continue;
        }
        
        // Jika tidak ada header, buat header otomatis
        if (firstLine && !m_hasHeader) {
            for (int i = 0; i < fields.size(); i++) {
                m_headers.append(QString("Column_%1").arg(i + 1));
            }
            firstLine = false;
        }
        
        // Parse data
        if (fields.size() != m_headers.size()) {
            Logger::warn("Line", lineNumber, "has", fields.size(), 
                        "fields but expected", m_headers.size());
            // Skip line yang tidak sesuai
            continue;
        }
        
        QVariantMap row;
        for (int i = 0; i < m_headers.size(); i++) {
            row[m_headers[i]] = fields[i];
        }
        
        m_data.append(row);
    }
    
    file.close();
    
    Logger::info("CSV file read successfully:", filePath);
    Logger::info("Total rows:", m_data.size());
    
    return true;
}

QStringList CSVReader::parseLine(const QString& line) {
    QStringList fields;
    QString currentField;
    bool inQuotes = false;
    
    for (int i = 0; i < line.length(); i++) {
        QChar c = line[i];
        
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == m_delimiter && !inQuotes) {
            fields.append(currentField.trimmed());
            currentField.clear();
        } else {
            currentField.append(c);
        }
    }
    
    // Tambahkan field terakhir
    fields.append(currentField.trimmed());
    
    return fields;
}

QVector<QVariantMap> CSVReader::getData() const {
    return m_data;
}

QStringList CSVReader::getHeaders() const {
    return m_headers;
}

QString CSVReader::getLastError() const {
    return m_lastError;
}

void CSVReader::setDelimiter(QChar delimiter) {
    m_delimiter = delimiter;
}

void CSVReader::setHasHeader(bool hasHeader) {
    m_hasHeader = hasHeader;
}

} // namespace StudentPicker