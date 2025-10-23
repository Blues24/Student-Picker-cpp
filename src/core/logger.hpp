#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <QDebug>
#include <QString>
#include <QDateTime>

namespace StudentPicker {

class Logger {
public:
    enum class debugMode {
        INFO,
        WARNING,
        ERROR
    };
    // Template untuk debug mode info
    template<typename... Args>
    static void info(Args&&... args){
#ifdef QT_DEBUG
        log(debugMode::INFO, std::forward<Args>(args)...);
#endif
    }

    // Template untuk debug mode WARNING
    template<typename... Args>
    static void warn(Args&&... args){
#ifdef QT_DEBUG
        log(debugMode::WARNING, std::forward<Args>(args)...);
#endif
    }

    // Template untuk debug mode ERROR
    template<typename... Args>
    static void error(Args&&... args) {
#ifdef QT_DEBUG
        log(debugMode::ERROR, std::forward<Args>(args)...);
#endif
    }

private:
    static QString getModePrefix(debugMode Mode){
        switch(Mode){
            case debugMode::INFO:
                return "[INFO]";
            case debugMode::WARNING:
                return "[WARN]";
            case debugMode::ERROR:
                return "[ERROR]";
            default:
                return "[LOG]";
        }
    }
};
}

#endif