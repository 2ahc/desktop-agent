#pragma once

#include <QObject>
#include <QByteArray>

class StreamParser : public QObject
{
    Q_OBJECT
public:
    explicit StreamParser(QObject* parent = nullptr);

    void feed(const QByteArray& chunk);
    void reset();

signals:
    void tokenReceived(const QString& delta);
    void streamComplete(const QString& accumulatedContent);
    void parseError(const QString& error);

private:
    QByteArray m_buffer;
    QString m_accumulatedContent;

    void processLine(const QByteArray& line);
};
