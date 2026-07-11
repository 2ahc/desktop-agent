#include "SettingsDialog.h"
#include "utils/AppSettings.h"

#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QGroupBox>

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Settings"));
    setMinimumWidth(460);
    setupUi();
}

void SettingsDialog::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);

    // Header
    auto* header = new QLabel(QString::fromUtf8("\xF0\x9F\x90\xB4  Settings"), this);
    header->setStyleSheet("font-size: 20px; font-weight: 700; color: #5e35b1; padding: 4px 0;");
    mainLayout->addWidget(header);

    // API Group
    auto* apiGroup = new QGroupBox(tr("API Configuration"), this);
    auto* apiLayout = new QFormLayout(apiGroup);
    apiLayout->setSpacing(10);

    m_endpointEdit = new QLineEdit("http://localhost:11434", this);
    m_endpointEdit->setPlaceholderText("http://localhost:11434");
    apiLayout->addRow(tr("Endpoint:"), m_endpointEdit);

    m_modelCombo = new QComboBox(this);
    m_modelCombo->setEditable(true);
    m_modelCombo->addItems({"qwen2.5:7b", "qwen2.5:14b", "qwen2.5:3b", "llama3.2", "mistral", "codellama"});
    apiLayout->addRow(tr("Model:"), m_modelCombo);

    mainLayout->addWidget(apiGroup);

    // Generation Group
    auto* genGroup = new QGroupBox(tr("Generation Parameters"), this);
    auto* genLayout = new QFormLayout(genGroup);
    genLayout->setSpacing(10);

    m_temperatureSpin = new QDoubleSpinBox(this);
    m_temperatureSpin->setRange(0.0, 2.0);
    m_temperatureSpin->setSingleStep(0.1);
    m_temperatureSpin->setValue(0.7);
    m_temperatureSpin->setToolTip(tr("Higher = more creative, Lower = more deterministic"));
    genLayout->addRow(tr("Temperature:"), m_temperatureSpin);

    m_maxTokensSpin = new QSpinBox(this);
    m_maxTokensSpin->setRange(1, 32768);
    m_maxTokensSpin->setValue(2048);
    m_maxTokensSpin->setToolTip(tr("Maximum tokens in the response"));
    genLayout->addRow(tr("Max Output Tokens:"), m_maxTokensSpin);

    m_contextTokensSpin = new QSpinBox(this);
    m_contextTokensSpin->setRange(256, 131072);
    m_contextTokensSpin->setValue(4096);
    m_contextTokensSpin->setToolTip(tr("Maximum tokens in context window"));
    genLayout->addRow(tr("Context Window:"), m_contextTokensSpin);

    mainLayout->addWidget(genGroup);

    mainLayout->addStretch(1);

    // Buttons
    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText(tr("Save"));
    buttons->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
}

void SettingsDialog::loadFromSettings(const AppSettings& settings)
{
    m_endpointEdit->setText(settings.endpoint());
    m_modelCombo->setCurrentText(settings.model());
    m_temperatureSpin->setValue(settings.temperature());
    m_maxTokensSpin->setValue(settings.maxTokens());
    m_contextTokensSpin->setValue(settings.contextTokens());
}

void SettingsDialog::saveToSettings(AppSettings& settings) const
{
    settings.setEndpoint(m_endpointEdit->text());
    settings.setModel(m_modelCombo->currentText());
    settings.setTemperature(m_temperatureSpin->value());
    settings.setMaxTokens(m_maxTokensSpin->value());
    settings.setContextTokens(m_contextTokensSpin->value());
}
