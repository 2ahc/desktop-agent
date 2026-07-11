#pragma once

#include <QApplication>
#include <QTranslator>
#include <memory>

class MainWindow;
class AppSettings;
class SessionStore;
class OllamaClient;
class ConversationManager;

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int& argc, char** argv);
    ~Application();

    int run();

private:
    void setupObjectOwnership();
    void wireConnections();
    void loadSettings();
    void loadSessions();
    void applySettings();

    std::unique_ptr<AppSettings> m_settings;
    std::unique_ptr<SessionStore> m_sessionStore;
    std::unique_ptr<OllamaClient> m_ollamaClient;
    std::unique_ptr<ConversationManager> m_conversationManager;
    std::unique_ptr<MainWindow> m_mainWindow;
    QTranslator m_translator;
};
