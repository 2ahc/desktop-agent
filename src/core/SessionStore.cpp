#include "SessionStore.h"
#include "utils/JsonHelper.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

SessionStore::SessionStore(const QString& dataDir, QObject* parent)
    : QObject(parent)
    , m_dataDir(dataDir)
{
    QDir().mkpath(m_dataDir);
}

QString SessionStore::filePath(const QString& sessionId) const
{
    return m_dataDir + "/" + sessionId + ".json";
}

QList<Session> SessionStore::loadAll()
{
    QList<Session> sessions;
    QDir dir(m_dataDir);
    for (const auto& fi : dir.entryInfoList({"*.json"}, QDir::Files)) {
        QJsonDocument doc = JsonHelper::readJsonFile(fi.absoluteFilePath());
        if (!doc.isNull()) {
            sessions.append(Session::fromJson(doc.object()));
        }
    }
    return sessions;
}

bool SessionStore::save(const Session& session)
{
    QJsonDocument doc(session.toJson());
    bool ok = JsonHelper::writeJsonFile(filePath(session.id), doc);
    if (!ok) {
        emit saveError(session.id, "Failed to write session file");
    }
    return ok;
}

bool SessionStore::remove(const QString& sessionId)
{
    return QFile::remove(filePath(sessionId));
}

QStringList SessionStore::listSessionIds() const
{
    QStringList ids;
    QDir dir(m_dataDir);
    for (const auto& fi : dir.entryInfoList({"*.json"}, QDir::Files)) {
        ids.append(fi.completeBaseName());
    }
    return ids;
}
