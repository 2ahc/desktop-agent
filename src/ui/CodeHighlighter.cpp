#include "CodeHighlighter.h"

CodeHighlighter::CodeHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    setupCppRules();
}

void CodeHighlighter::highlightBlock(const QString& text)
{
    for (const auto& rule : m_rules) {
        auto it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            auto match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

void CodeHighlighter::setupCppRules()
{
    QTextCharFormat keywordFmt;
    keywordFmt.setForeground(QColor("#d73a49"));
    keywordFmt.setFontWeight(QFont::Bold);

    QStringList keywords = {
        "auto", "break", "case", "class", "const", "continue", "default",
        "delete", "do", "else", "enum", "explicit", "for", "goto", "if",
        "int", "long", "namespace", "new", "operator", "private", "protected",
        "public", "return", "short", "sizeof", "static", "struct", "switch",
        "template", "this", "typedef", "union", "unsigned", "virtual", "void",
        "volatile", "while", "override", "final", "noexcept", "nullptr",
        "constexpr", "include", "pragma"
    };

    for (const auto& kw : keywords) {
        Rule r;
        r.pattern = QRegularExpression("\\b" + kw + "\\b");
        r.format = keywordFmt;
        m_rules.append(r);
    }

    QTextCharFormat stringFmt;
    stringFmt.setForeground(QColor("#032f62"));
    Rule strRule;
    strRule.pattern = QRegularExpression("\"[^\"]*\"");
    strRule.format = stringFmt;
    m_rules.append(strRule);

    QTextCharFormat commentFmt;
    commentFmt.setForeground(QColor("#6a737d"));
    commentFmt.setFontItalic(true);
    Rule commentRule;
    commentRule.pattern = QRegularExpression("//[^\n]*");
    commentRule.format = commentFmt;
    m_rules.append(commentRule);

    QTextCharFormat numberFmt;
    numberFmt.setForeground(QColor("#005cc5"));
    Rule numRule;
    numRule.pattern = QRegularExpression("\\b[0-9]+(\\.[0-9]+)?\\b");
    numRule.format = numberFmt;
    m_rules.append(numRule);
}

void CodeHighlighter::setupPythonRules()
{
    Q_UNUSED(this)
}

void CodeHighlighter::setupJsonRules()
{
    Q_UNUSED(this)
}
