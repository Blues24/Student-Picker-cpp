#include "ImageProcessor.hpp"
#include "logger.hpp"
#include "Global.hpp"
#include <QBuffer>
#include <QImageReader>

namespace StudentPicker {

ImageProcessor::ImageProcessor() {
}

ImageProcessor::~ImageProcessor() {
}

bool ImageProcessor::loadFromFile(const QString& filePath) {
    m_lastError.clear();
    
    QImageReader reader(filePath);
    reader.setAutoTransform(true);
    
    m_image = reader.read();
    
    if (m_image.isNull()) {
        m_lastError = "Failed to load image: " + reader.errorString();
        Logger::error(m_lastError);
        return false;
    }
    
    Logger::info("Image loaded:", filePath, "Size:", m_image.width(), "x", m_image.height());
    return true;
}

bool ImageProcessor::loadFromData(const QByteArray& data) {
    m_lastError.clear();
    
    if (data.isEmpty()) {
        m_lastError = "Empty image data";
        Logger::error(m_lastError);
        return false;
    }
    
    m_image.loadFromData(data);
    
    if (m_image.isNull()) {
        m_lastError = "Failed to load image from data";
        Logger::error(m_lastError);
        return false;
    }
    
    Logger::info("Image loaded from data. Size:", m_image.width(), "x", m_image.height());
    return true;
}

QByteArray ImageProcessor::compress(int quality) {
    if (m_image.isNull()) {
        Logger::error("Cannot compress: image is null");
        return QByteArray();
    }
    
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    
    // Save sebagai JPEG dengan quality yang ditentukan
    m_image.save(&buffer, "JPEG", quality);
    
    buffer.close();
    
    Logger::info("Image compressed. Size:", data.size() / 1024, "KB, Quality:", quality);
    return data;
}

QByteArray ImageProcessor::getCompressedData(int targetSizeKB, int quality) {
    if (m_image.isNull()) {
        Logger::error("Cannot get compressed data: image is null");
        return QByteArray();
    }
    
    QByteArray data = compress(quality);
    int sizeKB = data.size() / 1024;
    
    // Jika ukuran sudah sesuai target, return
    if (sizeKB <= targetSizeKB) {
        Logger::info("Image size OK:", sizeKB, "KB (target:", targetSizeKB, "KB)");
        return data;
    }
    
    // Jika masih terlalu besar, turunkan quality secara bertahap
    int currentQuality = quality;
    while (sizeKB > targetSizeKB && currentQuality > 20) {
        currentQuality -= 10;
        data = compress(currentQuality);
        sizeKB = data.size() / 1024;
        Logger::info("Trying quality:", currentQuality, "Size:", sizeKB, "KB");
    }
    
    // Jika masih terlalu besar, resize image
    if (sizeKB > targetSizeKB) {
        Logger::info("Resizing image to meet target size");
        
        int newWidth = m_image.width() * 0.8;
        int newHeight = m_image.height() * 0.8;
        
        QImage resized = m_image.scaled(newWidth, newHeight, 
                                       Qt::KeepAspectRatio, 
                                       Qt::SmoothTransformation);
        
        QByteArray resizedData;
        QBuffer buffer(&resizedData);
        buffer.open(QIODevice::WriteOnly);
        resized.save(&buffer, "JPEG", quality);
        buffer.close();
        
        data = resizedData;
        sizeKB = data.size() / 1024;
        Logger::info("After resize:", sizeKB, "KB");
    }
    
    Logger::info("Final compressed size:", sizeKB, "KB");
    return data;
}

QPixmap ImageProcessor::getPixmap(int width, int height) const {
    if (m_image.isNull()) {
        return QPixmap();
    }
    
    QPixmap pixmap = QPixmap::fromImage(m_image);
    
    if (width > 0 || height > 0) {
        // Jika hanya salah satu dimensi yang diberikan, maintain aspect ratio
        if (width > 0 && height <= 0) {
            height = pixmap.height() * width / pixmap.width();
        } else if (height > 0 && width <= 0) {
            width = pixmap.width() * height / pixmap.height();
        }
        
        pixmap = pixmap.scaled(width, height, 
                              Qt::KeepAspectRatio, 
                              Qt::SmoothTransformation);
    }
    
    return pixmap;
}

QImage ImageProcessor::getImage() const {
    return m_image;
}

bool ImageProcessor::isValid() const {
    return !m_image.isNull();
}

QString ImageProcessor::getLastError() const {
    return m_lastError;
}

// Static helpers

QByteArray ImageProcessor::compressData(const QByteArray& data, int targetSizeKB) {
    ImageProcessor processor;
    if (!processor.loadFromData(data)) {
        return QByteArray();
    }
    return processor.getCompressedData(targetSizeKB);
}

QPixmap ImageProcessor::pixmapFromData(const QByteArray& data, int width, int height) {
    if (data.isEmpty()) {
        return QPixmap();
    }
    
    QPixmap pixmap;
    pixmap.loadFromData(data);
    
    if (width > 0 || height > 0) {
        if (width > 0 && height <= 0) {
            height = pixmap.height() * width / pixmap.width();
        } else if (height > 0 && width <= 0) {
            width = pixmap.width() * height / pixmap.height();
        }
        
        pixmap = pixmap.scaled(width, height, 
                              Qt::KeepAspectRatio, 
                              Qt::SmoothTransformation);
    }
    
    return pixmap;
}

} // namespace StudentPicker