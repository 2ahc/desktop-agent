#pragma once

#include <QWidget>

class QPlainTextEdit;
class QPushButton;

class InputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InputWidget(QWidget* parent = nullptr);

    void clear();

public slots:
    void setEnabled(bool enabled);

signals:
    void sendClicked(const QString& text);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupUi();
    void updateSendButton();
    void sendMessage();

    QPlainTextEdit* m_textEdit = nullptr;
    QPushButton* m_sendButton = nullptr;
};
