#pragma once

#include <QFrame>
#include "core/Message.h"

class StreamingTextBrowser;
class QLabel;

class MessageBubble : public QFrame
{
    Q_OBJECT
public:
    explicit MessageBubble(const Message& msg, QWidget* parent = nullptr);

    QString messageId() const { return m_messageId; }
    Message::Role role() const { return m_role; }

    void setContent(const QString& text);
    void appendDelta(const QString& delta);
    void finishStreaming();
    void setError(const QString& errorText);

private:
    void setupUi();
    void applyRoleStyle();

    QString m_messageId;
    Message::Role m_role;
    QLabel* m_timeLabel = nullptr;
    StreamingTextBrowser* m_streamBrowser = nullptr;
    QLabel* m_contentLabel = nullptr;  // for user messages (non-streaming)
};
