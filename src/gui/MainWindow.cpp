#include "MainWindow.hpp"
#include "../core/logger.hpp"
#include "../core/userPreference.hpp"
#include "../core/CSVReader.hpp"
#include "../core/XLSXReader.hpp"
#include "../core/ImageProcessor.hpp"
#include "../core/global.hpp"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QGroupBox>
#include <QApplication>
#include <QStatusBar>
#include <QCloseEvent>

namespace StudentPicker {

// ==================== CONSTRUCTOR ====================

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_selectedStudentId(-1) {
    
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

// ==================== DESTRUCTOR ====================

MainWindow::~MainWindow() {
    saveWindowState();
}

// ==================== SETUP UI ====================

void MainWindow::setupUI() {
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
    
    m_tableView->setColumnWidth(0, 50);
    m_tableView->setColumnWidth(1, 200);
    m_tableView->setColumnWidth(2, 150);
    m_tableView->setColumnWidth(3, 150);
    m_tableView->setColumnWidth(4, 100);
    
    connect(m_tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onTableSelectionChanged);
    
    m_mainLayout->addWidget(m_tableView, 2);
    
    // === SELECTED STUDENT DISPLAY ===
    QGroupBox* studentDisplayGroup = new QGroupBox("Selected Student", this);
    QVBoxLayout* studentDisplayLayout = new QVBoxLayout(studentDisplayGroup);
    
    m_bottomLayout = new QHBoxLayout();
    
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
    
    setWindowTitle(GlobalConf::APP_NAME + " v" + GlobalConf::APP_VERSION);
    resize(1000, 800);
}

// ==================== SETUP MENU BAR ====================

void MainWindow::setupMenuBar() {
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    QMenu* fileMenu = menuBar->addMenu("&File");
    
    QAction* importAction = fileMenu->addAction("📥 Import Data");
    connect(importAction, &QAction::triggered, this, &MainWindow::onImportClicked);
    
    fileMenu->addSeparator();
    
    QAction* exitAction = fileMenu->addAction("❌ Exit");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    
    QMenu* dbMenu = menuBar->addMenu("&Database");
    
    QAction* refreshAction = dbMenu->addAction("🔄 Refresh");
    connect(refreshAction, &QAction::triggered, this, &MainWindow::onRefreshClicked);
    
    dbMenu->addSeparator();
    
    QAction* clearAction = dbMenu->addAction("🗑️ Clear All Data");
    connect(clearAction, &QAction::triggered, this, &MainWindow::onClearDatabaseClicked);
    
    QMenu* helpMenu = menuBar->addMenu("&Help");
    
    QAction* aboutAction = helpMenu->addAction("ℹ️ About");
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "About " + GlobalConf::APP_NAME,
            GlobalConf::APP_NAME + " v" + GlobalConf::APP_VERSION + "\n\n"
            "A simple application for random student selection.\n\n"
            "Built with Qt6 and C++");
    });
}

// ==================== APPLY STYLES ====================

void MainWindow::applyStyles() {
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

// ==================== HELPER FUNCTIONS ====================

void MainWindow::loadClasses() {
    m_classComboBox->clear();
    m_classComboBox->addItem("All Classes", -1);
    
    m_classes = DatabaseManager::instance().getAllClasses();
    
    for (const QVariantMap& classData : m_classes) {
        QString className = classData["name"].toString();
        int classId = classData["id"].toInt();
        m_classComboBox->addItem(className, classId);
    }
    
    Logger::info("Loaded", m_classes.size(), "classes");
}

void MainWindow::loadStudents() {
    QVector<Student> students = DatabaseManager::instance().getAllStudents();
    m_tableModel->setStudents(students);
    
    m_statusLabel->setText(QString("Total: %1 students").arg(students.size()));
    Logger::info("Loaded", students.size(), "students");
}

void MainWindow::loadStudentsByClass(const QString& className) {
    QVector<Student> students;
    
    if (className == "All Classes") {
        students = DatabaseManager::instance().getAllStudents();
    } else {
        students = DatabaseManager::instance().getStudentsByClassName(className);
    }
    
    m_tableModel->setStudents(students);
    
    m_statusLabel->setText(QString("Showing %1 students from %2")
                          .arg(students.size())
                          .arg(className));
    
    m_pickRandomButton->setEnabled(!students.isEmpty() && className != "All Classes");
    
    Logger::info("Loaded", students.size(), "students from class:", className);
}

void MainWindow::displaySelectedStudent() {
    if (m_selectedStudentId == -1) {
        m_nameLabel->setText("Name: -");
        m_studentIdLabel->setText("Student ID: -");
        m_classLabel->setText("Class: -");
        m_photoLabel->setText("No Photo");
        m_photoLabel->setPixmap(QPixmap());
        m_uploadPhotoButton->setEnabled(false);
        return;
    }
    
    Student student = DatabaseManager::instance().getStudentId(m_selectedStudentId);
    
    if (student.id == -1) {
        Logger::warn("Student not found:", m_selectedStudentId);
        return;
    }
    
    m_nameLabel->setText("Name: " + student.name);
    m_studentIdLabel->setText("Student ID: " + student.studentId);
    m_classLabel->setText("Class: " + student.className);
    
    if (student.photoData.isEmpty()) {
        m_photoLabel->setText("No Photo Available");
        m_photoLabel->setPixmap(QPixmap());
    } else {
        QPixmap pixmap = ImageProcessor::pixmapFromData(
            student.photoData, 
            GlobalConf::DISPLAY_IMAGE_WIDTH, 
            GlobalConf::DISPLAY_IMAGE_HEIGHT
        );
        m_photoLabel->setPixmap(pixmap);
    }
    
    m_uploadPhotoButton->setEnabled(true);
    
    Logger::info("Displaying student:", student.name);
}

void MainWindow::importCSV(const QString& filePath) {
    CSVReader reader;
    
    if (!reader.readFile(filePath)) {
        QMessageBox::critical(this, "Import Error", 
            "Failed to read CSV file:\n" + reader.getLastError());
        return;
    }
    
    QVector<QVariantMap> data = reader.getData();
    QStringList headers = reader.getHeaders();
    
    if (!headers.contains("Name") || !headers.contains("StudentID") || !headers.contains("Class")) {
        QMessageBox::warning(this, "Import Warning",
            "CSV file must contain columns: Name, StudentID, Class\n\n"
            "Found columns: " + headers.join(", "));
        return;
    }
    
    QVector<Student> students;
    for (const QVariantMap& row : data) {
        Student student;
        student.name = row["Name"].toString();
        student.studentId = row["StudentID"].toString();
        student.className = row["Class"].toString();
        student.photoData = QByteArray();
        
        students.append(student);
    }
    
    if (DatabaseManager::instance().importStudentsFile(students)) {
        QMessageBox::information(this, "Import Success",
            QString("Successfully imported %1 students!").arg(students.size()));
        
        loadClasses();
        loadStudents();
        
        UserConfig::instance().setValue(
            UserConfig::KEY_LAST_IMPORT_PATH, 
            filePath
        );
    } else {
        QMessageBox::critical(this, "Import Error",
            "Failed to import students:\n" + 
            DatabaseManager::instance().getLastError());
    }
}

void MainWindow::importXLSX(const QString& filePath) {
    XLSXReader reader;
    
    if (!reader.readFile(filePath)) {
        QMessageBox::information(this, "XLSX Not Supported", 
            reader.getLastError());
        return;
    }
}

void MainWindow::saveWindowState() {
    UserConfig::instance().setValue(
        UserConfig::KEY_WINDOW_GEOMETRY, 
        saveGeometry()
    );
    UserConfig::instance().setValue(
        UserConfig::KEY_WINDOW_STATE, 
        saveState()
    );
    
    Logger::info("Window state saved");
}

void MainWindow::restoreWindowState() {
    QByteArray geometry = UserConfig::instance().getValue(
        UserConfig::KEY_WINDOW_GEOMETRY
    ).toByteArray();
    
    QByteArray state = UserConfig::instance().getValue(
        UserConfig::KEY_WINDOW_STATE
    ).toByteArray();
    
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }
    
    if (!state.isEmpty()) {
        restoreState(state);
    }
    
    Logger::info("Window state restored");
}

void MainWindow::closeEvent(QCloseEvent* event) {
    saveWindowState();
    QMainWindow::closeEvent(event);
}

// ==================== SLOTS ====================

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
    
    Student randomStudent = DatabaseManager::instance().getRandomStudentClassName(currentClass);
    
    if (randomStudent.id == -1) {
        QMessageBox::warning(this, "Error",
            "Failed to pick random student.");
        return;
    }
    
    m_selectedStudentId = randomStudent.id;
    displaySelectedStudent();
    
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
    
    Student student = DatabaseManager::instance().getStudentId(m_selectedStudentId);
    student.photoData = compressedData;
    
    if (DatabaseManager::instance().updateStudent(student)) {
        QMessageBox::information(this, "Success",
            "Photo uploaded successfully!");
        
        displaySelectedStudent();
        loadStudents();
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
    Q_UNUSED(index);
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