#include <QtTest>
#include "core/TokenEstimator.h"

class TestTokenEstimator : public QObject
{
    Q_OBJECT

private slots:
    void estimateEmptyString()
    {
        QCOMPARE(TokenEstimator::estimate(""), 0);
    }

    void estimateEnglish()
    {
        int tokens = TokenEstimator::estimate("Hello world");
        QVERIFY(tokens >= 2);
        QVERIFY(tokens <= 5);
    }

    void estimateChinese()
    {
        int tokens = TokenEstimator::estimate("你好世界");
        QVERIFY(tokens >= 3);
        QVERIFY(tokens <= 6);
    }

    void estimateMixedScript()
    {
        int tokens = TokenEstimator::estimate("Hello 你好 world 世界");
        QVERIFY(tokens >= 5);
        QVERIFY(tokens <= 10);
    }

    void estimateLongText()
    {
        QString text(4000, 'a');
        int tokens = TokenEstimator::estimate(text);
        int expected = 4000 / 4 + 1;
        QCOMPARE(tokens, expected);
    }
};

QTEST_MAIN(TestTokenEstimator)
#include "TestTokenEstimator.moc"
