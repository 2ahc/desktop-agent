#include "Message.h"
#include "TokenEstimator.h"

Message Message::create(Role r, const QString& text)
{
    Message msg;
    msg.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    msg.role = r;
    msg.content = text;
    msg.timestamp = QDateTime::currentDateTimeUtc();
    msg.tokenCount = TokenEstimator::estimate(text);
    return msg;
}

QJsonObject Message::toJson() const
{
    QJsonObject obj;
    obj["id"] = id;
    obj["role"] = (role == System) ? "system" : (role == User) ? "user" : "assistant";
    obj["content"] = content;
    obj["timestamp"] = timestamp.toString(Qt::ISODate);
    obj["isStreaming"] = isStreaming;
    obj["tokenCount"] = tokenCount;
    return obj;
}

Message Message::fromJson(const QJsonObject& obj)
{
    Message msg;
    msg.id = obj["id"].toString();
    QString r = obj["role"].toString();
    if (r == "system") msg.role = System;
    else if (r == "assistant") msg.role = Assistant;
    else msg.role = User;
    msg.content = obj["content"].toString();
    msg.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
    msg.isStreaming = obj["isStreaming"].toBool();
    msg.tokenCount = obj["tokenCount"].toInt();
    return msg;
}
