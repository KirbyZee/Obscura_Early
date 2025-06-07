#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QDebug>
#include <QScreen>
#include "mainwindow.h"

void loadStyleSheet(QApplication &app) {
    // Try to load the external stylesheet first
    QFile styleFile(":/style.qss");
    if (!styleFile.exists()) {
        // If resource !=  exist, try loading from current directory
        styleFile.setFileName("style.qss");
    }

    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        QString styleSheet = stream.readAll();
        app.setStyleSheet(styleSheet);
        qDebug() << "Stylesheet loaded successfully";
    } else {
        qDebug() << "Failed to load stylesheet, using embedded styles";

        // Fallback stylesheet
        QString embeddedStyle = R"(
            QMainWindow {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                    stop:0 #0f0f23, stop:1 #1a1a2e);
                color: #e6e6fa;
                font-family: 'Segoe UI', sans-serif;
            }

            QPushButton {
                background: rgba(147, 112, 219, 0.15);
                border: 2px solid rgba(147, 112, 219, 0.3);
                border-radius: 12px;
                color: #e6e6fa;
                font-weight: 500;
                padding: 12px 20px;
            }

            QPushButton:hover {
                background: rgba(147, 112, 219, 0.25);
                border-color: rgba(147, 112, 219, 0.6);
                color: #dda0dd;
            }

            QLineEdit {
                background: rgba(30, 30, 50, 0.8);
                border: 2px solid rgba(147, 112, 219, 0.3);
                border-radius: 10px;
                color: #e6e6fa;
                padding: 14px 18px;
            }

            QLineEdit:focus {
                border-color: rgba(147, 112, 219, 0.8);
                background: rgba(30, 30, 50, 1.0);
            }
        )";

        app.setStyleSheet(embeddedStyle);
    }
}

void setupFonts() {
    // Set up modern fonts
    QFont defaultFont("Segoe UI", 10);
    defaultFont.setStyleHint(QFont::SansSerif);
    QApplication::setFont(defaultFont);

    // Load custom fonts if available
    QStringList fontPaths = {
        ":/fonts/SegoeUI.ttf",
        ":/fonts/Roboto-Regular.ttf"
    };

    for (const QString &fontPath : fontPaths) {
        QFontDatabase::addApplicationFont(fontPath);
    }
}

int main(int argc, char *argv[]) {
    
    QApplication app(argc, argv);

    // Properties
    app.setApplicationName("Obscura - ");
    app.setApplicationVersion("2.0");
    app.setOrganizationName("SecureVault");
    app.setApplicationDisplayName("Password manager (test build v.92)");

    // Setup fonts
    setupFonts();

    // Load stylesheets
    loadStyleSheet(app);

    // Create and show the main window
    MainWindow window;

    // Set window properties
    window.setWindowTitle("Obscura - Secure Password Vault");
    window.setWindowIcon(QIcon(":/icons/vault-icon.png"));

    // Center the window on screen
    window.show();
    window.resize(1200, 800);

    // Move to center of screen 
    QScreen *primaryScreen = app.primaryScreen();
    if (primaryScreen) {
        QRect screenGeometry = primaryScreen->geometry();
        int x = (screenGeometry.width() - window.width()) / 2;
        int y = (screenGeometry.height() - window.height()) / 2;
        window.move(x, y);
    }

    qDebug() << "Obscura Password Vault started successfully";

    return app.exec();
}
