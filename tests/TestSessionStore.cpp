#include <QtTest>
#include "core/SessionStore.h"
#include "core/Session.h"

#include <QDir>
#include <QTemporaryDir>

class TestSessionStore : public QObject
{
    Q_OBJECT

private:
    void cleanDir(const QString& path)
    {
        QDir dir(path);
        for (const auto& fi : dir.entryInfoList({"*.json"}, QDir::Files)) {
            QFile::remove(fi.absoluteFilePath());
        }
    }

private slots:
    void saveAndLoad()
    {
        QTemporaryDir dir;
        SessionStore store(dir.path());

        Session s;
        s.id = "test-session-1";
        s.title = "Test Chat";
        s.createdAt = QDateTime::currentDateTimeUtc();
        s.updatedAt = s.createdAt;
        s.buffer.append(Message::create(Message::User, "Hello"));
        s.buffer.append(Message::create(Message::Assistant, "Hi there!"));

        QVERIFY(store.save(s));

        auto sessions = store.loadAll();
        QCOMPARE(sessions.size(), 1);
        QCOMPARE(sessions.first().id, "test-session-1");
        QCOMPARE(sessions.first().title, "Test Chat");
        QCOMPARE(sessions.first().buffer.messageCount(), 2);
    }

    void saveMultiple()
    {
        QTemporaryDir dir;
        SessionStore store(dir.path());

        for (int i = 0; i < 3; ++i) {
            Session s;
            s.id = QString("session-%1").arg(i);
            s.title = QString("Chat %1").arg(i);
            s.buffer.append(Message::create(Message::User, "msg"));
            QVERIFY(store.save(s));
        }

        auto sessions = store.loadAll();
        QCOMPARE(sessions.size(), 3);
    }

    void remove()
    {
        QTemporaryDir dir;
        SessionStore store(dir.path());

        Session s;
        s.id = "to-delete";
        s.title = "Delete Me";
        QVERIFY(store.save(s));
        QVERIFY(store.remove("to-delete"));

        auto sessions = store.loadAll();
        QCOMPARE(sessions.size(), 0);
    }

    void loadEmptyDirectory()
    {
        QTemporaryDir emptyDir;
        SessionStore store(emptyDir.path());
        auto sessions = store.loadAll();
        QCOMPARE(sessions.size(), 0);
    }

    void specialCharactersInTitle()
    {
        QTemporaryDir dir;
        SessionStore store(dir.path());

        Session s;
        s.id = "special-chars";
        s.title = QString::fromUtf8("你好世界 \"quoted\" 'single' /path\\to");
        QVERIFY(store.save(s));

        auto sessions = store.loadAll();
        QCOMPARE(sessions.size(), 1);
        QVERIFY(sessions.first().title.contains("你好"));
    }

    void overwrite()
    {
        QTemporaryDir dir;
        SessionStore store(dir.path());

        Session s;
        s.id = "overwrite-test";
        s.title = "Original";
        QVERIFY(store.save(s));

        s.title = "Updated";
        QVERIFY(store.save(s));

        auto sessions = store.loadAll();
        QCOMPARE(sessions.size(), 1);
        QCOMPARE(sessions.first().title, "Updated");
    }
};

QTEST_MAIN(TestSessionStore)
#include "TestSessionStore.moc"
