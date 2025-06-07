#include "mainwindow.h"
#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QClipboard>
#include <QMessageBox>
#include <QEasingCurve>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), sidebarVisible(true) {

    setWindowTitle("Obscura - Password Vault");
    setMinimumSize(1000, 700);
    resize(1200, 800);

    applyModernStyling();

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create sidebar
    sidebar = createSidebar();
    mainLayout->addWidget(sidebar);

    // Create pages stack
    pages = new QStackedWidget;
    pages->addWidget(createPasswordsPage());
    pages->addWidget(createAddPasswordPage());
    pages->setStyleSheet("QStackedWidget { background: #1a1a2e; border-left: 2px solid #16213e; }");

    mainLayout->addWidget(pages);

    setupAnimations();
    loadPasswordsFromFile();
    pages->setCurrentIndex(0); // Start with passwords page
}

MainWindow::~MainWindow() {}

void MainWindow::applyModernStyling() {
    setStyleSheet(R"(
        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #0f0f23, stop:1 #1a1a2e);
        }
        QPushButton {
            background: rgba(147, 112, 219, 0.1);
            border: 2px solid rgba(147, 112, 219, 0.3);
            border-radius: 12px;
            color: #e6e6fa;
            font-family: 'Segoe UI', sans-serif;
            font-weight: 500;
            padding: 12px 20px;
            transition: all 0.3s ease;
        }
        QPushButton:hover {
            background: rgba(147, 112, 219, 0.25);
            border-color: rgba(147, 112, 219, 0.6);
            color: #dda0dd;
            transform: translateY(-2px);
        }
        QPushButton:pressed {
            background: rgba(147, 112, 219, 0.35);
            transform: translateY(1px);
        }
        QLineEdit {
            background: rgba(30, 30, 50, 0.8);
            border: 2px solid rgba(147, 112, 219, 0.3);
            border-radius: 8px;
            color: #e6e6fa;
            font-family: 'Segoe UI', sans-serif;
            font-size: 14px;
            padding: 12px 16px;
        }
        QLineEdit:focus {
            border-color: rgba(147, 112, 219, 0.8);
            background: rgba(30, 30, 50, 1.0);
            box-shadow: 0 0 15px rgba(147, 112, 219, 0.3);
        }
        QLabel { color: #e6e6fa; font-family: 'Segoe UI', sans-serif; }
        QScrollArea { border: none; background: transparent; }
        QScrollBar:vertical {
            background: rgba(30, 30, 50, 0.5);
            width: 12px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background: rgba(147, 112, 219, 0.6);
            border-radius: 6px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background: rgba(147, 112, 219, 0.8);
        }
    )");
}

QWidget *MainWindow::createSidebar() {
    QWidget *side = new QWidget;
    side->setFixedWidth(250);
    side->setStyleSheet(R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(22, 33, 62, 0.95), stop:1 rgba(15, 15, 35, 0.95));
            border-right: 2px solid rgba(147, 112, 219, 0.2);
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(side);
    layout->setContentsMargins(20, 30, 20, 30);
    layout->setSpacing(20);

    // Logo area
    QLabel *logo = new QLabel("Obscura v.92");
    logo->setStyleSheet(R"(
        QLabel {
            font-size: 20px;
            font-weight: bold;
            color: #dda0dd;
            padding: 15px;
            background: rgba(147, 112, 219, 0.1);
            border-radius: 12px;
            border: 2px solid rgba(147, 112, 219, 0.3);
        }
    )");
    logo->setAlignment(Qt::AlignCenter);
    layout->addWidget(logo);

    // Build type
    toggleButton = new QPushButton("Early build");
    toggleButton->setStyleSheet(R"(
        QPushButton {
            font-size: 10px;
            font-weight: bold;
            color: #dda0dd;
            padding: 5px;
            background: rgba(147, 112, 219, 0.1);
            border-radius: 12px;
            border: 2px solid rgba(147, 112, 219, 0.3);
        }
    )");
    connect(toggleButton, &QPushButton::clicked, this, &MainWindow::toggleSidebar);
    layout->addWidget(toggleButton);

    layout->addSpacing(20);

    // Navigation buttons
    QPushButton *passBtn = createSidebarButton("Passwords", "passwords");
    QPushButton *addBtn = createSidebarButton("Add New", "add");
    QPushButton *adminBtn = createSidebarButton("Admin Panel", "admin");

    connect(passBtn, &QPushButton::clicked, this, &MainWindow::showPasswords);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::showAddPassword);

    layout->addWidget(passBtn);
    layout->addWidget(addBtn);
    layout->addWidget(adminBtn);

    layout->addStretch();

    // User info at bottom
    QLabel *userInfo = new QLabel("User: Admin");
    userInfo->setStyleSheet(R"(
        QLabel {
            font-size: 12px;
            color: rgba(230, 230, 250, 0.7);
            padding: 10px;
            background: rgba(0, 0, 0, 0.2);
            border-radius: 8px;
        }
    )");
    userInfo->setAlignment(Qt::AlignCenter);
    layout->addWidget(userInfo);

    return side;
}

QPushButton *MainWindow::createSidebarButton(const QString &text, const QString &icon) {
    QPushButton *btn = new QPushButton(text);
    btn->setStyleSheet(R"(
        QPushButton {
            text-align: left;
            font-size: 16px;
            padding: 15px 20px;
            border-radius: 10px;
            background: rgba(147, 112, 219, 0.05);
            border: 1px solid rgba(147, 112, 219, 0.2);
        }
        QPushButton:hover {
            background: rgba(147, 112, 219, 0.15);
            border-color: rgba(147, 112, 219, 0.4);
            transform: translateX(5px);
        }
    )");
    return btn;
}

QWidget *MainWindow::createPasswordsPage() {
    QWidget *page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    // Header with search
    QHBoxLayout *headerLayout = new QHBoxLayout;

    QLabel *title = new QLabel("Password Vault");
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: #dda0dd;");
    headerLayout->addWidget(title);

    headerLayout->addStretch();

    searchBox = new QLineEdit;
    searchBox->setPlaceholderText("Search passwords...");
    searchBox->setMaximumWidth(300);
    connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    headerLayout->addWidget(searchBox);

    layout->addLayout(headerLayout);

    // Passwords scroll area
    passwordsScrollArea = new QScrollArea;
    passwordsScrollArea->setWidgetResizable(true);
    passwordsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *scrollWidget = new QWidget;
    passwordsLayout = new QVBoxLayout(scrollWidget);
    passwordsLayout->setSpacing(15);

    // Samples 
    passwordsLayout->addWidget(createPasswordCard("GitHub", "username@email.com", "2 hours ago"));
    passwordsLayout->addWidget(createPasswordCard("Gmail", "username@email.com", "1 day ago"));
    passwordsLayout->addWidget(createPasswordCard("Netflix", "username@email.com", "3 days ago"));
    passwordsLayout->addWidget(createPasswordCard("Amazon", "username@email.com", "1 week ago"));

    passwordsLayout->addStretch();
    passwordsScrollArea->setWidget(scrollWidget);
    layout->addWidget(passwordsScrollArea);

    return page;
}

QWidget *MainWindow::createAddPasswordPage() {
    QWidget *page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    QLabel *title = new QLabel("Add New Password");
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: #dda0dd; margin-bottom: 30px;");
    layout->addWidget(title);

    // Form fields
    QFormLayout *formLayout = new QFormLayout;
    formLayout->setSpacing(20);

    titleInput = new QLineEdit;
    titleInput->setPlaceholderText("Enter service name (e.g., Gmail, GitHub)");
    titleInput->setStyleSheet("font-size: 16px; padding: 15px;");

    usernameInput = new QLineEdit;
    usernameInput->setPlaceholderText("Enter username or email");
    usernameInput->setStyleSheet("font-size: 16px; padding: 15px;");

    passwordInput = new QLineEdit;
    passwordInput->setPlaceholderText("Enter password");
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setStyleSheet("font-size: 16px; padding: 15px;");

    // Labels
    QLabel *titleLabel = new QLabel("Service Name:");
    QLabel *usernameLabel = new QLabel("Username/Email:");
    QLabel *passwordLabel = new QLabel("Password:");

    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #dda0dd;");
    usernameLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #dda0dd;");
    passwordLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #dda0dd;");

    formLayout->addRow(titleLabel, titleInput);
    formLayout->addRow(usernameLabel, usernameInput);
    formLayout->addRow(passwordLabel, passwordInput);

    layout->addLayout(formLayout);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    QPushButton *saveButton = new QPushButton("Save Password");
    saveButton->setFixedSize(150, 50);
    saveButton->setStyleSheet(R"(
        QPushButton {
            font-size: 16px;
            font-weight: bold;
            background: rgba(34, 197, 94, 0.2);
            border: 2px solid rgba(34, 197, 94, 0.4);
            color: #e6e6fa;
        }
        QPushButton:hover {
            background: rgba(34, 197, 94, 0.4);
            border-color: rgba(34, 197, 94, 0.7);
        }
    )");
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveNewPassword);

    QPushButton *clearButton = new QPushButton("Clear Form");
    clearButton->setFixedSize(150, 50);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearAddForm);

    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout);
    layout->addStretch();

    return page;
}

PasswordCard *MainWindow::createPasswordCard(const QString &title, const QString &username, const QString &lastUsed) {
    return new PasswordCard(title, username, lastUsed);
}

void MainWindow::setupAnimations() {
    sidebarAnimation = new QPropertyAnimation(sidebar, "maximumWidth");
    sidebarAnimation->setDuration(400);
    sidebarAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void MainWindow::toggleSidebar() {
    int targetWidth = sidebarVisible ? 0 : 250;
    sidebarAnimation->setStartValue(sidebar->maximumWidth());
    sidebarAnimation->setEndValue(targetWidth);
    sidebarAnimation->start();
    sidebarVisible = !sidebarVisible;
}

void MainWindow::showPasswords() {
    pages->setCurrentIndex(0);
}

void MainWindow::showAddPassword() {
    pages->setCurrentIndex(1);
}

void MainWindow::onSearchTextChanged(const QString &text) {
    // Filter implementation goes here
    Q_UNUSED(text);
}

// PasswordCard Implementation
PasswordCard::PasswordCard(const QString &title, const QString &username, const QString &lastUsed, QWidget *parent)
    : QFrame(parent), m_title(title), m_username(username), m_lastUsed(lastUsed) {
    setupUI();
    applyCardStyling();

    // Setup hover animation
    hoverAnimation = new QPropertyAnimation(this, "geometry");
    hoverAnimation->setDuration(200);
    hoverAnimation->setEasingCurve(QEasingCurve::OutQuad);
}

void PasswordCard::setupUI() {
    setFixedHeight(100);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(20, 15, 20, 15);

    // Left side - Info
    QVBoxLayout *infoLayout = new QVBoxLayout;

    titleLabel = new QLabel(m_title);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #dda0dd;");

    usernameLabel = new QLabel(m_username);
    usernameLabel->setStyleSheet("font-size: 14px; color: rgba(230, 230, 250, 0.8);");

    lastUsedLabel = new QLabel("Last used: " + m_lastUsed);
    lastUsedLabel->setStyleSheet("font-size: 12px; color: rgba(230, 230, 250, 0.6);");

    infoLayout->addWidget(titleLabel);
    infoLayout->addWidget(usernameLabel);
    infoLayout->addWidget(lastUsedLabel);

    layout->addLayout(infoLayout);
    layout->addStretch();

    // Right side - Actions
    QHBoxLayout *actionsLayout = new QHBoxLayout;

    copyButton = new QPushButton("Copy");
    copyButton->setFixedSize(80, 50);
    copyButton->setToolTip("Copy Password");
    copyButton->setStyleSheet(R"(
        QPushButton {
            border-radius: 18px;
            font-size: 10px;
            font-weight: bold;
            background: rgba(147, 112, 219, 0.2);
            border: 2px solid rgba(147, 112, 219, 0.4);
            color: #e6e6fa;
        }
        QPushButton:hover {
            background: rgba(147, 112, 219, 0.4);
            border-color: rgba(147, 112, 219, 0.7);
            color: #dda0dd;
            transform: scale(1.05);
        }
        QPushButton:pressed {
            background: rgba(147, 112, 219, 0.6);
            transform: scale(0.95);
        }
    )");
    connect(copyButton, &QPushButton::clicked, this, &PasswordCard::copyPassword);

    editButton = new QPushButton("Edit");
    editButton->setFixedSize(80, 50);
    editButton->setToolTip("Edit Password");
    editButton->setStyleSheet(R"(
        QPushButton {
            border-radius: 18px;
            font-size: 11px;
            font-weight: bold;
            background: rgba(147, 112, 219, 0.2);
            border: 2px solid rgba(147, 112, 219, 0.4);
            color: #e6e6fa;
        }
        QPushButton:hover {
            background: rgba(147, 112, 219, 0.4);
            border-color: rgba(147, 112, 219, 0.7);
            color: #dda0dd;
            transform: scale(1.05);
        }
        QPushButton:pressed {
            background: rgba(147, 112, 219, 0.6);
            transform: scale(0.95);
        }
    )");
    connect(editButton, &QPushButton::clicked, this, &PasswordCard::editPassword);

   // Delete button
    deleteButton = new QPushButton("Delete");
    deleteButton->setFixedSize(80, 50);
    deleteButton->setToolTip("Delete Password");
    deleteButton->setStyleSheet(R"(
    QPushButton {
        border-radius: 18px;
        font-size: 10px;
        font-weight: bold;
        background: rgba(239, 68, 68, 0.2);
        border: 2px solid rgba(239, 68, 68, 0.4);
        color: #e6e6fa;
    }
    QPushButton:hover {
        background: rgba(239, 68, 68, 0.4);
        border-color: rgba(239, 68, 68, 0.7);
        color: #dda0dd;
        transform: scale(1.05);
    }
    QPushButton:pressed {
        background: rgba(239, 68, 68, 0.6);
        transform: scale(0.95);
    }
)");
    connect(deleteButton, &QPushButton::clicked, this, &PasswordCard::removePassword);

    actionsLayout->addWidget(copyButton);
    actionsLayout->addWidget(editButton);
    actionsLayout->addWidget(deleteButton); 

    actionsLayout->addWidget(copyButton);
    actionsLayout->addWidget(editButton);

    layout->addLayout(actionsLayout);
}

void PasswordCard::applyCardStyling() {
    setStyleSheet(R"(
        PasswordCard {
            background: rgba(147, 112, 219, 0.08);
            border: 2px solid rgba(147, 112, 219, 0.2);
            border-radius: 12px;
        }
    )");
}

void PasswordCard::enterEvent(QEnterEvent *event) {
    setStyleSheet(R"(
        PasswordCard {
            background: rgba(147, 112, 219, 0.15);
            border: 2px solid rgba(147, 112, 219, 0.4);
            border-radius: 12px;
        }
    )");
    QFrame::enterEvent(event);
}

void PasswordCard::leaveEvent(QEvent *event) {
    setStyleSheet(R"(
        PasswordCard {
            background: rgba(147, 112, 219, 0.08);
            border: 2px solid rgba(147, 112, 219, 0.2);
            border-radius: 12px;
        }
    )");
    QFrame::leaveEvent(event);
}

void PasswordCard::copyPassword() {
    QApplication::clipboard()->setText("dummy_password_" + m_title);
    QString originalText = copyButton->text();
    copyButton->setText("COPIED!");
    copyButton->setStyleSheet(R"(
        QPushButton {
            border-radius: 18px;
            font-size: 10px;
            font-weight: bold;
            background: rgba(34, 197, 94, 0.3);
            border: 2px solid rgba(34, 197, 94, 0.6);
            color: #e6e6fa;
        }
    )");

    QTimer::singleShot(2000, [this, originalText]() {
        copyButton->setText(originalText);
        copyButton->setStyleSheet(R"(
            QPushButton {
                border-radius: 18px;
                font-size: 11px;
                font-weight: bold;
                background: rgba(147, 112, 219, 0.2);
                border: 2px solid rgba(147, 112, 219, 0.4);
                color: #e6e6fa;
            }
            QPushButton:hover {
                background: rgba(147, 112, 219, 0.4);
                border-color: rgba(147, 112, 219, 0.7);
                color: #dda0dd;
                transform: scale(1.05);
            }
        )");
    });
}

void PasswordCard::editPassword() {
    QMessageBox::information(this, "Edit Password", "Edit " + m_title + "pass func nigga");
}

void PasswordCard::removePassword() {
    int ret = QMessageBox::question(this, "Confirm Delete",
                                    "Are you sure you want to delete the password for " + m_title + "?",
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        emit removeRequested();
    }
}

void MainWindow::addPassword(const QString &title, const QString &username, const QString &password) {
    PasswordEntry entry;
    entry.title = title;
    entry.username = username;
    entry.password = password;
    entry.lastUsed = "Just now";

    passwords.append(entry);
    refreshPasswordsList();
    savePasswordsToFile();
}

void MainWindow::removePassword(int index) {
    if (index >= 0 && index < passwords.size()) {
        passwords.removeAt(index);
        refreshPasswordsList();
        savePasswordsToFile();
    }
}

void MainWindow::refreshPasswordsList() {
    // Clear existing password cards
    QLayoutItem *item;
    while ((item = passwordsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    // Add updated password cards
    for (int i = 0; i < passwords.size(); ++i) {
        const PasswordEntry &entry = passwords[i];
        PasswordCard *card = createPasswordCard(entry.title, entry.username, entry.lastUsed);

        // Connect remove signal
        connect(card, &PasswordCard::removeRequested, [this, i]() {
            removePassword(i);
        });

        passwordsLayout->addWidget(card);
    }

    passwordsLayout->addStretch();
}

void MainWindow::saveNewPassword() {
    QString title = titleInput->text().trimmed();
    QString username = usernameInput->text().trimmed();
    QString password = passwordInput->text().trimmed();

    if (title.isEmpty() || username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill in all fields.");
        return;
    }

    addPassword(title, username, password);
    clearAddForm();

    QMessageBox::information(this, "Success", "Password saved successfully!");
    showPasswords(); // Switch back to passwords page
}

void MainWindow::clearAddForm() {
    titleInput->clear();
    usernameInput->clear();
    passwordInput->clear();
}

void MainWindow::savePasswordsToFile() {
    QFile file("passwords.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const PasswordEntry &entry : passwords) {
            out << entry.title << "|" << entry.username << "|" << entry.password << "|" << entry.lastUsed << "\n";
        }
    }
}

void MainWindow::loadPasswordsFromFile() {
    QFile file("passwords.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        passwords.clear();
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split("|");
            if (parts.size() == 4) {
                PasswordEntry entry;
                entry.title = parts[0];
                entry.username = parts[1];
                entry.password = parts[2];
                entry.lastUsed = parts[3];
                passwords.append(entry);
            }
        }
        refreshPasswordsList();
    }
}

#include "mainwindow.moc"
