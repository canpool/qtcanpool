#include "tst_global.h"

#include "qxribbon/ribbonbutton.h"

QX_RIBBON_USE_NAMESPACE

class tst_RibbonButton : public QObject
{
    Q_OBJECT
private slots:
    void buttonType();
    void size();
};

void tst_RibbonButton::buttonType()
{
    RibbonButton btn;

    RibbonButton::RibbonButtonType type = btn.buttonType();
    QCOMPARE(type, RibbonButton::SmallButton);
    QCOMPARE(btn.toolButtonStyle(), Qt::ToolButtonTextBesideIcon);

    QCOMPARE(btn.largeButtonType(), RibbonButton::Normal);

    btn.setButtonType(RibbonButton::LargeButton);
    QCOMPARE(btn.buttonType(), RibbonButton::LargeButton);
    QCOMPARE(btn.toolButtonStyle(), Qt::ToolButtonTextUnderIcon);

    QCOMPARE(btn.largeButtonType(), RibbonButton::Normal);

    btn.setLargeButtonType(RibbonButton::Lite);
    QCOMPARE(btn.largeButtonType(), RibbonButton::Lite);
}

void tst_RibbonButton::size()
{
    RibbonButton btn;
    QCOMPARE(btn.sizeHint(), btn.minimumSizeHint());
}

TEST_ADD(tst_RibbonButton)

#include "tst_ribbonbutton.moc"
