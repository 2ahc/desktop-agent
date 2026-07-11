#pragma once

#include <QTextBrowser>

class QTimer;

class StreamingTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit StreamingTextBrowser(QWidget* parent = nullptr);

    void appendDelta(const QString& delta);
    void finishStreaming();
    void setMarkdown(const QString& markdown);

private slots:
    void flushPending();

private:
    QString markdownToHtml(const QString& md) const;

    QTimer* m_flushTimer = nullptr;
    QString m_pendingText;
    QString m_accumulatedText;
};
