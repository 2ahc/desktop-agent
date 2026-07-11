#pragma once

#include <QDialog>
#include <QGroupBox>

class QLineEdit;
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;

class AppSettings;

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);

    void loadFromSettings(const AppSettings& settings);
    void saveToSettings(AppSettings& settings) const;

private:
    void setupUi();

    QLineEdit* m_endpointEdit = nullptr;
    QComboBox* m_modelCombo = nullptr;
    QDoubleSpinBox* m_temperatureSpin = nullptr;
    QSpinBox* m_maxTokensSpin = nullptr;
    QSpinBox* m_contextTokensSpin = nullptr;
};
