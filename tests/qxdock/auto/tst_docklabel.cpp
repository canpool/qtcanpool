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

    int i = 0;
    connect(&label, &DockLabel::elidedChanged, this, [&i](bool) {
        ++i;
    });

    QCOMPARE(label.text(), "dock1");
    QCOMPARE(label.isElided(), false);
    QCOMPARE(label.elideMode(), Qt::ElideNone);
    QCOMPARE(i, 0);

    label.resize(label.minimumSizeHint());
    QCOMPARE(label.isElided(), false);
    QCOMPARE(i, 0);

    label.setElideMode(Qt::ElideRight);
    QCOMPARE(label.text(), "dock1");
    QCOMPARE(label.isElided(), false);
    QCOMPARE(i, 0);

    label.resize(label.minimumSizeHint());
    QCOMPARE(label.text(), "dock1");
    QCOMPARE(label.isElided(), false);
    QCOMPARE(i, 0);

    label.setText("loogdock");
    QCOMPARE(label.isElided(), true);
    QVERIFY(label.QLabel::text().endsWith(u'\x2026'));
    QCOMPARE(i, 1);

    label.setElideMode(Qt::ElideLeft);
    QCOMPARE(label.isElided(), true);
    QVERIFY(label.QLabel::text().startsWith(u'\x2026'));
    QCOMPARE(i, 1);

    label.setElideMode(Qt::ElideMiddle);
    QCOMPARE(label.isElided(), true);
    QVERIFY(label.QLabel::text().contains(u'\x2026'));
    QCOMPARE(i, 1);
}

TEST_ADD(tst_DockLabel)

#include "tst_docklabel.moc"
