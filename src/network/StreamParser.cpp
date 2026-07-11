#include "StreamParser.h"

#include <QJsonDocument>
#include <QJsonObject>

StreamParser::StreamParser(QObject* parent)
    : QObject(parent)
{
}

void StreamParser::feed(const QByteArray& chunk)
{
    m_buffer.append(chunk);

    while (true) {
        int idx = m_buffer.indexOf('\n');
        if (idx < 0) break;

        QByteArray line = m_buffer.left(idx).trimmed();
        m_buffer.remove(0, idx + 1);

        if (!line.isEmpty()) {
            processLine(line);
        }
    }
}

void StreamParser::processLine(const QByteArray& line)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(line, &err);

    if (err.error != QJsonParseError::NoError) {
        emit parseError(QString("JSON parse error: %1").arg(err.errorString()));
        return;
    }

    QJsonObject obj = doc.object();

    if (obj.value("done").toBool(false)) {
        emit streamComplete(m_accumulatedContent);
        return;
    }

    QJsonObject message = obj.value("message").toObject();
    QString content = message.value("content").toString();

    if (!content.isEmpty()) {
        m_accumulatedContent += content;
        emit tokenReceived(content);
    }
}

void StreamParser::reset()
{
    m_buffer.clear();
    m_accumulatedContent.clear();
}
