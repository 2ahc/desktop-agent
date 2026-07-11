#pragma once

#include <QObject>
#include <QSettings>

class AppSettings : public QObject
{
    Q_OBJECT
public:
    explicit AppSettings(QObject* parent = nullptr);

    QString endpoint() const;
    void setEndpoint(const QString& endpoint);

    QString model() const;
    void setModel(const QString& model);

    double temperature() const;
    void setTemperature(double temp);

    int maxTokens() const;
    void setMaxTokens(int tokens);

    int contextTokens() const;
    void setContextTokens(int tokens);

    QByteArray mainWindowGeometry() const;
    void setMainWindowGeometry(const QByteArray& geo);

    QByteArray mainWindowState() const;
    void setMainWindowState(const QByteArray& state);

    QString lastSessionId() const;
    void setLastSessionId(const QString& id);

signals:
    void settingsChanged();

private:
    QSettings m_settings;
};
