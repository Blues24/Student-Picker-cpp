#include "userPreference.hpp"
#include "logger.hpp"


namespace StudentPicker {

// Definisi static constant
const QString UserConfig::KEY_LAST_SELECTED_CLASS = "selection/lastClass";
const QString UserConfig::KEY_LAST_DATABASE_PATH = "database/lastPath";
const QString UserConfig::KEY_LAST_IMPORT_PATH = "import/lastPath";
const QString UserConfig::KEY_WINDOW_GEOMETRY = "window/geometry";
const QString UserConfig::KEY_WINDOW_STATE = "window/state";

UserConfig::UserConfig(){
    QString configPath = GlobalConf::getConfigPath();
    m_settings = new QSettings(configPath, QSettings::IniFormat);
    Logger::info("User Config Manager has been initialized. At: ", configPath);

}

UserConfig::~UserConfig(){
    delete m_settings;
}

UserConfig& UserConfig::instance(){
    static UserConfig instance;
    return instance;
}

void UserConfig::setValue(const QString& key, const QVariant& value){
    m_settings->setValue(key, value);
    m_settings->sync();
    Logger::info("Config saved:", key, "=", value.toString());
}

QVariant UserConfig::getValue(const QString& key, const QVariant& defaultValue) const{
    return m_settings->value(key, defaultValue);
}

void UserConfig::removeValue(const QString& key) {
    m_settings->remove(key);
    Logger::info("Config has been removed: ", key);
}

bool UserConfig::containsKey(const QString& key) const {
    return m_settings->contains(key);
}

void UserConfig::clearConf(){
    m_settings->clear();
    Logger::warn("All configurations has been exterminated");
}

}