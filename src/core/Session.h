#pragma once

#include "ConversationBuffer.h"
#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Session {
public:
    QString          id;
    QString          title;
    QDateTime        createdAt;
    QDateTime        updatedAt;
    ConversationBuffer buffer;

    QJsonObject toJson() const;
    static Session fromJson(const QJsonObject& obj);
};
