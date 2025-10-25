#ifndef CSVREADER_HPP
#define CSVREADER_HPP

#include <QString>
#include <QVector>
#include <QVariantMap>

namespace StudentPicker {

class CSVReader {
public:
    CSVReader();
    ~CSVReader();
    
    // Baca file CSV
    bool readFile(const QString& filePath);
    
    // Get data yang sudah dibaca
    QVector<QVariantMap> getData() const;
    
    // Get headers
    QStringList getHeaders() const;
    
    // Get error message
    QString getLastError() const;
    
    // Set delimiter (default: koma)
    void setDelimiter(QChar delimiter);
    
    // Set apakah file punya header atau tidak
    void setHasHeader(bool hasHeader);
    
private:
    // Parse satu baris CSV
    QStringList parseLine(const QString& line);
    
    QVector<QVariantMap> m_data;
    QStringList m_headers;
    QString m_lastError;
    QChar m_delimiter;
    bool m_hasHeader;
};

} // namespace StudentPicker

#endif // CSVREADER_HPP