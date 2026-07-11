#include "AppSettings.h"

AppSettings::AppSettings(QObject* parent)
    : QObject(parent)
    , m_settings("DesktopAgent", "DesktopAgent")
{
}

QString AppSettings::endpoint() const
{
    return m_settings.value("api/endpoint", "http://localhost:11434").toString();
}

void AppSettings::setEndpoint(const QString& endpoint)
{
    m_settings.setValue("api/endpoint", endpoint);
    emit settingsChanged();
}

QString AppSettings::model() const
{
    return m_settings.value("api/model", "qwen2.5:7b").toString();
}

void AppSettings::setModel(const QString& model)
{
    m_settings.setValue("api/model", model);
    emit settingsChanged();
}

double AppSettings::temperature() const
{
    return m_settings.value("api/temperature", 0.7).toDouble();
}

void AppSettings::setTemperature(double temp)
{
    m_settings.setValue("api/temperature", temp);
    emit settingsChanged();
}

int AppSettings::maxTokens() const
{
    return m_settings.value("api/maxTokens", 2048).toInt();
}

void AppSettings::setMaxTokens(int tokens)
{
    m_settings.setValue("api/maxTokens", tokens);
    emit settingsChanged();
}

int AppSettings::contextTokens() const
{
    return m_settings.value("api/contextTokens", 4096).toInt();
}

void AppSettings::setContextTokens(int tokens)
{
    m_settings.setValue("api/contextTokens", tokens);
    emit settingsChanged();
}

QByteArray AppSettings::mainWindowGeometry() const
{
    return m_settings.value("ui/geometry").toByteArray();
}

void AppSettings::setMainWindowGeometry(const QByteArray& geo)
{
    m_settings.setValue("ui/geometry", geo);
}

QByteArray AppSettings::mainWindowState() const
{
    return m_settings.value("ui/state").toByteArray();
}

void AppSettings::setMainWindowState(const QByteArray& state)
{
    m_settings.setValue("ui/state", state);
}

QString AppSettings::lastSessionId() const
{
    return m_settings.value("session/lastId").toString();
}

void AppSettings::setLastSessionId(const QString& id)
{
    m_settings.setValue("session/lastId", id);
}
