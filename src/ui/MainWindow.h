#pragma once

#include <QMainWindow>
#include <QMap>
#include "core/Message.h"

class QStackedWidget;
class SessionListWidget;
class ChatWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

signals:
    void newSessionRequested();
    void deleteSessionRequested(const QString& sessionId);
    void settingsRequested();
    void userMessageSubmitted(const QString& sessionId, const QString& text);

public slots:
    void displaySession(const QString& sessionId);
    void onSessionCreated(const QString& sessionId, const QString& title);
    void onSessionDeleted(const QString& sessionId);
    void onMessageAdded(const QString& sessionId, const Message& msg);
    void onMessageContentUpdated(const QString& sessionId, const QString& messageId,
                                  const QString& fullContent);
    void onStreamingStarted(const QString& sessionId, const QString& messageId);
    void onStreamingFinished(const QString& sessionId, const QString& messageId);
    void onStreamingError(const QString& sessionId, const QString& messageId,
                           const QString& error);

private:
    void loadStyleSheet();
    void setupUi();
    void setupMenuBar();
    void setupToolBar();

    ChatWidget* getOrCreateChatWidget(const QString& sessionId);

    SessionListWidget* m_sessionList = nullptr;
    QStackedWidget* m_chatStack = nullptr;
    QMap<QString, ChatWidget*> m_chatWidgets;
    QString m_activeSessionId;
};
