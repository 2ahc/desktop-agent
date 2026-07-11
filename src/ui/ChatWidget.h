#pragma once

#include <QWidget>
#include "core/Message.h"

class MessageListWidget;
class InputWidget;

class ChatWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChatWidget(const QString& sessionId, QWidget* parent = nullptr);

    QString sessionId() const { return m_sessionId; }

public slots:
    void addMessage(const Message& msg);
    void updateMessageContent(const QString& messageId, const QString& fullContent);
    void onStreamingStarted(const QString& messageId);
    void onStreamingFinished(const QString& messageId);
    void setInputEnabled(bool enabled);

signals:
    void userMessageSubmitted(const QString& sessionId, const QString& text);

private:
    void setupUi();
    QString m_sessionId;
    MessageListWidget* m_messageList = nullptr;
    InputWidget* m_input = nullptr;
};
