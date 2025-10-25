#ifndef STUDENTTABLEMODEL_HPP
#define STUDENTTABLEMODEL_HPP

#include <QAbstractTableModel>
#include <QVector>
#include "../core/DatabaseManager.hpp"

namespace StudentPicker {

class StudentTableModel : public QAbstractTableModel {
    Q_OBJECT
    
public:
    explicit StudentTableModel(QObject* parent = nullptr);
    
    // QAbstractTableModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    // Custom methods
    void setStudents(const QVector<Student>& students);
    void addStudent(const Student& student);
    void updateStudent(int row, const Student& student);
    void removeStudent(int row);
    void clear();
    
    Student getStudent(int row) const;
    QVector<Student> getAllStudents() const;
    
private:
    QVector<Student> m_students;
    QStringList m_headers;
};

} // namespace StudentPicker

#endif // STUDENTTABLEMODEL_HPP