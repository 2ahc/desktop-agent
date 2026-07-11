#pragma once

#include <QSyntaxHighlighter>
#include <QRegularExpression>

class CodeHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit CodeHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct Rule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    void setupCppRules();
    void setupPythonRules();
    void setupJsonRules();

    QVector<Rule> m_rules;
};
