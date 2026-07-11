#include "MainWindow.h"
#include "SessionListWidget.h"
#include "ChatWidget.h"
#include "core/Message.h"

#include <QStackedWidget>
#include <QSplitter>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFile>
#include <QApplication>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    loadStyleSheet();
    setupUi();
    setupMenuBar();
    setupToolBar();
}

void MainWindow::loadStyleSheet()
{
    QFile file(":/styles/style.qss");
    if (file.open(QIODevice::ReadOnly)) {
        qApp->setStyleSheet(QString::fromUtf8(file.readAll()));
        file.close();
    }
}

void MainWindow::setupUi()
{
    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(1);

    m_sessionList = new SessionListWidget(this);
    m_sessionList->setObjectName("sessionListWidgetContainer");
    m_sessionList->setFixedWidth(240);

    connect(m_sessionList, &SessionListWidget::sessionClicked,
            this, &MainWindow::displaySession);
    connect(m_sessionList, &SessionListWidget::newSessionClicked,
            this, &MainWindow::newSessionRequested);
    connect(m_sessionList, &SessionListWidget::deleteSessionClicked,
            this, &MainWindow::deleteSessionRequested);

    m_chatStack = new QStackedWidget(this);

    auto* placeholder = new QWidget(m_chatStack);
    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setAlignment(Qt::AlignCenter);

    auto* placeholderIcon = new QLabel(placeholder);
    placeholderIcon->setText(QString::fromUtf8("\xF0\x9F\x90\xB4")); // horse
    placeholderIcon->setAlignment(Qt::AlignCenter);
    placeholderIcon->setStyleSheet("font-size: 64px; background: transparent; border: none;");
    placeholderLayout->addWidget(placeholderIcon);

    auto* placeholderText = new QLabel(tr("Create a new chat to get started with AI"), placeholder);
    placeholderText->setAlignment(Qt::AlignCenter);
    placeholderText->setStyleSheet("color: #9e9e9e; font-size: 15px; margin-top: 8px;");
    placeholderLayout->addWidget(placeholderText);

    auto* placeholderSub = new QLabel(tr("Powered by Ollama  •  qwen2.5:7b"), placeholder);
    placeholderSub->setAlignment(Qt::AlignCenter);
    placeholderSub->setStyleSheet("color: #c0c0c0; font-size: 12px; margin-top: 4px;");
    placeholderLayout->addWidget(placeholderSub);

    m_chatStack->addWidget(placeholder);

    splitter->addWidget(m_sessionList);
    splitter->addWidget(m_chatStack);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    setCentralWidget(splitter);
}

void MainWindow::setupMenuBar()
{
    auto* fileMenu = menuBar()->addMenu(tr("&File"));

    auto* settingsAction = fileMenu->addAction(tr("&Settings..."));
    settingsAction->setShortcut(QKeySequence("Ctrl+,"));
    connect(settingsAction, &QAction::triggered, this, &MainWindow::settingsRequested);

    fileMenu->addSeparator();

    auto* exitAction = fileMenu->addAction(tr("E&xit"));
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    auto* helpMenu = menuBar()->addMenu(tr("&Help"));

    auto* aboutAction = helpMenu->addAction(tr("&About Desktop Agent"));
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, tr("About Desktop Agent"),
            tr("<h3>Desktop Agent v0.1.0</h3>"
               "<p>A beautiful desktop AI assistant powered by Ollama.</p>"
               "<p>Built with Qt 6 and C++17.</p>"));
    });
}

void MainWindow::setupToolBar()
{
    auto* toolbar = addToolBar(tr("Main"));
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setObjectName("mainToolbar");

    auto* brandLabel = new QLabel(QString::fromUtf8("  \xF0\x9F\x90\xB4  Desktop Agent"), this);
    brandLabel->setStyleSheet(
        "color: white; font-size: 14px; font-weight: 700; background: transparent; border: none;");
    toolbar->addWidget(brandLabel);

    auto* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacer);

    auto* newBtn = new QPushButton(QString::fromUtf8("\xE2\x9E\x95"), this); // ➕
    newBtn->setFixedSize(34, 34);
    newBtn->setToolTip(tr("New Chat"));
    newBtn->setCursor(Qt::PointingHandCursor);
    newBtn->setStyleSheet(
        "QPushButton { background: rgba(255,255,255,0.12); color: white; border: none;"
        "  border-radius: 8px; font-size: 16px; }"
        "QPushButton:hover { background: rgba(255,255,255,0.25); }");
    connect(newBtn, &QPushButton::clicked, this, &MainWindow::newSessionRequested);
    toolbar->addWidget(newBtn);

    auto* settingsBtn = new QPushButton(QString::fromUtf8("\xE2\x9A\x99"), this); // ⚙
    settingsBtn->setFixedSize(34, 34);
    settingsBtn->setToolTip(tr("Settings"));
    settingsBtn->setCursor(Qt::PointingHandCursor);
    settingsBtn->setStyleSheet(
        "QPushButton { background: rgba(255,255,255,0.12); color: white; border: none;"
        "  border-radius: 8px; font-size: 16px; }"
        "QPushButton:hover { background: rgba(255,255,255,0.25); }");
    connect(settingsBtn, &QPushButton::clicked, this, &MainWindow::settingsRequested);
    toolbar->addWidget(settingsBtn);
}

ChatWidget* MainWindow::getOrCreateChatWidget(const QString& sessionId)
{
    if (auto* existing = m_chatWidgets.value(sessionId)) {
        return existing;
    }

    auto* chat = new ChatWidget(sessionId, this);
    connect(chat, &ChatWidget::userMessageSubmitted,
            this, &MainWindow::userMessageSubmitted);

    m_chatWidgets.insert(sessionId, chat);
    m_chatStack->addWidget(chat);
    return chat;
}

void MainWindow::displaySession(const QString& sessionId)
{
    m_activeSessionId = sessionId;
    auto* chat = getOrCreateChatWidget(sessionId);
    m_chatStack->setCurrentWidget(chat);
    m_sessionList->setCurrentSession(sessionId);
}

void MainWindow::onSessionCreated(const QString& sessionId, const QString& title)
{
    m_sessionList->addSession(sessionId, title);
    displaySession(sessionId);
}

void MainWindow::onSessionDeleted(const QString& sessionId)
{
    m_sessionList->removeSession(sessionId);
    if (auto* chat = m_chatWidgets.take(sessionId)) {
        m_chatStack->removeWidget(chat);
        chat->deleteLater();
    }
    if (m_activeSessionId == sessionId) {
        m_activeSessionId.clear();
        m_chatStack->setCurrentIndex(0);
    }
}

void MainWindow::onMessageAdded(const QString& sessionId, const Message& msg)
{
    if (auto* chat = m_chatWidgets.value(sessionId)) {
        chat->addMessage(msg);
    }
}

void MainWindow::onMessageContentUpdated(const QString& sessionId, const QString& messageId,
                                           const QString& fullContent)
{
    if (auto* chat = m_chatWidgets.value(sessionId)) {
        chat->updateMessageContent(messageId, fullContent);
    }
}

void MainWindow::onStreamingStarted(const QString& sessionId, const QString& messageId)
{
    if (auto* chat = m_chatWidgets.value(sessionId)) {
        chat->onStreamingStarted(messageId);
    }
}

void MainWindow::onStreamingFinished(const QString& sessionId, const QString& messageId)
{
    if (auto* chat = m_chatWidgets.value(sessionId)) {
        chat->onStreamingFinished(messageId);
    }
}

void MainWindow::onStreamingError(const QString& sessionId, const QString& messageId,
                                    const QString& error)
{
    if (auto* chat = m_chatWidgets.value(sessionId)) {
        chat->onStreamingFinished(messageId);
        Message errMsg = Message::create(Message::Assistant,
            tr("Error: %1").arg(error));
        errMsg.isStreaming = false;
        chat->addMessage(errMsg);
    }
}
