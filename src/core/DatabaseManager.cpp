#include "DatabaseManager.hpp"
#include "logger.hpp"
#include "global.hpp"
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
        CREATE TABLEIF NOT EXISTS classes (
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
}