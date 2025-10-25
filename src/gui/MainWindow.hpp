#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "StudentTableModel.hpp"
#include "../core/DatabaseManager.hpp"

namespace StudentPicker {

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
protected:
    void closeEvent(QCloseEvent* event) override;
    
private slots:
    // Slot untuk tombol-tombol
    void onImportClicked();
    void onPickRandomClicked();
    void onUploadPhotoClicked();
    void onClearDatabaseClicked();
    void onRefreshClicked();
    
    // Slot untuk class selection
    void onClassChanged(int index);
    
    // Slot untuk table selection
    void onTableSelectionChanged();
    
private:
    // Setup UI
    void setupUI();
    void setupMenuBar();
    void applyStyles();
    
    // Helper functions
    void loadClasses();
    void loadStudents();
    void loadStudentsByClass(const QString& className);
    void displaySelectedStudent();
    void importCSV(const QString& filePath);
    void importXLSX(const QString& filePath);
    void saveWindowState();
    void restoreWindowState();
    
    // UI Components
    QWidget* m_centralWidget;
    QVBoxLayout* m_mainLayout;
    
    // Top controls
    QHBoxLayout* m_topLayout;
    QPushButton* m_importButton;
    QComboBox* m_classComboBox;
    QPushButton* m_pickRandomButton;
    QPushButton* m_refreshButton;
    
    // Table
    QTableView* m_tableView;
    StudentTableModel* m_tableModel;
    
    // Bottom section - Selected student display
    QHBoxLayout* m_bottomLayout;
    
    // Left side - Photo
    QLabel* m_photoLabel;
    QPushButton* m_uploadPhotoButton;
    
    // Right side - Info
    QVBoxLayout* m_infoLayout;
    QLabel* m_nameLabel;
    QLabel* m_studentIdLabel;
    QLabel* m_classLabel;
    
    // Status bar
    QLabel* m_statusLabel;
    
    // Data
    int m_selectedStudentId;
    QVector<QVariantMap> m_classes;
};

} // namespace StudentPicker

#endif // MAINWINDOW_HPP