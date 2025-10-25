#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include "qcontainerfwd.h"
#include "qsqlquery.h"
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
    int getClassID(const QString& className);

    // Check for existing class
    bool classExists(const QString& className);
    
    bool addStudent(const Student& student);

    bool updateStudent(const Student& student);

    bool deleteStudentById(int studentId);

    Student getStudentId(int studentId);

    QVector<Student> getAllStudents();

    QVector<Student> getStudentsByClassId(int classId);

    QVector<Student> getStudentsByClassName(const QString& className);

    // Search students name
    QVector<Student> searchStudentsName(const QString& keyword);

    // Pick random student from class
    Student getRandomStudentClassId(int classId);
    Student getRandomStudentClassName(const QString& className);

    // Count student
    int countStudents();
    int countStudentsByClass(int classId);

    bool importStudentsFile(const QVector<Student>& students);

    bool clearAllStudents();

    QString getLastError() const;

private:
    DatabaseManager();
    ~DatabaseManager();

    // Create students database table
    bool createTables();

    Student resultToStudent(const QSqlQuery& s_query);

    QSqlDatabase m_database;
    QString m_lastError;
    static const QString CONNECTION_NAME;
};
}

#endif