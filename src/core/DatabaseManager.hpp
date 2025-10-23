#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include "qcontainerfwd.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVector>
#include <QVariantMap>
#include <QByteArray>

namespace StudentPicker{

struct Student {
    int id;
    int classId;
    QString name;
    QString studentId;
    QString className;
    QByteArray photoData;

    Student() : id(-1), classId(-1) {}
};

class DatabaseManager {
public:

    // Singleton pattern
    static DatabaseManager& instance();

    // Delete copy constructor and assignment operator
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Database initialize
    bool initDb(const QString& dbPath = QString());

    // Close the database
    void closeDb();

    // Check if the Db is still open
    bool isOpen() const;

    // CRUD operation for classes

    // Add new class table
    bool addClass(const QString& className);

    // Fetch all classes in the database
    QVector<QVariantMap> getAllClasses();

    // Get class ID by name
    int getClassID_byName(const QString& className);

    // Check for existing class
    bool classExists(const QString& className);
    
};
}

#endif