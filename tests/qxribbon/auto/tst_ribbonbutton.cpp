#include "tst_global.h"

#include "qxribbon/ribbonbutton.h"

#include <QAction>
#include <QApplication>
#include <QHelpEvent>
#include <QKeySequence>
#include <QLabel>

QX_RIBBON_USE_NAMESPACE

class tst_RibbonButton : public QObject
{
    Q_OBJECT
private slots:
    void buttonType();
    void size();
    void screenTip();
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

void tst_RibbonButton::screenTip()
{
    RibbonButton btn;
    QAction action(QStringLiteral("&Paste"), &btn);
    action.setStatusTip(QStringLiteral("Insert the clipboard contents"));
    action.setShortcut(QKeySequence(QStringLiteral("Ctrl+V")));
    btn.setDefaultAction(&action);

    QHelpEvent tipEvent(QEvent::ToolTip, QPoint(2, 2), btn.mapToGlobal(QPoint(2, 2)));
    QApplication::sendEvent(&btn, &tipEvent);

    // ScreenTip (B6): the tip is a shared top-level tool window, found by object name.
    QWidget *tip = nullptr;
    for (QWidget *widget : QApplication::topLevelWidgets()) {
        if (widget->objectName() == QStringLiteral("RibbonScreenTip")) {
            tip = widget;
        }
    }
    QVERIFY(tip);
    QVERIFY(tip->isVisible());
    // The '&' mnemonic is stripped, and the description / shortcut are shown verbatim.
    QCOMPARE(tip->findChild<QLabel *>(QStringLiteral("RibbonScreenTipTitle"))->text(), QStringLiteral("Paste"));
    QCOMPARE(tip->findChild<QLabel *>(QStringLiteral("RibbonScreenTipDescription"))->text(),
             QStringLiteral("Insert the clipboard contents"));
    QCOMPARE(tip->findChild<QLabel *>(QStringLiteral("RibbonScreenTipShortcut"))->text(),
             QKeySequence(QStringLiteral("Ctrl+V")).toString(QKeySequence::NativeText));

    // Leaving the button hides it again.
    QEvent leaveEvent(QEvent::Leave);
    QApplication::sendEvent(&btn, &leaveEvent);
    QVERIFY(!tip->isVisible());

    // An action with neither a description nor a shortcut keeps the plain tooltip:
    // the ScreenTip must stay out of the way, so no tip is shown for it.
    RibbonButton plainBtn;
    QAction plainAction(QStringLiteral("Copy"), &plainBtn);
    plainBtn.setDefaultAction(&plainAction);
    QHelpEvent plainEvent(QEvent::ToolTip, QPoint(2, 2), plainBtn.mapToGlobal(QPoint(2, 2)));
    QApplication::sendEvent(&plainBtn, &plainEvent);
    QVERIFY(!tip->isVisible());
}

TEST_ADD(tst_RibbonButton)

#include "tst_ribbonbutton.moc"
