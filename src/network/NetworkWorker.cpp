#include "NetworkWorker.h"
#include "StreamParser.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>

NetworkWorker::NetworkWorker(QObject* parent)
    : QObject(parent)
{
    // QNAM will be created lazily in init() after moveToThread.
    // For now, create it directly — it must live in the worker thread.
    m_nam = new QNetworkAccessManager(this);
}

NetworkWorker::~NetworkWorker()
{
    for (auto* reply : m_activeReplies) {
        reply->abort();
    }
    for (auto* parser : m_activeParsers) {
        delete parser;
    }
}

void NetworkWorker::init() {}

void NetworkWorker::postChat(const QString& endpoint, const QJsonObject& body,
                              const QString& correlationId)
{
    QUrl url(endpoint + "/api/chat");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(120000);

    QJsonDocument doc(body);
    QNetworkReply* reply = m_nam->post(request, doc.toJson(QJsonDocument::Compact));

    auto* parser = new StreamParser(this);
    m_activeParsers.insert(correlationId, parser);
    m_activeReplies.insert(correlationId, reply);

    connect(parser, &StreamParser::tokenReceived, this,
            [this, correlationId](const QString& delta) {
                emit tokenReceived(correlationId, delta);
            });

    connect(parser, &StreamParser::streamComplete, this,
            [this, correlationId](const QString& fullContent) {
                emit requestComplete(correlationId, fullContent);
            });

    connect(parser, &StreamParser::parseError, this,
            [this, correlationId](const QString& error) {
                emit requestError(correlationId, error);
            });

    connect(reply, &QNetworkReply::readyRead, this, [this, reply, correlationId]() {
        QByteArray data = reply->readAll();
        if (auto* parser = m_activeParsers.value(correlationId)) {
            parser->feed(data);
        }
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply, correlationId]() {
        if (reply->error() != QNetworkReply::NoError
            && reply->error() != QNetworkReply::OperationCanceledError)
        {
            emit requestError(correlationId, reply->errorString());
        }
        m_activeReplies.remove(correlationId);
        reply->deleteLater();
    });

    connect(reply, &QNetworkReply::errorOccurred, this,
            [this, correlationId](QNetworkReply::NetworkError err) {
                Q_UNUSED(err)
                // Handled in finished() above
            });
}

void NetworkWorker::cancelRequest(const QString& correlationId)
{
    if (auto* reply = m_activeReplies.take(correlationId)) {
        reply->abort();
        reply->deleteLater();
    }
    if (auto* parser = m_activeParsers.take(correlationId)) {
        parser->deleteLater();
    }
}
