#include <QtTest>
#include "network/StreamParser.h"

class TestStreamParser : public QObject
{
    Q_OBJECT

private slots:
    void singleCompleteLine()
    {
        StreamParser parser;
        QString receivedDelta;
        connect(&parser, &StreamParser::tokenReceived,
                [&](const QString& d) { receivedDelta = d; });

        QByteArray line = R"({"model":"llama3","message":{"role":"assistant","content":"Hello"},"done":false})";
        parser.feed(line + "\n");
        QCOMPARE(receivedDelta, "Hello");
    }

    void partialThenComplete()
    {
        StreamParser parser;
        QStringList deltas;
        connect(&parser, &StreamParser::tokenReceived,
                [&](const QString& d) { deltas.append(d); });

        parser.feed(QByteArray(R"({"model":"llama3","message":{"content":"Hel)"));
        parser.feed(QByteArray(R"(lo"},"done":false})") + QByteArray("\n"));

        QCOMPARE(deltas.size(), 1);
        QCOMPARE(deltas.first(), "Hello");
    }

    void multipleLinesInChunk()
    {
        StreamParser parser;
        QStringList deltas;
        connect(&parser, &StreamParser::tokenReceived,
                [&](const QString& d) { deltas.append(d); });

        QByteArray chunk =
            R"({"message":{"content":"A"},"done":false})" + QByteArray("\n") +
            R"({"message":{"content":"B"},"done":false})" + QByteArray("\n");

        parser.feed(chunk);
        QCOMPARE(deltas.size(), 2);
    }

    void doneFlag()
    {
        StreamParser parser;
        QString fullContent;
        connect(&parser, &StreamParser::streamComplete,
                [&](const QString& c) { fullContent = c; });

        parser.feed(QByteArray(R"({"message":{"content":"Hi"},"done":false})") + QByteArray("\n"));
        parser.feed(QByteArray(R"({"message":{"content":""},"done":true})") + QByteArray("\n"));

        QCOMPARE(fullContent, "Hi");
    }

    void malformedJson()
    {
        StreamParser parser;
        QString errorMsg;
        connect(&parser, &StreamParser::parseError,
                [&](const QString& e) { errorMsg = e; });

        parser.feed(QByteArray("not valid json\n"));
        QVERIFY(!errorMsg.isEmpty());
        QVERIFY(errorMsg.contains("parse error"));
    }

    void emptyLineSkipped()
    {
        StreamParser parser;
        int tokenCount = 0;
        connect(&parser, &StreamParser::tokenReceived, [&](const QString&) { tokenCount++; });

        parser.feed(QByteArray("\n\n") + QByteArray(R"({"message":{"content":"X"},"done":false})") + QByteArray("\n\n\n"));
        QCOMPARE(tokenCount, 1);
    }

    void resetMidStream()
    {
        StreamParser parser;
        connect(&parser, &StreamParser::tokenReceived, [](const QString&) {});

        parser.feed(QByteArray(R"({"message":{"content":"A"},"done":false})") + QByteArray("\n"));
        parser.reset();

        QString content;
        connect(&parser, &StreamParser::tokenReceived,
                [&](const QString& d) { content = d; });

        parser.feed(QByteArray(R"({"message":{"content":"Fresh"},"done":false})") + QByteArray("\n"));
        QCOMPARE(content, "Fresh");
    }
};

QTEST_MAIN(TestStreamParser)
#include "TestStreamParser.moc"
