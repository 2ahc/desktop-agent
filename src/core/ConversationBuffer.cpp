#include "ConversationBuffer.h"
#include "TokenEstimator.h"

ConversationBuffer::ConversationBuffer(int maxMessages, int maxTokens)
    : m_maxMessages(maxMessages)
    , m_maxTokens(maxTokens)
{
}

void ConversationBuffer::append(const Message& msg)
{
    m_messages.append(msg);
    m_totalTokens += msg.tokenCount;
    enforceLimits();
}

void ConversationBuffer::setSystemPrompt(const QString& prompt)
{
    m_systemPrompt = Message::create(Message::System, prompt);
}

void ConversationBuffer::enforceLimits()
{
    while ((m_messages.size() > m_maxMessages || m_totalTokens > m_maxTokens)
           && m_messages.size() > 1)
    {
        int removeIdx = -1;
        for (int i = 0; i < m_messages.size(); ++i) {
            if (m_messages[i].role != Message::System && !m_messages[i].isStreaming) {
                removeIdx = i;
                break;
            }
        }
        if (removeIdx < 0) break;

        m_totalTokens -= m_messages[removeIdx].tokenCount;
        m_messages.removeAt(removeIdx);
    }
}

QList<Message> ConversationBuffer::getContext() const
{
    QList<Message> context;
    if (!m_systemPrompt.content.isEmpty()) {
        context.append(m_systemPrompt);
    }
    for (const auto& msg : m_messages) {
        if (msg.role != Message::System) {
            context.append(msg);
        }
    }
    return context;
}

void ConversationBuffer::clear()
{
    m_messages.clear();
    m_totalTokens = 0;
}

void ConversationBuffer::setMaxMessages(int n) { m_maxMessages = n; enforceLimits(); }
void ConversationBuffer::setMaxTokens(int n)   { m_maxTokens = n; enforceLimits(); }

QJsonArray ConversationBuffer::toJsonArray() const
{
    QJsonArray arr;
    for (const auto& msg : m_messages) {
        arr.append(msg.toJson());
    }
    return arr;
}

void ConversationBuffer::fromJsonArray(const QJsonArray& arr)
{
    m_messages.clear();
    m_totalTokens = 0;
    for (const auto& val : arr) {
        Message msg = Message::fromJson(val.toObject());
        m_messages.append(msg);
        m_totalTokens += msg.tokenCount;
    }
}
