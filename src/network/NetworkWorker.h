#pragma once

#include <QObject>
#include <QMap>

class QNetworkAccessManager;
class QNetworkReply;
class StreamParser;

class NetworkWorker : public QObject
{
    Q_OBJECT
public:
    explicit NetworkWorker(QObject* parent = nullptr);
    ~NetworkWorker();

public slots:
    void postChat(const QString& endpoint, const QJsonObject& requestBody,
                  const QString& correlationId);
    void cancelRequest(const QString& correlationId);

signals:
    void tokenReceived(const QString& correlationId, const QString& delta);
    void requestComplete(const QString& correlationId, const QString& fullContent);
    void requestError(const QString& correlationId, const QString& error);

private:
    void init();

    QNetworkAccessManager* m_nam = nullptr;
    QMap<QString, QNetworkReply*> m_activeReplies;
    QMap<QString, StreamParser*> m_activeParsers;
};
