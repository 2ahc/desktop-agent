#pragma once

#include <QObject>
#include <QList>
#include "Session.h"

class SessionStore : public QObject
{
    Q_OBJECT
public:
    explicit SessionStore(const QString& dataDir, QObject* parent = nullptr);

    QList<Session> loadAll();
    bool save(const Session& session);
    bool remove(const QString& sessionId);
    QStringList listSessionIds() const;

signals:
    void saveError(const QString& sessionId, const QString& error);

private:
    QString m_dataDir;
    QString filePath(const QString& sessionId) const;
};
