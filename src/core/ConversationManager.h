#pragma once

#include <QObject>
#include <QMap>
#include "Session.h"

class OllamaClient;
class SessionStore;

class ConversationManager : public QObject
{
    Q_OBJECT
public:
    explicit ConversationManager(OllamaClient* client, SessionStore* store, QObject* parent = nullptr);

    Session* session(const QString& sessionId);
    QList<Session*> allSessions() const;
    void loadSession(const Session& session);

    void setModelEndpoint(const QString& endpoint);
    void setModelName(const QString& model);
    void setModelTemperature(double temp);
    void setModelMaxTokens(int tokens);
    void setModelContextTokens(int tokens);

signals:
    void sessionCreated(const QString& sessionId, const QString& title);
    void sessionDeleted(const QString& sessionId);
    void messageAdded(const QString& sessionId, const Message& msg);
    void messageContentUpdated(const QString& sessionId, const QString& messageId,
                               const QString& fullContent);
    void streamingStarted(const QString& sessionId, const QString& messageId);
    void streamingFinished(const QString& sessionId, const QString& messageId);
    void streamingError(const QString& sessionId, const QString& messageId, const QString& error);

public slots:
    void createSession();
    void deleteSession(const QString& sessionId);
    void sendUserMessage(const QString& sessionId, const QString& text);
    void setSystemPrompt(const QString& sessionId, const QString& prompt);

private slots:
    void onStreamChunk(const QString& messageId, const QString& delta);
    void onStreamDone(const QString& messageId, const QString& fullContent);
    void onStreamError(const QString& messageId, const QString& error);

private:
    OllamaClient* m_client;
    SessionStore* m_store;
    QMap<QString, Session*> m_sessions;
    QMap<QString, QString> m_messageToSession;
    QString m_activeSessionId;

    QString m_modelEndpoint = "http://localhost:11434";
    QString m_modelName = "qwen2.5:7b";
    double m_modelTemperature = 0.7;
    int m_modelMaxTokens = 2048;
};
