#include "tst_global.h"

#include "qxdock/docklabel.h"

#include <QDebug>

QX_DOCK_USE_NAMESPACE

class tst_DockLabel : public QObject
{
    Q_OBJECT
private slots:
    void elide();
};

void tst_DockLabel::elide()
{
    DockLabel label("dock1");
    QCOMPARE(label.text(), "dock1");
    QVERIFY(!label.isElided());
    QCOMPARE(label.elideMode(), Qt::ElideNone);

    label.resize(label.minimumSizeHint());
    QVERIFY(!label.isElided());

    label.setElideMode(Qt::ElideRight);
    QCOMPARE(label.text(), "dock1");
    QVERIFY(!label.isElided());

    label.resize(label.minimumSizeHint());
    QCOMPARE(label.text(), "dock1");
    QVERIFY(!label.isElided());

    label.setText("loogdock");
    QVERIFY(label.isElided());
    QVERIFY(label.QLabel::text().endsWith(u'\x2026'));
}

TEST_ADD(tst_DockLabel)

#include "tst_docklabel.moc"
