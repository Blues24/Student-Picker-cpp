#include <QApplication>
#include <QMessageBox>
#include <exception>
#include "gui/MainWindow.hpp"
#include "core/logger.hpp"
#include "core/global.hpp"


using namespace StudentPicker;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QApplication::setApplicationName(GlobalConf::APP_NAME);
    QApplication::setApplicationVersion(GlobalConf::APP_VERSION);
    QApplication::setOrganizationName(GlobalConf::APP_DEVELOPER);

    Logger::info("========================================");
    Logger::info("Starting", GlobalConf::APP_NAME, "v" + GlobalConf::APP_VERSION);
    Logger::info("========================================");

    try {
        MainWindow window;
        window.show();

        Logger::info("Application started successfully");

        // run event loop
        int result = app.exec();

        Logger::info("Application exited with code:", result);
        Logger::info("========================================");
        
        return result;
    } catch (const std::exception& err){
        QString errorMsg = QString("Fatal error: %1").arg(err.what());
        Logger::error(errorMsg);

        QMessageBox::critical(nullptr, "Fatal Error", errorMsg);
        return 1;
    }
}
