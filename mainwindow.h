#pragma once

#include <QMainWindow>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QParallelAnimationGroup>
#include <QVector>
#include <QFormLayout>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

class PasswordCard;

class MainWindow : public QMainWindow {
    Q_OBJECT

    struct PasswordEntry {
        QString title;
        QString username;
        QString password;
        QString lastUsed;
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QWidget *createSidebar();
    QWidget *createPasswordsPage();
    QWidget *createAddPasswordPage();
    PasswordCard *createPasswordCard(const QString &title, const QString &username, const QString &lastUsed);
    QPushButton *createSidebarButton(const QString &text, const QString &icon);
    void setupAnimations();
    void applyModernStyling();

    // Add these new functions
    void addPassword(const QString &title, const QString &username, const QString &password);
    void removePassword(int index);
    void refreshPasswordsList();
    void savePasswordsToFile();
    void loadPasswordsFromFile();

    QStackedWidget *pages;
    QPushButton *toggleButton;
    QWidget *sidebar;
    QPropertyAnimation *sidebarAnimation;
    QLineEdit *searchBox;
    QScrollArea *passwordsScrollArea;
    QVBoxLayout *passwordsLayout;
    bool sidebarVisible;

    // Add these new member variables
    QVector<PasswordEntry> passwords;
    QLineEdit *titleInput;
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;

private slots:
    void toggleSidebar();
    void showPasswords();
    void showAddPassword();
    void onSearchTextChanged(const QString &text);

    // Add these new slots
    void saveNewPassword();
    void clearAddForm();
};

class PasswordCard : public QFrame {
    Q_OBJECT
public:
    PasswordCard(const QString &title, const QString &username, const QString &lastUsed, QWidget *parent = nullptr);

signals:
    void removeRequested();

private:
    void setupUI();
    void applyCardStyling();

    QString m_title;
    QString m_username;
    QString m_lastUsed;
    QLabel *titleLabel;
    QLabel *usernameLabel;
    QLabel *lastUsedLabel;
    QPushButton *copyButton;
    QPushButton *editButton;
    QPushButton *deleteButton;  
    QPropertyAnimation *hoverAnimation;

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void copyPassword();
    void editPassword();
    void removePassword();  
};
