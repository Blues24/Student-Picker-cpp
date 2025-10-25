#include "StudentTableModel.hpp"

namespace StudentPicker {

StudentTableModel::StudentTableModel(QObject* parent)
    : QAbstractTableModel(parent) {
    m_headers << "ID" << "Name" << "Student ID" << "Class" << "Has Photo";
}

int StudentTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_students.size();
}

int StudentTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_headers.size();
}

QVariant StudentTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_students.size()) {
        return QVariant();
    }
    
    const Student& student = m_students[index.row()];
    
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return student.id;
            case 1: return student.name;
            case 2: return student.studentId;
            case 3: return student.className;
            case 4: return student.photoData.isEmpty() ? "No" : "Yes";
            default: return QVariant();
        }
    }
    
    if (role == Qt::TextAlignmentRole) {
        if (index.column() == 0 || index.column() == 4) {
            return Qt::AlignCenter;
        }
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    }
    
    return QVariant();
}

QVariant StudentTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    
    if (orientation == Qt::Horizontal && section < m_headers.size()) {
        return m_headers[section];
    }
    
    if (orientation == Qt::Vertical) {
        return section + 1;
    }
    
    return QVariant();
}

void StudentTableModel::setStudents(const QVector<Student>& students) {
    beginResetModel();
    m_students = students;
    endResetModel();
}

void StudentTableModel::addStudent(const Student& student) {
    beginInsertRows(QModelIndex(), m_students.size(), m_students.size());
    m_students.append(student);
    endInsertRows();
}

void StudentTableModel::updateStudent(int row, const Student& student) {
    if (row >= 0 && row < m_students.size()) {
        m_students[row] = student;
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }
}

void StudentTableModel::removeStudent(int row) {
    if (row >= 0 && row < m_students.size()) {
        beginRemoveRows(QModelIndex(), row, row);
        m_students.removeAt(row);
        endRemoveRows();
    }
}

void StudentTableModel::clear() {
    beginResetModel();
    m_students.clear();
    endResetModel();
}

Student StudentTableModel::getStudent(int row) const {
    if (row >= 0 && row < m_students.size()) {
        return m_students[row];
    }
    return Student();
}

QVector<Student> StudentTableModel::getAllStudents() const {
    return m_students;
}

} // namespace StudentPicker