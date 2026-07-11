#pragma once

#include <QScrollArea>
#include <QMap>
#include <QVBoxLayout>
#include "core/Message.h"

class MessageBubble;

class MessageListWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit MessageListWidget(QWidget* parent = nullptr);

    MessageBubble* addMessageBubble(const Message& msg);
    MessageBubble* findBubble(const QString& messageId);
    void removeBubble(const QString& messageId);
    void updateBubbleContent(const QString& messageId, const QString& fullContent);
    void scrollToBottom();

private:
    QWidget* m_container = nullptr;
    QVBoxLayout* m_layout = nullptr;
    QMap<QString, MessageBubble*> m_bubbles;
};
