#include "tst_global.h"

#include "qxdock/dockbutton.h"

QX_DOCK_USE_NAMESPACE

class tst_DockButton : public QObject
{
    Q_OBJECT
private slots:
    void orientation();
    void sizeHint();
};

void tst_DockButton::orientation()
{
    DockButton btn;

    QCOMPARE(btn.buttonOrientation(), DockButton::Horizontal);

    btn.setButtonOrientation(DockButton::VerticalTopToBottom);
    QCOMPARE(btn.buttonOrientation(), DockButton::VerticalTopToBottom);

    btn.setButtonOrientation(DockButton::VerticalBottomToTop);
    QCOMPARE(btn.buttonOrientation(), DockButton::VerticalBottomToTop);
}

void tst_DockButton::sizeHint()
{
    DockButton btn;

    QSize s1 = btn.sizeHint();
    QPoint pt1 = btn.pos();

    btn.setButtonOrientation(DockButton::VerticalTopToBottom);
    QSize s2 = btn.sizeHint();
    QPoint pt2 = btn.pos();

    btn.setButtonOrientation(DockButton::VerticalBottomToTop);
    QSize s3 = btn.sizeHint();
    QPoint pt3 = btn.pos();

    QCOMPARE(s1.width(), s2.height());
    QCOMPARE(s1.height(), s2.width());
    QCOMPARE(s2, s3);

    QCOMPARE(pt1, pt2);
    QCOMPARE(pt2, pt3);
}

TEST_ADD(tst_DockButton)

#include "tst_dockbutton.moc"
