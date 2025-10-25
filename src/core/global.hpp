#ifndef GLOBAL_HPP
#define GLOBAL_HPP 

// Include QT library
#include "qstandardpaths.h"
#include <QString>
#include <QStandardPaths>
#include <QDir>

namespace StudentPicker {


namespace GlobalConf {
    // App info and metadata
    const QString APP_NAME = "Zencore Student Picker";
    const QString APP_VERSION = "1.0.0";
    const QString APP_DEVELOPER = "Blues24";

    // File config
    const QString DATABASE_NAME = "students.db";
    const QString CONFIG_NAME = "app.ini";

    // Function to get appdata directory
    inline QString getDataPath(){
        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir;
        // Ensure the APPDATA directory is exist
        if (!dir.exists(dataPath)){
            dir.mkpath(dataPath);
        }

        return dataPath;

    }

    inline QString getDatabasePath(){
        return getDataPath() + "/" + DATABASE_NAME;
    }

    inline QString getConfigPath(){
        return getDataPath() + "/" + CONFIG_NAME;
    }

    const int MAX_IMAGE_SIZE_KB = 300; // in kilobytes
    const int MIN_IMAGE_SIZE_KB = 100; // in kilobytes

    // GUI window size when presenting the image
    const int DISPLAY_IMAGE_WIDTH = 300;
    const int DISPLAY_IMAGE_HEIGHT = 400;

}
}

#endif