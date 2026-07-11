#include "MessageBubble.h"
#include "StreamingTextBrowser.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

MessageBubble::MessageBubble(const Message& msg, QWidget* parent)
    : QFrame(parent)
    , m_messageId(msg.id)
    , m_role(msg.role)
{
    setupUi();
    applyRoleStyle();

    if (msg.role == Message::User) {
        m_contentLabel->setText(msg.content);
        m_streamBrowser->hide();
    } else {
        m_contentLabel->hide();
        if (!msg.isStreaming) {
            m_streamBrowser->setMarkdown(msg.content);
        }
    }

    m_timeLabel->setText(msg.timestamp.toLocalTime().toString("hh:mm"));
}

void MessageBubble::setupUi()
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Avatar
    auto* avatarContainer = new QWidget(this);
    avatarContainer->setFixedSize(44, 44);
    avatarContainer->setStyleSheet(
        "background-color: #f0e6f6;"
        "border-radius: 22px;"
        "border: 2px solid #e8e0f0;");

    auto* avatarLayout = new QHBoxLayout(avatarContainer);
    avatarLayout->setContentsMargins(0, 0, 0, 0);
    avatarLayout->setAlignment(Qt::AlignCenter);

    auto* avatarLabel = new QLabel(avatarContainer);
    avatarLabel->setFixedSize(36, 36);
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setStyleSheet(
        "font-size: 22px;"
        "background: transparent;"
        "border: none;");

    if (m_role == Message::User) {
        avatarLabel->setText(QString::fromUtf8("\xF0\x9F\x91\xA4"));
    } else {
        avatarLabel->setText(QString::fromUtf8("\xF0\x9F\x90\xB4"));
    }
    avatarLayout->addWidget(avatarLabel);

    // Content column: bubble + time
    auto* contentColumn = new QWidget(this);
    auto* columnLayout = new QVBoxLayout(contentColumn);
    columnLayout->setContentsMargins(0, 0, 0, 0);
    columnLayout->setSpacing(4);

    // Streaming browser (for assistant)
    m_streamBrowser = new StreamingTextBrowser(contentColumn);
    m_streamBrowser->setStyleSheet(
        QString("background: transparent; border: none; font-size: 14px; line-height: 1.6; padding: 2px 0;")
        + (m_role == Message::User ? "color: #ffffff;" : "color: #333333;"));
    columnLayout->addWidget(m_streamBrowser);

    // Plain text label (for user)
    m_contentLabel = new QLabel(contentColumn);
    m_contentLabel->setWordWrap(true);
    m_contentLabel->setTextFormat(Qt::PlainText);
    m_contentLabel->setStyleSheet(
        "background: transparent; border: none; font-size: 14px; color: #ffffff; padding: 2px 0;");
    columnLayout->addWidget(m_contentLabel);

    // Time label
    m_timeLabel = new QLabel(contentColumn);
    m_timeLabel->setStyleSheet("font-size: 11px; color: #bdbdbd;");
    m_timeLabel->setAlignment(m_role == Message::User ? Qt::AlignRight : Qt::AlignLeft);
    columnLayout->addWidget(m_timeLabel);

    // Arrange: user right-aligned, assistant left-aligned
    if (m_role == Message::User) {
        mainLayout->addStretch(1);
        mainLayout->addWidget(contentColumn);
        mainLayout->addSpacing(10);
        mainLayout->addWidget(avatarContainer);
    } else {
        mainLayout->addWidget(avatarContainer);
        mainLayout->addSpacing(10);
        mainLayout->addWidget(contentColumn);
        mainLayout->addStretch(1);
    }

    contentColumn->setMaximumWidth(580);
}

void MessageBubble::applyRoleStyle()
{
    if (m_role == Message::User) {
        setStyleSheet(
            "MessageBubble {"
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "    stop:0 #667eea, stop:1 #764ba2);"
            "  border: none;"
            "  border-radius: 16px;"
            "  border-bottom-right-radius: 4px;"
            "  padding: 12px 16px;"
            "  margin: 8px 24px 8px 80px;"
            "}");
    } else {
        setStyleSheet(
            "MessageBubble {"
            "  background-color: #ffffff;"
            "  border: 1px solid #e8e0f0;"
            "  border-radius: 16px;"
            "  border-bottom-left-radius: 4px;"
            "  padding: 12px 16px;"
            "  margin: 8px 80px 8px 24px;"
            "}");

        auto* shadow = new QGraphicsDropShadowEffect(this);
        shadow->setBlurRadius(12);
        shadow->setOffset(0, 1);
        shadow->setColor(QColor(0, 0, 0, 20));
        setGraphicsEffect(shadow);
    }
}

void MessageBubble::setContent(const QString& text)
{
    m_streamBrowser->setMarkdown(text);
    m_streamBrowser->show();
    m_contentLabel->hide();
}

void MessageBubble::appendDelta(const QString& delta)
{
    if (m_role == Message::Assistant) {
        setStyleSheet(
            "MessageBubble {"
            "  background-color: #ffffff;"
            "  border: 1.5px solid #b39ddb;"
            "  border-radius: 16px;"
            "  border-bottom-left-radius: 4px;"
            "  padding: 12px 16px;"
            "  margin: 8px 80px 8px 24px;"
            "}");
    }
    m_streamBrowser->appendDelta(delta);
}

void MessageBubble::finishStreaming()
{
    m_streamBrowser->finishStreaming();
    applyRoleStyle();
}

void MessageBubble::setError(const QString& errorText)
{
    setStyleSheet(
        "MessageBubble {"
        "  background-color: #fff5f5;"
        "  border: 1px solid #fecaca;"
        "  border-radius: 16px;"
        "  padding: 12px 16px;"
        "  margin: 8px 80px 8px 24px;"
        "}");
    m_streamBrowser->setStyleSheet(
        "background: transparent; border: none; font-size: 14px; color: #c62828; padding: 2px 0;");
    m_streamBrowser->setMarkdown(errorText);
    m_streamBrowser->show();
    m_contentLabel->hide();
}
