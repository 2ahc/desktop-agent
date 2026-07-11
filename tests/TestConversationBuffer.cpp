#include <QtTest>
#include "core/ConversationBuffer.h"

class TestConversationBuffer : public QObject
{
    Q_OBJECT

private slots:
    void appendWithinLimits()
    {
        ConversationBuffer buf(100, 4096);
        for (int i = 0; i < 5; ++i) {
            buf.append(Message::create(Message::User, "test message " + QString::number(i)));
        }
        QCOMPARE(buf.messageCount(), 5);
        QVERIFY(buf.totalTokens() > 0);
    }

    void evictOnMessageCount()
    {
        ConversationBuffer buf(3, 99999);
        for (int i = 0; i < 5; ++i) {
            buf.append(Message::create(Message::User, "msg"));
        }
        QVERIFY(buf.messageCount() <= 3);
    }

    void evictOnTokenCount()
    {
        ConversationBuffer buf(100, 50);
        for (int i = 0; i < 10; ++i) {
            buf.append(Message::create(Message::User, QString(200, 'x')));
        }
        QVERIFY(buf.totalTokens() <= 50 || buf.messageCount() <= 1);
    }

    void systemMessagePreserved()
    {
        ConversationBuffer buf(2, 99999);
        buf.setSystemPrompt("You are a helpful assistant.");
        buf.append(Message::create(Message::User, "msg1"));
        buf.append(Message::create(Message::Assistant, "reply1"));
        buf.append(Message::create(Message::User, "msg2"));

        auto ctx = buf.getContext();
        QCOMPARE(ctx.first().role, Message::System);
        QVERIFY(ctx.first().content.contains("helpful"));
    }

    void getContextOrdering()
    {
        ConversationBuffer buf;
        buf.setSystemPrompt("System prompt");
        buf.append(Message::create(Message::User, "user1"));
        buf.append(Message::create(Message::Assistant, "assistant1"));

        auto ctx = buf.getContext();
        QCOMPARE(ctx.size(), 3);
        QCOMPARE(ctx[0].role, Message::System);
        QCOMPARE(ctx[1].role, Message::User);
        QCOMPARE(ctx[2].role, Message::Assistant);
    }

    void streamingMessageProtected()
    {
        ConversationBuffer buf(2, 99999);
        buf.append(Message::create(Message::User, "msg1"));

        Message streaming = Message::create(Message::Assistant, "");
        streaming.isStreaming = true;
        buf.append(streaming);

        buf.append(Message::create(Message::User, "msg3"));

        QVERIFY(buf.messageCount() >= 2);
    }

    void clear()
    {
        ConversationBuffer buf;
        buf.append(Message::create(Message::User, "test"));
        buf.clear();
        QCOMPARE(buf.messageCount(), 0);
        QCOMPARE(buf.totalTokens(), 0);
        QVERIFY(buf.isEmpty());
    }
};

QTEST_MAIN(TestConversationBuffer)
#include "TestConversationBuffer.moc"
