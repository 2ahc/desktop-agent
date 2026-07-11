#include "ConversationManager.h"
#include "SessionStore.h"
#include "TokenEstimator.h"
#include "network/OllamaClient.h"

#include <QUuid>

ConversationManager::ConversationManager(OllamaClient* client, SessionStore* store,
                                         QObject* parent)
    : QObject(parent)
    , m_client(client)
    , m_store(store)
{
    connect(m_client, &OllamaClient::streamChunk,
            this, &ConversationManager::onStreamChunk);
    connect(m_client, &OllamaClient::streamFinished,
            this, &ConversationManager::onStreamDone);
    connect(m_client, &OllamaClient::streamError,
            this, &ConversationManager::onStreamError);
}

Session* ConversationManager::session(const QString& sessionId)
{
    return m_sessions.value(sessionId, nullptr);
}

QList<Session*> ConversationManager::allSessions() const
{
    return m_sessions.values();
}

void ConversationManager::createSession()
{
    auto* s = new Session;
    s->id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    s->title = tr("New Chat");
    s->createdAt = QDateTime::currentDateTimeUtc();
    s->updatedAt = s->createdAt;
    m_sessions.insert(s->id, s);
    m_activeSessionId = s->id;
    emit sessionCreated(s->id, s->title);
}

void ConversationManager::deleteSession(const QString& sessionId)
{
    if (auto* s = m_sessions.take(sessionId)) {
        m_store->remove(sessionId);
        delete s;
        emit sessionDeleted(sessionId);
    }
}

void ConversationManager::loadSession(const Session& session)
{
    auto* s = new Session(session);
    m_sessions.insert(s->id, s);
    emit sessionCreated(s->id, s->title);
}

void ConversationManager::sendUserMessage(const QString& sessionId, const QString& text)
{
    auto* s = m_sessions.value(sessionId);
    if (!s) return;

    Message userMsg = Message::create(Message::User, text);
    s->buffer.append(userMsg);
    s->updatedAt = QDateTime::currentDateTimeUtc();

    if (s->title == "New Chat" || s->title.isEmpty()) {
        s->title = text.left(60);
    }

    emit messageAdded(sessionId, userMsg);

    Message assistantMsg = Message::create(Message::Assistant, QString());
    assistantMsg.isStreaming = true;
    s->buffer.append(assistantMsg);
    m_messageToSession.insert(assistantMsg.id, sessionId);

    emit messageAdded(sessionId, assistantMsg);
    emit streamingStarted(sessionId, assistantMsg.id);

    m_store->save(*s);

    m_client->sendChat(assistantMsg.id,
                       s->buffer.getContext(),
                       m_modelName,
                       m_modelEndpoint,
                       m_modelTemperature,
                       m_modelMaxTokens);
}

void ConversationManager::setModelEndpoint(const QString& endpoint)
{
    m_modelEndpoint = endpoint;
}

void ConversationManager::setModelName(const QString& model)
{
    m_modelName = model;
}

void ConversationManager::setModelTemperature(double temp)
{
    m_modelTemperature = temp;
}

void ConversationManager::setModelMaxTokens(int tokens)
{
    m_modelMaxTokens = tokens;
}

void ConversationManager::setModelContextTokens(int tokens)
{
    for (auto* s : m_sessions) {
        s->buffer.setMaxTokens(tokens);
    }
}

void ConversationManager::setSystemPrompt(const QString& sessionId, const QString& prompt)
{
    if (auto* s = m_sessions.value(sessionId)) {
        s->buffer.setSystemPrompt(prompt);
    }
}

void ConversationManager::onStreamChunk(const QString& messageId, const QString& delta)
{
    QString sessionId = m_messageToSession.value(messageId);
    auto* s = m_sessions.value(sessionId);
    if (!s) return;

    auto& msgs = const_cast<QVector<Message>&>(s->buffer.messages());
    for (auto& msg : msgs) {
        if (msg.id == messageId) {
            msg.content += delta;
            msg.tokenCount = TokenEstimator::estimate(msg.content);
            emit messageContentUpdated(sessionId, messageId, msg.content);
            break;
        }
    }
}

void ConversationManager::onStreamDone(const QString& messageId, const QString& fullContent)
{
    QString sessionId = m_messageToSession.take(messageId);
    auto* s = m_sessions.value(sessionId);
    if (!s) return;

    auto& msgs = const_cast<QVector<Message>&>(s->buffer.messages());
    for (auto& msg : msgs) {
        if (msg.id == messageId) {
            msg.content = fullContent;
            msg.isStreaming = false;
            msg.tokenCount = TokenEstimator::estimate(fullContent);
            break;
        }
    }

    s->updatedAt = QDateTime::currentDateTimeUtc();
    m_store->save(*s);
    emit streamingFinished(sessionId, messageId);
}

void ConversationManager::onStreamError(const QString& messageId, const QString& error)
{
    QString sessionId = m_messageToSession.take(messageId);
    if (sessionId.isEmpty()) return;

    auto* s = m_sessions.value(sessionId);
    if (!s) return;

    auto& msgs = const_cast<QVector<Message>&>(s->buffer.messages());
    for (auto& msg : msgs) {
        if (msg.id == messageId) {
            msg.content = error;
            msg.isStreaming = false;
            break;
        }
    }

    emit streamingError(sessionId, messageId, error);
}
