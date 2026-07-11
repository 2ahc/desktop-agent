#include "MessageListWidget.h"
#include "MessageBubble.h"
#include "core/Message.h"

#include <QVBoxLayout>
#include <QScrollBar>

MessageListWidget::MessageListWidget(QWidget* parent)
    : QScrollArea(parent)
{
    setObjectName("messageScrollArea");
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_container = new QWidget(this);
    m_layout = new QVBoxLayout(m_container);
    m_layout->setContentsMargins(0, 12, 0, 12);
    m_layout->setSpacing(4);
    m_layout->addStretch(1);

    setWidget(m_container);
}

MessageBubble* MessageListWidget::addMessageBubble(const Message& msg)
{
    auto* bubble = new MessageBubble(msg, m_container);
    // Insert before the trailing stretch
    m_layout->insertWidget(m_layout->count() - 1, bubble);
    m_bubbles.insert(msg.id, bubble);
    scrollToBottom();
    return bubble;
}

MessageBubble* MessageListWidget::findBubble(const QString& messageId)
{
    return m_bubbles.value(messageId, nullptr);
}

void MessageListWidget::removeBubble(const QString& messageId)
{
    if (auto* bubble = m_bubbles.take(messageId)) {
        m_layout->removeWidget(bubble);
        bubble->deleteLater();
    }
}

void MessageListWidget::updateBubbleContent(const QString& messageId, const QString& fullContent)
{
    if (auto* bubble = m_bubbles.value(messageId)) {
        bubble->setContent(fullContent);
    }
}

void MessageListWidget::scrollToBottom()
{
    auto* sb = verticalScrollBar();
    QMetaObject::invokeMethod(this, [sb]() {
        sb->setValue(sb->maximum());
    }, Qt::QueuedConnection);
}
