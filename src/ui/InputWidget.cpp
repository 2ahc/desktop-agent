#include "InputWidget.h"

#include <QPlainTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QTextDocument>

InputWidget::InputWidget(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("inputWidget");
    setupUi();
}

void InputWidget::setupUi()
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(20, 14, 20, 16);
    layout->setSpacing(10);

    m_textEdit = new QPlainTextEdit(this);
    m_textEdit->setObjectName("inputTextEdit");
    m_textEdit->setPlaceholderText(tr("Type a message... Enter to send, Shift+Enter for new line"));
    m_textEdit->setMaximumHeight(120);
    m_textEdit->setMinimumHeight(44);
    m_textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(m_textEdit, 1);

    m_sendButton = new QPushButton(this);
    m_sendButton->setObjectName("sendButton");
    m_sendButton->setText(QString::fromUtf8("\xE2\x9C\x88")); // paper plane ✈
    m_sendButton->setFixedSize(44, 44);
    m_sendButton->setEnabled(false);
    m_sendButton->setCursor(Qt::PointingHandCursor);
    m_sendButton->setToolTip(tr("Send"));
    layout->addWidget(m_sendButton);

    connect(m_textEdit, &QPlainTextEdit::textChanged, this, &InputWidget::updateSendButton);
    connect(m_sendButton, &QPushButton::clicked, this, &InputWidget::sendMessage);

    // Auto-resize on content change
    connect(m_textEdit->document(), &QTextDocument::contentsChanged, this, [this]() {
        int docHeight = (int)m_textEdit->document()->size().height();
        int newHeight = qBound(44, docHeight + 12, 120);
        m_textEdit->setMinimumHeight(newHeight);
    });

    m_textEdit->installEventFilter(this);
}

void InputWidget::updateSendButton()
{
    bool hasText = !m_textEdit->toPlainText().trimmed().isEmpty();
    m_sendButton->setEnabled(hasText && m_textEdit->isEnabled());
}

void InputWidget::setEnabled(bool enabled)
{
    m_textEdit->setEnabled(enabled);
    m_sendButton->setEnabled(enabled && !m_textEdit->toPlainText().trimmed().isEmpty());
    if (enabled) {
        m_textEdit->setFocus();
    }
}

void InputWidget::clear()
{
    m_textEdit->clear();
}

void InputWidget::sendMessage()
{
    QString text = m_textEdit->toPlainText().trimmed();
    if (!text.isEmpty()) {
        emit sendClicked(text);
        m_textEdit->clear();
    }
}

bool InputWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_textEdit && event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            if (keyEvent->modifiers() & Qt::ShiftModifier) {
                return false;
            }
            sendMessage();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
