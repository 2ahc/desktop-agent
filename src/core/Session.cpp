#include "Session.h"
#include <QUuid>

QJsonObject Session::toJson() const
{
    QJsonObject obj;
    obj["id"] = id;
    obj["title"] = title;
    obj["createdAt"] = createdAt.toString(Qt::ISODate);
    obj["updatedAt"] = updatedAt.toString(Qt::ISODate);
    obj["messages"] = buffer.toJsonArray();
    return obj;
}

Session Session::fromJson(const QJsonObject& obj)
{
    Session s;
    s.id = obj["id"].toString();
    s.title = obj["title"].toString();
    s.createdAt = QDateTime::fromString(obj["createdAt"].toString(), Qt::ISODate);
    s.updatedAt = QDateTime::fromString(obj["updatedAt"].toString(), Qt::ISODate);
    s.buffer.fromJsonArray(obj["messages"].toArray());
    return s;
}
