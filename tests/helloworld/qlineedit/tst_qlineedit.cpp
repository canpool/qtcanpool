#include <QtTest/QtTest>
#include <QLineEdit>

class TestQLineEdit: public QObject
{
    Q_OBJECT
private slots:
    void setText();
};

void TestQLineEdit::setText()
{
    QLineEdit lineEdit;
    QTest::keyClicks(&lineEdit, "hello world");
    QCOMPARE(lineEdit.text(), QString("hello world"));
}

QTEST_MAIN(TestQLineEdit)

#include "tst_qlineedit.moc"
