#include "MainWindow.hpp"
#include "../core/logger.hpp"
#include "../core/userPreference.hpp"
#include "../core/CSVReader.hpp"
#include "../core/XLSXReader.hpp"
#include "../core/ImageProcessor.hpp"
#include "../core/global.hpp"
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QGroupBox>
#include <QApplication>

namespace StudentPicker {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_selectedStudentId(-1) {
    
    // Initialize database
    if (!DatabaseManager::instance().initDb()) {
        QMessageBox::critical(this, "Error", 
            "Failed to initialize database: " + 
            DatabaseManager::instance().getLastError());
        QApplication::quit();
        return;
    }
    
    setupUI();
    setupMenuBar();
    applyStyles();
    restoreWindowState();
    loadClasses();
    
    Logger::info("MainWindow initialized");
}

MainWindow::~MainWindow() {
    saveWindowState();
}

void MainWindow::setupUI() {
    // Central widget
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // === TOP CONTROLS ===
    m_topLayout = new QHBoxLayout();
    
    m_importButton = new QPushButton("📥 Import CSV/XLSX", this);
    m_importButton->setMinimumHeight(40);
    connect(m_importButton, &QPushButton::clicked, this, &MainWindow::onImportClicked);
    
    m_classComboBox = new QComboBox(this);
    m_classComboBox->setMinimumHeight(40);
    m_classComboBox->setMinimumWidth(200);
    connect(m_classComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &MainWindow::onClassChanged);
    
    m_pickRandomButton = new QPushButton("🎲 Pick Random Student", this);
    m_pickRandomButton->setMinimumHeight(40);
    m_pickRandomButton->setEnabled(false);
    connect(m_pickRandomButton, &QPushButton::clicked, this, &MainWindow::onPickRandomClicked);
    
    m_refreshButton = new QPushButton("🔄 Refresh", this);
    m_refreshButton->setMinimumHeight(40);
    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    
    m_topLayout->addWidget(m_importButton);
    m_topLayout->addWidget(new QLabel("Class:", this));
    m_topLayout->addWidget(m_classComboBox);
    m_topLayout->addWidget(m_pickRandomButton);
    m_topLayout->addStretch();
    m_topLayout->addWidget(m_refreshButton);
    
    m_mainLayout->addLayout(m_topLayout);
    
    // === TABLE VIEW ===
    m_tableModel = new StudentTableModel(this);
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_tableModel);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->setMinimumHeight(300);
    
    // Column widths
    m_tableView->setColumnWidth(0, 50);   // ID
    m_tableView->setColumnWidth(1, 200);  // Name
    m_tableView->setColumnWidth(2, 150);  // Student ID
    m_tableView->setColumnWidth(3, 150);  // Class
    m_tableView->setColumnWidth(4, 100);  // Has Photo
    
    connect(m_tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onTableSelectionChanged);
    
    m_mainLayout->addWidget(m_tableView, 2);
    
    // === SELECTED STUDENT DISPLAY ===
    QGroupBox* studentDisplayGroup = new QGroupBox("Selected Student", this);
    QVBoxLayout* studentDisplayLayout = new QVBoxLayout(studentDisplayGroup);
    
    m_bottomLayout = new QHBoxLayout();
    
    // Left side - Photo
    QVBoxLayout* photoLayout = new QVBoxLayout();
    m_photoLabel = new QLabel(this);
    m_photoLabel->setFixedSize(GlobalConf::DISPLAY_IMAGE_WIDTH, GlobalConf::DISPLAY_IMAGE_HEIGHT);
    m_photoLabel->setAlignment(Qt::AlignCenter);
    m_photoLabel->setStyleSheet("QLabel { background-color: #f0f0f0; border: 2px dashed #cccccc; }");
    m_photoLabel->setText("No Photo");
    
    m_uploadPhotoButton = new QPushButton("📷 Upload Photo", this);
    m_uploadPhotoButton->setEnabled(false);
    connect(m_uploadPhotoButton, &QPushButton::clicked, this, &MainWindow::onUploadPhotoClicked);
    
    photoLayout->addWidget(m_photoLabel);
    photoLayout->addWidget(m_uploadPhotoButton);
    photoLayout->addStretch();
    
    // Right side - Info
    m_infoLayout = new QVBoxLayout();
    m_infoLayout->setSpacing(10);
    
    m_nameLabel = new QLabel("Name: -", this);
    m_nameLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    
    m_studentIdLabel = new QLabel("Student ID: -", this);
    m_studentIdLabel->setStyleSheet("font-size: 14px;");
    
    m_classLabel = new QLabel("Class: -", this);
    m_classLabel->setStyleSheet("font-size: 14px;");
    
    m_infoLayout->addWidget(m_nameLabel);
    m_infoLayout->addWidget(m_studentIdLabel);
    m_infoLayout->addWidget(m_classLabel);
    m_infoLayout->addStretch();
    
    m_bottomLayout->addLayout(photoLayout);
    m_bottomLayout->addLayout(m_infoLayout, 1);
    
    studentDisplayLayout->addLayout(m_bottomLayout);
    m_mainLayout->addWidget(studentDisplayGroup, 1);
    
    // === STATUS BAR ===
    m_statusLabel = new QLabel("Ready", this);
    statusBar()->addWidget(m_statusLabel);
    
    // Window settings
    setWindowTitle(GlobalConf::APP_NAME + " v" + GlobalConf::APP_VERSION);
    resize(1000, 800);
}

void MainWindow::setupMenuBar() {
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    // File Menu
    QMenu* fileMenu = menuBar->addMenu("&File");
    
    QAction* importAction = fileMenu->addAction("📥 Import Data");
    connect(importAction, &QAction::triggered, this, &MainWindow::onImportClicked);
    
    fileMenu->addSeparator();
    
    QAction* exitAction = fileMenu->addAction("❌ Exit");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    
    // Database Menu
    QMenu* dbMenu = menuBar->addMenu("&Database");
    
    QAction* refreshAction = dbMenu->addAction("🔄 Refresh");
    connect(refreshAction, &QAction::triggered, this, &MainWindow::onRefreshClicked);
    
    dbMenu->addSeparator();
    
    QAction* clearAction = dbMenu->addAction("🗑️ Clear All Data");
    connect(clearAction, &QAction::triggered, this, &MainWindow::onClearDatabaseClicked);
    
    // Help Menu
    QMenu* helpMenu = menuBar->addMenu("&Help");
    
    QAction* aboutAction = helpMenu->addAction("ℹ️ About");
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "About " + GlobalConf::APP_NAME,
            GlobalConf::APP_NAME + " v" + GlobalConf::APP_VERSION + "\n\n"
            "A simple application for random student selection.\n\n"
            "Built with Qt6 and C++");
    });
}

void MainWindow::applyStyles() {
    // Zen Light Mode styling
    QString styleSheet = R"(
        QMainWindow {
            background-color: #ffffff;
        }
        
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            padding: 10px;
            border-radius: 5px;
            font-size: 14px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #45a049;
        }
        
        QPushButton:pressed {
            background-color: #3d8b40;
        }
        
        QPushButton:disabled {
            background-color: #cccccc;
            color: #666666;
        }
        
        QComboBox {
            background-color: #f5f5f5;
            border: 2px solid #e0e0e0;
            border-radius: 5px;
            padding: 8px;
            font-size: 14px;
        }
        
        QComboBox:hover {
            border-color: #4CAF50;
        }
        
        QTableView {
            background-color: #ffffff;
            alternate-background-color: #f9f9f9;
            border: 1px solid #e0e0e0;
            border-radius: 5px;
            gridline-color: #e0e0e0;
        }
        
        QTableView::item:selected {
            background-color: #4CAF50;
            color: white;
        }
        
        QHeaderView::section {
            background-color: #f5f5f5;
            padding: 8px;
            border: none;
            border-bottom: 2px solid #e0e0e0;
            font-weight: bold;
            font-size: 13px;
        }
        
        QGroupBox {
            border: 2px solid #e0e0e0;
            border-radius: 5px;
            margin-top: 10px;
            font-weight: bold;
            font-size: 14px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
        }
        
        QLabel {
            color: #333333;
        }
        
        QStatusBar {
            background-color: #f5f5f5;
            border-top: 1px solid #e0e0e0;
        }
    )";
    
    setStyleSheet(styleSheet);
}

// Slots implementation

void MainWindow::onImportClicked() {
    QString lastPath = UserConfig::instance().getValue(
        UserConfig::KEY_LAST_IMPORT_PATH, 
        QDir::homePath()
    ).toString();
    
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Import Student Data",
        lastPath,
        "Data Files (*.csv *.xlsx);;CSV Files (*.csv);;Excel Files (*.xlsx);;All Files (*.*)"
    );
    
    if (filePath.isEmpty()) {
        return;
    }
    
    Logger::info("Importing file:", filePath);
    
    if (filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        importCSV(filePath);
    } else if (filePath.endsWith(".xlsx", Qt::CaseInsensitive)) {
        importXLSX(filePath);
    } else {
        QMessageBox::warning(this, "Unsupported Format",
            "Please select a CSV or XLSX file.");
    }
}

void MainWindow::onPickRandomClicked() {
    QString currentClass = m_classComboBox->currentText();
    
    if (currentClass == "All Classes") {
        QMessageBox::information(this, "Select Class",
            "Please select a specific class first.");
        return;
    }
    
    int studentCount = DatabaseManager::instance().countStudentsByClass(
        DatabaseManager::instance().getClassID(currentClass)
    );
    
    if (studentCount == 0) {
        QMessageBox::information(this, "No Students",
            "No students found in this class.");
        return;
    }
    
    // Pick random student
    Student randomStudent = DatabaseManager::instance().getRandomStudentClassName(currentClass);
    
    if (randomStudent.id == -1) {
        QMessageBox::warning(this, "Error",
            "Failed to pick random student.");
        return;
    }
    
    // Display with animation-like effect (flash the selection)
    m_selectedStudentId = randomStudent.id;
    displaySelectedStudent();
    
    // Highlight in table
    QVector<Student> students = m_tableModel->getAllStudents();
    for (int i = 0; i < students.size(); i++) {
        if (students[i].id == randomStudent.id) {
            m_tableView->selectRow(i);
            m_tableView->scrollTo(m_tableModel->index(i, 0));
            break;
        }
    }
    
    m_statusLabel->setText(QString("🎲 Random Pick: %1").arg(randomStudent.name));
    
    Logger::info("Random pick:", randomStudent.name, "from", currentClass);
}

void MainWindow::onUploadPhotoClicked() {
    if (m_selectedStudentId == -1) {
        return;
    }
    
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select Student Photo",
        QDir::homePath(),
        "Images (*.png *.jpg *.jpeg *.bmp);;All Files (*.*)"
    );
    
    if (filePath.isEmpty()) {
        return;
    }
    
    // Load and compress image
    ImageProcessor processor;
    if (!processor.loadFromFile(filePath)) {
        QMessageBox::critical(this, "Error",
            "Failed to load image:\n" + processor.getLastError());
        return;
    }
    
    QByteArray compressedData = processor.getCompressedData(
        GlobalConf::MAX_IMAGE_SIZE_KB
    );
    
    if (compressedData.isEmpty()) {
        QMessageBox::critical(this, "Error",
            "Failed to compress image.");
        return;
    }
    
    // Update student photo in database
    Student student = DatabaseManager::instance().getStudentId(m_selectedStudentId);
    student.photoData = compressedData;
    
    if (DatabaseManager::instance().updateStudent(student)) {
        QMessageBox::information(this, "Success",
            "Photo uploaded successfully!");
        
        // Refresh display
        displaySelectedStudent();
        loadStudents(); // Refresh table to update "Has Photo" column
    } else {
        QMessageBox::critical(this, "Error",
            "Failed to upload photo:\n" + 
            DatabaseManager::instance().getLastError());
    }
}

void MainWindow::onClearDatabaseClicked() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Clear Database",
        "Are you sure you want to delete ALL student data?\n\n"
        "This action cannot be undone!",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (DatabaseManager::instance().clearAllStudents()) {
            QMessageBox::information(this, "Success",
                "All student data has been cleared.");
            
            m_selectedStudentId = -1;
            loadClasses();
            loadStudents();
            displaySelectedStudent();
        } else {
            QMessageBox::critical(this, "Error",
                "Failed to clear database:\n" + 
                DatabaseManager::instance().getLastError());
        }
    }
}

void MainWindow::onRefreshClicked() {
    loadClasses();
    
    QString currentClass = m_classComboBox->currentText();
    if (currentClass.isEmpty()) {
        loadStudents();
    } else {
        loadStudentsByClass(currentClass);
    }
    
    m_statusLabel->setText("Refreshed");
    Logger::info("Data refreshed");
}

void MainWindow::onClassChanged(int index) {
    QString className = m_classComboBox->currentText();
    loadStudentsByClass(className);
}

void MainWindow::onTableSelectionChanged() {
    QModelIndexList selection = m_tableView->selectionModel()->selectedRows();
    
    if (selection.isEmpty()) {
        m_selectedStudentId = -1;
        displaySelectedStudent();
        return;
    }
    
    int row = selection.first().row();
    Student student = m_tableModel->getStudent(row);
    m_selectedStudentId = student.id;
    
    displaySelectedStudent();
}

} // namespace StudentPicker