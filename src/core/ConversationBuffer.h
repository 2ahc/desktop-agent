#pragma once

#include "Message.h"
#include <QVector>
#include <QList>
#include <QJsonArray>

class ConversationBuffer {
public:
    static constexpr int DEFAULT_MAX_MESSAGES = 100;
    static constexpr int DEFAULT_MAX_TOKENS  = 4096;

    explicit ConversationBuffer(int maxMessages = DEFAULT_MAX_MESSAGES,
                                 int maxTokens   = DEFAULT_MAX_TOKENS);

    void append(const Message& msg);
    void setSystemPrompt(const QString& prompt);
    QList<Message> getContext() const;
    int totalTokens() const { return m_totalTokens; }
    int messageCount() const { return m_messages.size(); }
    bool isEmpty() const { return m_messages.isEmpty(); }
    void clear();
    const QVector<Message>& messages() const { return m_messages; }

    void setMaxMessages(int n);
    void setMaxTokens(int n);

    QJsonArray toJsonArray() const;
    void fromJsonArray(const QJsonArray& arr);

private:
    QVector<Message> m_messages;
    int m_totalTokens = 0;
    int m_maxMessages;
    int m_maxTokens;
    Message m_systemPrompt;

    void enforceLimits();
};
