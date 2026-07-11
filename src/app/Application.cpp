#include "Application.h"
#include "ui/MainWindow.h"
#include "ui/SettingsDialog.h"
#include "utils/AppSettings.h"
#include "core/SessionStore.h"
#include "core/ConversationManager.h"
#include "network/OllamaClient.h"

#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv)
{
    setApplicationName("Desktop Agent");
    setApplicationVersion("0.1.0");
    setOrganizationName("DesktopAgent");
}

Application::~Application()
{
    if (m_mainWindow && m_settings) {
        m_settings->setMainWindowGeometry(m_mainWindow->saveGeometry());
        m_settings->setMainWindowState(m_mainWindow->saveState());
    }
}

int Application::run()
{
    if (m_translator.load(":/translations/desktop_agent_zh_CN.qm"))
        installTranslator(&m_translator);

    loadSettings();
    setupObjectOwnership();
    wireConnections();
    applySettings();
    loadSessions();

    if (!m_settings->mainWindowGeometry().isEmpty())
        m_mainWindow->restoreGeometry(m_settings->mainWindowGeometry());

    m_mainWindow->show();

    int result = exec();

    auto sessions = m_conversationManager->allSessions();
    if (!sessions.isEmpty())
        m_settings->setLastSessionId(sessions.first()->id);

    return result;
}

void Application::loadSettings()
{
    m_settings = std::make_unique<AppSettings>(this);
}

void Application::setupObjectOwnership()
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_sessionStore = std::make_unique<SessionStore>(dataDir + "/sessions", this);

    m_ollamaClient = std::make_unique<OllamaClient>(this);

    m_conversationManager = std::make_unique<ConversationManager>(
        m_ollamaClient.get(), m_sessionStore.get(), this);

    m_mainWindow = std::make_unique<MainWindow>();
    m_mainWindow->setWindowTitle(tr("Desktop Agent"));
    m_mainWindow->resize(900, 600);
}

void Application::applySettings()
{
    m_conversationManager->setModelEndpoint(m_settings->endpoint());
    m_conversationManager->setModelName(m_settings->model());
    m_conversationManager->setModelTemperature(m_settings->temperature());
    m_conversationManager->setModelMaxTokens(m_settings->maxTokens());
    m_conversationManager->setModelContextTokens(m_settings->contextTokens());
}

void Application::wireConnections()
{
    // MainWindow → ConversationManager
    connect(m_mainWindow.get(), &MainWindow::newSessionRequested,
            m_conversationManager.get(), &ConversationManager::createSession);
    connect(m_mainWindow.get(), &MainWindow::deleteSessionRequested,
            m_conversationManager.get(), &ConversationManager::deleteSession);
    connect(m_mainWindow.get(), &MainWindow::settingsRequested,
            this, [this]() {
        SettingsDialog dlg(m_mainWindow.get());
        dlg.loadFromSettings(*m_settings);
        if (dlg.exec() == QDialog::Accepted) {
            dlg.saveToSettings(*m_settings);
            applySettings();
        }
    });

    // ConversationManager → MainWindow
    connect(m_conversationManager.get(), &ConversationManager::sessionCreated,
            m_mainWindow.get(), &MainWindow::onSessionCreated);
    connect(m_conversationManager.get(), &ConversationManager::sessionDeleted,
            m_mainWindow.get(), &MainWindow::onSessionDeleted);
    connect(m_conversationManager.get(), &ConversationManager::messageAdded,
            m_mainWindow.get(), &MainWindow::onMessageAdded);
    connect(m_conversationManager.get(), &ConversationManager::messageContentUpdated,
            m_mainWindow.get(), &MainWindow::onMessageContentUpdated);
    connect(m_conversationManager.get(), &ConversationManager::streamingStarted,
            m_mainWindow.get(), &MainWindow::onStreamingStarted);
    connect(m_conversationManager.get(), &ConversationManager::streamingFinished,
            m_mainWindow.get(), &MainWindow::onStreamingFinished);
    connect(m_conversationManager.get(), &ConversationManager::streamingError,
            m_mainWindow.get(), &MainWindow::onStreamingError);

    // MainWindow chat input → ConversationManager
    connect(m_mainWindow.get(), &MainWindow::userMessageSubmitted,
            m_conversationManager.get(), &ConversationManager::sendUserMessage);
}

void Application::loadSessions()
{
    auto sessions = m_sessionStore->loadAll();
    for (auto& session : sessions) {
        m_conversationManager->loadSession(session);
    }

    if (!sessions.isEmpty()) {
        QString lastId = m_settings->lastSessionId();
        if (!lastId.isEmpty() && m_conversationManager->session(lastId)) {
            m_mainWindow->displaySession(lastId);
        } else {
            m_mainWindow->displaySession(sessions.first().id);
        }
    } else {
        m_conversationManager->createSession();
    }
}
