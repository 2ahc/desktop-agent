#include "OllamaClient.h"
#include "NetworkWorker.h"

#include <QThread>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUuid>

OllamaClient::OllamaClient(QObject* parent)
    : QObject(parent)
{
    m_workerThread = new QThread(this);
    m_worker = new NetworkWorker();

    m_worker->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::finished,
            m_worker, &QObject::deleteLater);

    connect(this, &OllamaClient::_internalSendRequest,
            m_worker, &NetworkWorker::postChat,
            Qt::QueuedConnection);

    connect(this, &OllamaClient::_internalCancelRequest,
            m_worker, &NetworkWorker::cancelRequest,
            Qt::QueuedConnection);

    connect(m_worker, &NetworkWorker::tokenReceived,
            this, &OllamaClient::onWorkerToken,
            Qt::QueuedConnection);

    connect(m_worker, &NetworkWorker::requestComplete,
            this, &OllamaClient::onWorkerComplete,
            Qt::QueuedConnection);

    connect(m_worker, &NetworkWorker::requestError,
            this, &OllamaClient::onWorkerError,
            Qt::QueuedConnection);

    m_workerThread->start();
}

OllamaClient::~OllamaClient()
{
    m_workerThread->quit();
    m_workerThread->wait(5000);
}

void OllamaClient::sendChat(const QString& messageId,
                             const QList<Message>& history,
                             const QString& model,
                             const QString& endpoint,
                             double temperature,
                             int maxTokens)
{
    QString correlationId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_correlationToMessage.insert(correlationId, messageId);

    QJsonObject body = buildRequestBody(history, model, temperature, maxTokens);
    emit _internalSendRequest(endpoint, body, correlationId);
}

void OllamaClient::cancelChat(const QString& messageId)
{
    for (auto it = m_correlationToMessage.begin(); it != m_correlationToMessage.end(); ++it) {
        if (it.value() == messageId) {
            emit _internalCancelRequest(it.key());
            m_correlationToMessage.erase(it);
            break;
        }
    }
}

QJsonObject OllamaClient::buildRequestBody(const QList<Message>& history,
                                            const QString& model,
                                            double temperature,
                                            int maxTokens) const
{
    QJsonArray messages;
    for (const auto& msg : history) {
        QJsonObject m;
        QString role;
        switch (msg.role) {
        case Message::System:    role = "system"; break;
        case Message::Assistant: role = "assistant"; break;
        default:                 role = "user"; break;
        }
        m["role"] = role;
        m["content"] = msg.content;
        messages.append(m);
    }

    QJsonObject options;
    options["temperature"] = temperature;
    options["num_predict"] = maxTokens;

    QJsonObject body;
    body["model"] = model;
    body["messages"] = messages;
    body["stream"] = true;
    body["options"] = options;

    return body;
}

void OllamaClient::onWorkerToken(const QString& correlationId, const QString& delta)
{
    if (auto msgId = m_correlationToMessage.value(correlationId); !msgId.isEmpty()) {
        emit streamChunk(msgId, delta);
    }
}

void OllamaClient::onWorkerComplete(const QString& correlationId, const QString& fullContent)
{
    if (auto msgId = m_correlationToMessage.take(correlationId); !msgId.isEmpty()) {
        emit streamFinished(msgId, fullContent);
    }
}

void OllamaClient::onWorkerError(const QString& correlationId, const QString& error)
{
    if (auto msgId = m_correlationToMessage.take(correlationId); !msgId.isEmpty()) {
        emit streamError(msgId, error);
    }
}
