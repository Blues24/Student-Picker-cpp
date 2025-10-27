#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <QDebug>
#include <QString>
#include <QDateTime>
#include <QTextStream>

namespace StudentPicker {

class Logger {
public:
    enum class Level {
        INFO,
        WARNING,
        ERROR
    };
    
    // Template function untuk info log
    template<typename... Args>
    static void info(Args&&... args) {
#ifdef QT_DEBUG
        log(Level::INFO, std::forward<Args>(args)...);
#endif
    }
    
    // Template function untuk warning log
    template<typename... Args>
    static void warn(Args&&... args) {
#ifdef QT_DEBUG
        log(Level::WARNING, std::forward<Args>(args)...);
#endif
    }
    
    // Template function untuk error log
    template<typename... Args>
    static void error(Args&&... args) {
#ifdef QT_DEBUG
        log(Level::ERROR, std::forward<Args>(args)...);
#endif
    }
    
private:
    static QString getLevelPrefix(Level level) {
        switch(level) {
            case Level::INFO:    return "[INFO]";
            case Level::WARNING: return "[WARN]";
            case Level::ERROR:   return "[ERROR]";
            default:             return "[LOG]";
        }
    }
    
    // Single argument version
    template<typename T>
    static void log(Level level, T&& arg) {
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QString prefix = getLevelPrefix(level);
        qDebug().noquote() << timestamp << prefix << std::forward<T>(arg);
    }
    
    // Multiple arguments version - build string first
    template<typename First, typename Second, typename... Rest>
    static void log(Level level, First&& first, Second&& second, Rest&&... rest) {
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QString prefix = getLevelPrefix(level);
        
        QString message;
        QTextStream stream(&message);
        stream << first << " " << second;
        appendToStream(stream, std::forward<Rest>(rest)...);
        
        qDebug().noquote() << timestamp << prefix << message;
    }
    
    // Helper to append remaining arguments
    template<typename First, typename... Rest>
    static void appendToStream(QTextStream& stream, First&& first, Rest&&... rest) {
        stream << " " << first;
        if constexpr (sizeof...(rest) > 0) {
            appendToStream(stream, std::forward<Rest>(rest)...);
        }
    }
    
    // Base case
    static void appendToStream(QTextStream& stream) {
        // Do nothing
    }
};

} // namespace StudentPicker

#endif // LOGGER_HPP
