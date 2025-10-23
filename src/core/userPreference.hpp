#ifndef USERPREFERENCE_HPP
#define USERPREFERENCE_HPP

#include <QSettings>
#include <QString>
#include <QVariant>


namespace StudentPicker {

class UserConfig {
public:
    static UserConfig& instance();

    // Delete copy constructor and assignment operator
    UserConfig(const UserConfig&) = delete;
    UserConfig& operator=(const UserConfig&) = delete;
    
    // Save user configuration value
    void setValue(const QString& key, const QVariant& value);
    
    // Fetch user configuration value
    QVariant getValue(const QString& key, const QVariant& defaultValue = QVariant()) const;

    // Delete user configuration value
    void removeValue(const QString& key);

    //check is there any key exists
    bool containsKey(const QString& key) const;

    // Reset the user config
    void clearConf();

    static const QString KEY_LAST_DATABASE_PATH;
    static const QString KEY_LAST_IMPORT_PATH;
    static const QString KEY_WINDOW_GEOMETRY;
    static const QString KEY_WINDOW_STATE;
    static const QString KEY_LAST_SELECTED_CLASS;

private:
    UserConfig();
    ~UserConfig();

    QSettings* m_settings;
};
}

#endif