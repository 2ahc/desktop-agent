#pragma once

#include <QJsonDocument>
#include <QJsonArray>
#include <QList>
#include "core/Message.h"

class JsonHelper {
public:
    static QJsonDocument readJsonFile(const QString& path);
    static bool writeJsonFile(const QString& path, const QJsonDocument& doc);
    static QJsonArray messagesToJsonArray(const QList<Message>& messages);
    static QList<Message> jsonArrayToMessages(const QJsonArray& arr);
};
