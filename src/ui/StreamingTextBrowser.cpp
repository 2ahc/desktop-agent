#include "StreamingTextBrowser.h"

#include <QTimer>
#include <QTextCursor>
#include <QTextBlockFormat>
#include <QRegularExpression>
#include <QScrollBar>

StreamingTextBrowser::StreamingTextBrowser(QWidget* parent)
    : QTextBrowser(parent)
{
    setReadOnly(true);
    setOpenExternalLinks(true);
    setFrameShape(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    document()->setDefaultStyleSheet(
        "body { font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif; }"
        "pre { background-color: #1e1e2e; color: #cdd6f4; "
        "     border-radius: 8px; padding: 14px 16px; margin: 8px 0;"
        "     font-family: 'Cascadia Code', 'Consolas', 'Courier New', monospace;"
        "     font-size: 13px; line-height: 1.5; overflow-x: auto; }"
        "code { background-color: #f0e6f6; color: #5e35b1; "
        "      padding: 2px 6px; border-radius: 4px; "
        "      font-family: 'Cascadia Code', 'Consolas', monospace; font-size: 13px; }"
        "pre code { background-color: transparent; color: inherit; padding: 0; border-radius: 0; }"
        "h1 { font-size: 20px; font-weight: 700; color: #333; margin: 12px 0 6px; }"
        "h2 { font-size: 17px; font-weight: 700; color: #444; margin: 10px 0 5px; }"
        "h3 { font-size: 15px; font-weight: 700; color: #555; margin: 8px 0 4px; }"
        "p { margin: 4px 0; }"
        "li { margin: 2px 0; }"
        "b, strong { color: #333; }"
        "i, em { color: #555; }"
    );

    m_flushTimer = new QTimer(this);
    m_flushTimer->setSingleShot(true);
    connect(m_flushTimer, &QTimer::timeout, this, &StreamingTextBrowser::flushPending);
}

void StreamingTextBrowser::appendDelta(const QString& delta)
{
    m_pendingText += delta;
    if (!m_flushTimer->isActive()) {
        m_flushTimer->start(30);
    }
}

void StreamingTextBrowser::finishStreaming()
{
    m_flushTimer->stop();
    flushPending();
}

void StreamingTextBrowser::setMarkdown(const QString& markdown)
{
    m_accumulatedText = markdown;
    setHtml(markdownToHtml(markdown));
}

void StreamingTextBrowser::flushPending()
{
    if (m_pendingText.isEmpty()) return;

    m_accumulatedText += m_pendingText;
    QString html = markdownToHtml(m_accumulatedText);
    setHtml(html);

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);

    m_pendingText.clear();
}

QString StreamingTextBrowser::markdownToHtml(const QString& md) const
{
    QString html = md;

    // Escape HTML entities
    html.replace("&", "&amp;");
    html.replace("<", "&lt;");
    html.replace(">", "&gt;");

    // Restore already-escaped sequences
    html.replace("&amp;lt;", "&lt;");
    html.replace("&amp;gt;", "&gt;");
    html.replace("&amp;amp;", "&amp;");

    // Fenced code blocks: ```lang\n...\n```
    static QRegularExpression codeBlockRe("```(\\w*)\\n([\\s\\S]*?)```");
    int idx = 0;
    while (true) {
        auto match = codeBlockRe.match(html, idx);
        if (!match.hasMatch()) break;
        QString code = match.captured(2);
        // Escape HTML inside code
        code.replace("&", "&amp;");
        code.replace("<", "&lt;");
        code.replace(">", "&gt;");
        QString replacement = "<pre><code>" + code + "</code></pre>";
        html.replace(match.capturedStart(), match.capturedLength(), replacement);
        idx = match.capturedStart() + replacement.length();
    }

    // Inline code: `text`
    static QRegularExpression inlineCodeRe("`([^`]+)`");
    html.replace(inlineCodeRe, "<code>\\1</code>");

    // Bold: **text**
    static QRegularExpression boldRe("\\*\\*(.+?)\\*\\*");
    html.replace(boldRe, "<b>\\1</b>");

    // Italic: *text* (but not **)
    static QRegularExpression italicRe("(?<!\\*)\\*(?!\\*)(.+?)(?<!\\*)\\*(?!\\*)");
    html.replace(italicRe, "<i>\\1</i>");

    // Headers
    static QRegularExpression h3Re("^### (.+)$", QRegularExpression::MultilineOption);
    html.replace(h3Re, "<h3>\\1</h3>");
    static QRegularExpression h2Re("^## (.+)$", QRegularExpression::MultilineOption);
    html.replace(h2Re, "<h2>\\1</h2>");
    static QRegularExpression h1Re("^# (.+)$", QRegularExpression::MultilineOption);
    html.replace(h1Re, "<h1>\\1</h1>");

    // Unordered lists
    static QRegularExpression ulRe("^(\\-|\\*) (.+)$", QRegularExpression::MultilineOption);
    html.replace(ulRe, "<li>\\2</li>");

    // Ordered lists
    static QRegularExpression olRe("^\\d+\\. (.+)$", QRegularExpression::MultilineOption);
    html.replace(olRe, "<li>\\1</li>");

    // Horizontal rule
    static QRegularExpression hrRe("^---$", QRegularExpression::MultilineOption);
    html.replace(hrRe, "<hr>");

    // Paragraph breaks
    html.replace("\n\n", "<br><br>");
    html.replace("\n", "<br>");

    return html;
}
