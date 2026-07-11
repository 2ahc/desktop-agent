#include "SessionListWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QLineEdit>
#include <QLabel>

SessionListWidget::SessionListWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void SessionListWidget::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Header: brand + icon buttons
    auto* header = new QWidget(this);
    header->setObjectName("sessionListHeader");
    header->setFixedHeight(56);
    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(14, 0, 8, 0);
    headerLayout->setSpacing(4);

    auto* brand = new QLabel(QString::fromUtf8("  \xF0\x9F\x90\xB4  Desktop Agent"), header);
    brand->setStyleSheet(
        "font-size: 14px; font-weight: 700; color: #5e35b1; background: transparent; border: none;");
    headerLayout->addWidget(brand);
    headerLayout->addStretch(1);

    auto* newBtn = new QPushButton(QString::fromUtf8("\xE2\x9E\x95"), header); // ➕
    newBtn->setFixedSize(32, 32);
    newBtn->setToolTip(tr("Start a new conversation"));
    newBtn->setCursor(Qt::PointingHandCursor);
    newBtn->setStyleSheet(
        "QPushButton { background: transparent; border: none; font-size: 16px; color: #666; border-radius: 8px; }"
        "QPushButton:hover { background: #f0e6f6; color: #5e35b1; }");
    connect(newBtn, &QPushButton::clicked, this, &SessionListWidget::newSessionClicked);
    headerLayout->addWidget(newBtn);

    auto* delBtn = new QPushButton(QString::fromUtf8("\xF0\x9F\x97\x91"), header); // 🗑
    delBtn->setFixedSize(32, 32);
    delBtn->setToolTip(tr("Delete selected conversation"));
    delBtn->setCursor(Qt::PointingHandCursor);
    delBtn->setStyleSheet(
        "QPushButton { background: transparent; border: none; font-size: 14px; color: #999; border-radius: 8px; }"
        "QPushButton:hover { background: #fff0f0; color: #e53935; }");
    connect(delBtn, &QPushButton::clicked, this, [this]() {
        auto* item = m_listWidget->currentItem();
        if (item) {
            emit deleteSessionClicked(item->data(Qt::UserRole).toString());
        }
    });
    headerLayout->addWidget(delBtn);

    layout->addWidget(header);

    // Separator
    auto* sep = new QWidget(this);
    sep->setFixedHeight(1);
    sep->setStyleSheet("background-color: #e8e8e8;");
    layout->addWidget(sep);

    // Search box
    auto* searchContainer = new QWidget(this);
    searchContainer->setObjectName("searchContainer");
    auto* searchLayout = new QHBoxLayout(searchContainer);
    searchLayout->setContentsMargins(12, 8, 12, 8);

    m_searchEdit = new QLineEdit(searchContainer);
    m_searchEdit->setObjectName("sessionSearch");
    m_searchEdit->setPlaceholderText(tr("Search conversations..."));
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setStyleSheet(
        "QLineEdit { background: #f5f6fa; border: 1px solid #e8e8e8; border-radius: 8px;"
        "  padding: 8px 12px; font-size: 12px; color: #333; }"
        "QLineEdit:focus { border-color: #b39ddb; background: #ffffff; }");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &SessionListWidget::filterSessions);
    searchLayout->addWidget(m_searchEdit);

    layout->addWidget(searchContainer);

    // Session list
    m_listWidget = new QListWidget(this);
    m_listWidget->setObjectName("sessionListWidget");
    m_listWidget->setFrameShape(QFrame::NoFrame);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setSpacing(2);

    connect(m_listWidget, &QListWidget::currentItemChanged, this,
            [this](QListWidgetItem* current, QListWidgetItem*) {
        if (current) {
            emit sessionClicked(current->data(Qt::UserRole).toString());
        }
    });

    layout->addWidget(m_listWidget);
}

void SessionListWidget::addSession(const QString& sessionId, const QString& title)
{
    auto* item = new QListWidgetItem(title);
    item->setData(Qt::UserRole, sessionId);
    item->setData(Qt::UserRole + 1, ""); // preview text placeholder
    item->setToolTip(title);
    m_listWidget->insertItem(0, item);
    m_listWidget->setCurrentItem(item);
    m_sessionTitles.insert(sessionId, title);
}

void SessionListWidget::updateSessionPreview(const QString& sessionId, const QString& preview)
{
    m_sessionPreviews.insert(sessionId, preview);
    for (int i = 0; i < m_listWidget->count(); ++i) {
        auto* item = m_listWidget->item(i);
        if (item->data(Qt::UserRole).toString() == sessionId) {
            item->setData(Qt::UserRole + 1, preview);
            break;
        }
    }
}

void SessionListWidget::removeSession(const QString& sessionId)
{
    for (int i = 0; i < m_listWidget->count(); ++i) {
        if (m_listWidget->item(i)->data(Qt::UserRole).toString() == sessionId) {
            delete m_listWidget->takeItem(i);
            break;
        }
    }
    m_sessionTitles.remove(sessionId);
    m_sessionPreviews.remove(sessionId);
}

void SessionListWidget::setCurrentSession(const QString& sessionId)
{
    for (int i = 0; i < m_listWidget->count(); ++i) {
        if (m_listWidget->item(i)->data(Qt::UserRole).toString() == sessionId) {
            m_listWidget->setCurrentRow(i);
            break;
        }
    }
}

void SessionListWidget::filterSessions(const QString& text)
{
    for (int i = 0; i < m_listWidget->count(); ++i) {
        auto* item = m_listWidget->item(i);
        QString title = m_sessionTitles.value(item->data(Qt::UserRole).toString());
        QString preview = m_sessionPreviews.value(item->data(Qt::UserRole).toString());
        bool match = text.isEmpty()
            || title.contains(text, Qt::CaseInsensitive)
            || preview.contains(text, Qt::CaseInsensitive);
        item->setHidden(!match);
    }
}
