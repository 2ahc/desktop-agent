#pragma once

#include <QObject>
#include <QList>
#include "core/Message.h"

class QThread;
class NetworkWorker;

class OllamaClient : public QObject
{
    Q_OBJECT
public:
    explicit OllamaClient(QObject* parent = nullptr);
    ~OllamaClient();

public slots:
    void sendChat(const QString& messageId,
                  const QList<Message>& conversationHistory,
                  const QString& model,
                  const QString& endpoint,
                  double temperature,
                  int maxTokens);
    void cancelChat(const QString& messageId);

signals:
    void streamChunk(const QString& messageId, const QString& delta);
    void streamFinished(const QString& messageId, const QString& fullContent);
    void streamError(const QString& messageId, const QString& error);

private slots:
    void onWorkerToken(const QString& correlationId, const QString& delta);
    void onWorkerComplete(const QString& correlationId, const QString& fullContent);
    void onWorkerError(const QString& correlationId, const QString& error);

signals:
    void _internalSendRequest(const QString& endpoint, const QJsonObject& body,
                              const QString& correlationId);
    void _internalCancelRequest(const QString& correlationId);

private:
    QJsonObject buildRequestBody(const QList<Message>& history,
                                  const QString& model,
                                  double temperature,
                                  int maxTokens) const;

    QThread* m_workerThread = nullptr;
    NetworkWorker* m_worker = nullptr;
    QMap<QString, QString> m_correlationToMessage;
};
