#pragma once

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QUuid>

struct Message {
    enum Role { System, User, Assistant };

    QString   id;
    Role      role = User;
    QString   content;
    QDateTime timestamp;
    bool      isStreaming = false;
    int       tokenCount = 0;

    static Message create(Role r, const QString& text);

    QJsonObject toJson() const;
    static Message fromJson(const QJsonObject& obj);
};
