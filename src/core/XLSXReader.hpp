#ifndef XLSXREADER_HPP
#define XLSXREADER_HPP

#include <QString>
#include <QVector>
#include <QVariantMap>

namespace StudentPicker {

class XLSXReader {
public:
    XLSXReader();
    ~XLSXReader();
    
    // Baca file XLSX
    bool readFile(const QString& filePath);
    
    // Get data yang sudah dibaca
    QVector<QVariantMap> getData() const;
    
    // Get headers
    QStringList getHeaders() const;
    
    // Get error message
    QString getLastError() const;
    
    // Set sheet name yang akan dibaca (default: sheet pertama)
    void setSheetName(const QString& sheetName);
    
    // Set sheet index yang akan dibaca (default: 0)
    void setSheetIndex(int index);
    
private:
    QVector<QVariantMap> m_data;
    QStringList m_headers;
    QString m_lastError;
    QString m_sheetName;
    int m_sheetIndex;
};

} // namespace StudentPicker

#endif // XLSXREADER_HPP