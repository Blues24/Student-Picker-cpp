#include "DatabaseManager.hpp"
#include "logger.hpp"
#include "global.hpp"
#include "qcontainerfwd.h"
#include "qsqldatabase.h"
#include "qsqlquery.h"
#include <QSqlRecord>
#include <QVariant>
#include <QRandomGenerator>


namespace StudentPicker {

const QString DatabaseManager::CONNECTION_NAME = "StudentPickerDB";

DatabaseManager::DatabaseManager(){
    Logger::info("DatabaseManager has been created");

}

DatabaseManager::~DatabaseManager(){
    closeDb();
}

DatabaseManager& DatabaseManager::instance(){
    static DatabaseManager instance;
    return instance;

}

bool DatabaseManager::initDb(const QString& dbPath){
    QString path = dbPath.isEmpty() ? GlobalConf::getDatabasePath() : dbPath;

    if ( QSqlDatabase::contains(CONNECTION_NAME) ){
        QSqlDatabase::removeDatabase(CONNECTION_NAME);
    }

    m_database = QSqlDatabase::addDatabase("QSQLITE", CONNECTION_NAME);
    m_database.setDatabaseName(path);

    if (!m_database.open()){
        m_lastError = m_database.lastError().text();
        Logger::error("Failed to open database: ", m_lastError);
        return false;
    }

    Logger::info("Database opened successfully: ", path);

    if (!createTables()){
        m_lastError = "Failed to create tables";
        Logger::error(m_lastError);
        return false;
    }

    return true;
}

void DatabaseManager::closeDb(){
    if (m_database.isOpen()){
        m_database.close();
        Logger::info("Database has been shutdown");
    }
}

bool DatabaseManager::isOpen() const {
    return m_database.isOpen();
}

bool DatabaseManager::createTables(){
    QSqlQuery query(m_database);

    QString createClassesTables = R"(
        CREATE TABLE IF NOT EXISTS classes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
            )   
    )";

    if (!query.exec(createClassesTables)){
        m_lastError = query.lastError().text();
        Logger::error("Failed to create classes table: ", m_lastError);
        return false;

    }

    QString createStudentsTable = R"(
        CREATE TABLE IF NOT EXISTS students (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            student_id TEXT NOT NULL UNIQUE,
            class_id INTEGER NOT NULL,
            photo BLOB,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (class_id) REFERENCES classes(id) ON DELETE CASCADE
            )
        )";
    
        if (!query.exec(createStudentsTable)){
            m_lastError = query.lastError().text();
            Logger::error("Failed to create students table: ", m_lastError);
            return false;
        }

        // Create indexes
        query.exec("CREATE INDEX IF NOT EXISTS idx_student_class ON students(class_id)");
        query.exec("CREATE INDEX IF NOT EXISTS idx_student_name ON students(name)");

        Logger::info("Database tables created successfully");
        return true;
}

bool DatabaseManager::addClass(const QString& className) {
    QSqlQuery classQuery(m_database);
    classQuery.prepare("INSERT INTO classes (name) VALUES (:name)");
    classQuery.bindValue(":name", className);
    
    if (!classQuery.exec()){
        m_lastError = classQuery.lastError().text();
        Logger::error("Failed to add class: ", m_lastError);
        return false;

    }

    Logger::info("Class added: ", className);
    return true;
}

QVector<QVariantMap> DatabaseManager::getAllClasses(){
    QVector<QVariantMap> classes;
    QSqlQuery query("SELECT id, name FROM classes ORDER BY name", m_database);

    while(query.next()){
        QVariantMap classData;
        classData["id"] = query.value(0).toInt();
        classData["name"] = query.value(1).toString();
        classes.append(classData);
    }
    
    return classes;
}

int DatabaseManager::getClassID(const QString& className){
    QVector<QVariantMap> classes;
    QSqlQuery query("SELECT id, name FROM classes ORDER BY name", m_database);
    query.bindValue(":name", className);

    if (query.exec() && query.next()){
        return query.value(0).toInt();
    }
    
    return -1;
}

bool DatabaseManager::classExists(const QString& className){
    return getClassID(className) != -1;
}

bool DatabaseManager::addStudent(const Student& student){
    if (!classExists(student.className)){
        if(!addClass(student.className)){
            return false;
        }

    }

    int classId = getClassID(student.className);

    QSqlQuery query(m_database);
    query.prepare("INSERT INTO students (name, student_id, class_id, photo)"
                    "VALUES (:name, :student_id, :class_id, :photo)");
    query.bindValue(":name", student.name);
    query.bindValue(":student_id", student.studentId);
    query.bindValue(":classId",classId);
    query.bindValue(":photo", student.photoData);

    if (!query.exec()){
        m_lastError = query.lastError().text();
        Logger::error("Failed to add student: ", m_lastError);
        return false;

    }

    Logger::info("Student added: ", student.name);
    return true;
}

bool DatabaseManager::updateStudent(const Student& student){
    QSqlQuery query(m_database);
    query.prepare("UPDATE students SET name = :name, student_id = :student_id, "
                    "class_id = :class_id, photo = : photo WHERE id = :id");

    query.bindValue(":name", student.name);
    query.bindValue(":student_id", student.studentId);
    query.bindValue(":class_id", student.classId);
    query.bindValue(":photo", student.photoData);
    query.bindValue(":id", student.id);

    if (!query.exec()){
        m_lastError = query.lastError().text();
        Logger::error("Failed to update student: ", m_lastError);
    }

    Logger::info("Student updated: ", student.name);
    return true;
}
bool DatabaseManager::deleteStudentById(int studentId) {
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM students WHERE id = :id");
    query.bindValue(":id", studentId);
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        Logger::error("Failed to delete student:", m_lastError);
        return false;
    }
    
    Logger::info("Student deleted, ID:", studentId);
    return true;
}

Student DatabaseManager::resultToStudent(const QSqlQuery& query) {
    Student student;
    student.id = query.value("id").toInt();
    student.name = query.value("name").toString();
    student.studentId = query.value("student_id").toString();
    student.classId = query.value("class_id").toInt();
    student.photoData = query.value("photo").toByteArray();
    
    // Get class name
    QSqlQuery classQuery(m_database);
    classQuery.prepare("SELECT name FROM classes WHERE id = :id");
    classQuery.bindValue(":id", student.classId);
    if (classQuery.exec() && classQuery.next()) {
        student.className = classQuery.value(0).toString();
    }
    
    return student;
}

Student DatabaseManager::getStudentId(int studentId) {
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM students WHERE id = :id");
    query.bindValue(":id", studentId);
    
    if (query.exec() && query.next()) {
        return resultToStudent(query);
    }
    
    return Student();
}

QVector<Student> DatabaseManager::getAllStudents() {
    QVector<Student> students;
    QSqlQuery query("SELECT * FROM students ORDER BY name", m_database);
    
    while (query.next()) {
        students.append(resultToStudent(query));
    }
    
    return students;
}

QVector<Student> DatabaseManager::getStudentsByClassId(int classId) {
    QVector<Student> students;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM students WHERE class_id = :class_id ORDER BY name");
    query.bindValue(":class_id", classId);
    
    if (query.exec()) {
        while (query.next()) {
            students.append(resultToStudent(query));
        }
    }
    
    return students;
}

QVector<Student> DatabaseManager::getStudentsByClassName(const QString& className) {
    int classId = getClassID(className);
    if (classId == -1) {
        return QVector<Student>();
    }
    return getStudentsByClassId(classId);
}

QVector<Student> DatabaseManager::searchStudentsName(const QString& keyword) {
    QVector<Student> students;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM students WHERE name LIKE :keyword OR student_id LIKE :keyword");
    query.bindValue(":keyword", "%" + keyword + "%");
    
    if (query.exec()) {
        while (query.next()) {
            students.append(resultToStudent(query));
        }
    }
    
    return students;
}

Student DatabaseManager::getRandomStudentClassId(int classId) {
    QVector<Student> students = getStudentsByClassId(classId);
    
    if (students.isEmpty()) {
        return Student();
    }
    
    int randomIndex = QRandomGenerator::global()->bounded(students.size());
    return students[randomIndex];
}

Student DatabaseManager::getRandomStudentClassName(const QString& className) {
    int classId = getClassID(className);
    if (classId == -1) {
        return Student();
    }
    return getRandomStudentClassId(classId);
}

int DatabaseManager::countStudents() {
    QSqlQuery query("SELECT COUNT(*) FROM students", m_database);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int DatabaseManager::countStudentsByClass(int classId) {
    QSqlQuery query(m_database);
    query.prepare("SELECT COUNT(*) FROM students WHERE class_id = :class_id");
    query.bindValue(":class_id", classId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

// ==== BATCH OPERATIONS ====

bool DatabaseManager::importStudentsFile(const QVector<Student>& students) {
    // Start transaction
    m_database.transaction();
    
    bool success = true;
    for (const Student& student : students) {
        if (!addStudent(student)) {
            success = false;
            break;
        }
    }
    
    if (success) {
        m_database.commit();
        Logger::info("Successfully imported", students.size(), "students");
    } else {
        m_database.rollback();
        Logger::error("Failed to import students, transaction rolled back");
    }
    
    return success;
}

bool DatabaseManager::clearAllStudents() {
    QSqlQuery query("DELETE FROM students", m_database);
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        Logger::error("Failed to clear students:", m_lastError);
        return false;
    }
    
    Logger::warn("All students cleared from database");
    return true;
}

QString DatabaseManager::getLastError() const {
    return m_lastError;
}

} // End of Namespace