#include "ChatWidget.h"
#include "MessageListWidget.h"
#include "InputWidget.h"
#include "core/Message.h"

#include <QVBoxLayout>

ChatWidget::ChatWidget(const QString& sessionId, QWidget* parent)
    : QWidget(parent)
    , m_sessionId(sessionId)
{
    setupUi();
}

void ChatWidget::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_messageList = new MessageListWidget(this);
    layout->addWidget(m_messageList, 1);

    m_input = new InputWidget(this);
    layout->addWidget(m_input);

    connect(m_input, &InputWidget::sendClicked, this, [this](const QString& text) {
        emit userMessageSubmitted(m_sessionId, text);
    });
}

void ChatWidget::addMessage(const Message& msg)
{
    m_messageList->addMessageBubble(msg);
}

void ChatWidget::updateMessageContent(const QString& messageId, const QString& fullContent)
{
    m_messageList->updateBubbleContent(messageId, fullContent);
}

void ChatWidget::onStreamingStarted(const QString& messageId)
{
    Q_UNUSED(messageId)
    m_input->setEnabled(false);
}

void ChatWidget::onStreamingFinished(const QString& messageId)
{
    Q_UNUSED(messageId)
    m_input->setEnabled(true);
    m_input->clear();
}

void ChatWidget::setInputEnabled(bool enabled)
{
    m_input->setEnabled(enabled);
}
