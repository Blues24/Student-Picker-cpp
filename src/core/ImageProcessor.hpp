#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

#include <QString>
#include <QByteArray>
#include <QImage>
#include <QPixmap>

namespace StudentPicker {

class ImageProcessor {
public:
    ImageProcessor();
    ~ImageProcessor();
    
    // Load image dari file
    bool loadFromFile(const QString& filePath);
    
    // Load image dari QByteArray (dari database)
    bool loadFromData(const QByteArray& data);
    
    // Compress image
    QByteArray compress(int quality = 85);
    
    // Get compressed data (siap disimpan ke database)
    QByteArray getCompressedData(int targetSizeKB = 200, int quality = 85);
    
    // Get QPixmap untuk ditampilkan di GUI
    QPixmap getPixmap(int width = 0, int height = 0) const;
    
    // Get original QImage
    QImage getImage() const;
    
    // Cek apakah image valid
    bool isValid() const;
    
    // Get error message
    QString getLastError() const;
    
    // Static helper: Compress existing byte array
    static QByteArray compressData(const QByteArray& data, int targetSizeKB = 200);
    
    // Static helper: Get pixmap from byte array
    static QPixmap pixmapFromData(const QByteArray& data, int width = 0, int height = 0);
    
private:
    QImage m_image;
    QString m_lastError;
};

} // namespace StudentPicker

#endif // IMAGEPROCESSOR_HPP