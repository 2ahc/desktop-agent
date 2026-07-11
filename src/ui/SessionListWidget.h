#pragma once

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QMap>

class SessionListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SessionListWidget(QWidget* parent = nullptr);

    void updateSessionPreview(const QString& sessionId, const QString& preview);

signals:
    void sessionClicked(const QString& sessionId);
    void newSessionClicked();
    void deleteSessionClicked(const QString& sessionId);

public slots:
    void addSession(const QString& sessionId, const QString& title);
    void removeSession(const QString& sessionId);
    void setCurrentSession(const QString& sessionId);

private slots:
    void filterSessions(const QString& text);

private:
    void setupUi();
    QListWidget* m_listWidget = nullptr;
    QLineEdit* m_searchEdit = nullptr;
    QMap<QString, QString> m_sessionTitles;
    QMap<QString, QString> m_sessionPreviews;
};
