#include "JsonHelper.h"

#include <QFile>
#include <QTemporaryFile>
#include <QDir>
#include <QFileInfo>

QJsonDocument JsonHelper::readJsonFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return QJsonDocument();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    file.close();

    if (err.error != QJsonParseError::NoError)
        return QJsonDocument();

    return doc;
}

bool JsonHelper::writeJsonFile(const QString& path, const QJsonDocument& doc)
{
    QFileInfo fi(path);
    QDir().mkpath(fi.absolutePath());

    QTemporaryFile temp(path);
    temp.setAutoRemove(false);

    if (!temp.open()) return false;
    temp.write(doc.toJson(QJsonDocument::Indented));
    temp.close();

    if (QFile::exists(path))
        QFile::remove(path);

    return temp.rename(path);
}

QJsonArray JsonHelper::messagesToJsonArray(const QList<Message>& messages)
{
    QJsonArray arr;
    for (const auto& msg : messages) {
        arr.append(msg.toJson());
    }
    return arr;
}

QList<Message> JsonHelper::jsonArrayToMessages(const QJsonArray& arr)
{
    QList<Message> messages;
    for (const auto& val : arr) {
        messages.append(Message::fromJson(val.toObject()));
    }
    return messages;
}
