#include "tst_global.h"

#include "qxribbon/windowbuttongroup.h"

#include <QSignalSpy>
#include <QToolButton>
#include <QWidget>

QX_RIBBON_USE_NAMESPACE

/*
 * WindowButtonGroup is the stand-in for the system title bar buttons a frameless ribbon
 * window has to draw itself. It has two modes, and which one is active decides who acts
 * when a button is clicked:
 *
 *   signalIsEnabled() == false (the default) - the group drives its parent widget
 *                       directly: minimize, maximize/restore, close.
 *   signalIsEnabled() == true                - the group only reports the click, and the
 *                       owner - normally the frameless helper - decides what it means.
 *
 * The parent widget has to be visible for isVisible() on the child buttons to mean
 * anything, so the cases that read button visibility show the host first.
 */
class tst_WindowButtonGroup : public QObject
{
    Q_OBJECT
private:
    static QWidget *button(WindowButtonGroup *group, const QString &name)
    {
        return group->findChild<QToolButton *>(name);
    }

private slots:
    void signalIsDisabledByDefault();
    void updateWindowFlagsDecidesWhatIsVisible();
    void sizeHintFollowsTheVisibleButtons();
    void setWindowStatesChecksTheMaximizeButton();
    void widgetForMaximizeButtonIsTheOneInTheLayout();
    void enabledSignalsReachTheOwner();
    void disabledSignalsActOnTheParentWindow();
};

void tst_WindowButtonGroup::signalIsDisabledByDefault()
{
    QWidget host;
    WindowButtonGroup group(&host);

    QVERIFY(!group.signalIsEnabled());

    group.setSignalEnabled(true);
    QVERIFY(group.signalIsEnabled());

    group.setSignalEnabled(false);
    QVERIFY(!group.signalIsEnabled());
}

void tst_WindowButtonGroup::updateWindowFlagsDecidesWhatIsVisible()
{
    QWidget host;
    WindowButtonGroup group(&host, Qt::WindowCloseButtonHint);
    host.show();

    QVERIFY(button(&group, QStringLiteral("qx_CloseWindowButton"))->isVisible());
    QVERIFY(!button(&group, QStringLiteral("qx_MinimizeWindowButton"))->isVisible());
    QVERIFY(!button(&group, QStringLiteral("qx_MaximizeWindowButton"))->isVisible());

    group.updateWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    QVERIFY(button(&group, QStringLiteral("qx_CloseWindowButton"))->isVisible());
    QVERIFY(button(&group, QStringLiteral("qx_MinimizeWindowButton"))->isVisible());
    QVERIFY(!button(&group, QStringLiteral("qx_MaximizeWindowButton"))->isVisible());
}

void tst_WindowButtonGroup::sizeHintFollowsTheVisibleButtons()
{
    QWidget host;
    WindowButtonGroup group(&host, Qt::WindowCloseButtonHint);
    host.show();

    // Close is the widest of the three.
    QCOMPARE(group.sizeHint().width(), 40);

    group.updateWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    QCOMPARE(group.sizeHint().width(), 30 + 30 + 40);

    // The height comes from the ribbon style option, which is never zero.
    QVERIFY(group.sizeHint().height() > 0);
}

void tst_WindowButtonGroup::setWindowStatesChecksTheMaximizeButton()
{
    QWidget host;
    WindowButtonGroup group(&host, Qt::WindowMaximizeButtonHint);

    QAbstractButton *maximize =
        qobject_cast<QAbstractButton *>(button(&group, QStringLiteral("qx_MaximizeWindowButton")));
    QVERIFY(maximize != Q_NULLPTR);
    QVERIFY(!maximize->isChecked());

    group.setWindowStates(Qt::WindowMaximized);
    QVERIFY(maximize->isChecked());

    group.setWindowStates(Qt::WindowNoState);
    QVERIFY(!maximize->isChecked());
}

void tst_WindowButtonGroup::widgetForMaximizeButtonIsTheOneInTheLayout()
{
    QWidget host;
    WindowButtonGroup group(&host, Qt::WindowMaximizeButtonHint);

    QWidget *maximize = group.widgetForMaximizeButton();
    QVERIFY(maximize != Q_NULLPTR);
    QCOMPARE(maximize, button(&group, QStringLiteral("qx_MaximizeWindowButton")));
    QCOMPARE(maximize->parentWidget(), &group);
}

void tst_WindowButtonGroup::enabledSignalsReachTheOwner()
{
    QWidget host;
    WindowButtonGroup group(&host,
                            Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    group.setSignalEnabled(true);
    host.show();

    QSignalSpy minimize(&group, &WindowButtonGroup::buttonMinimizeClicked);
    QSignalSpy maximize(&group, &WindowButtonGroup::buttonMaximzieClicked);
    QSignalSpy close(&group, &WindowButtonGroup::buttonCloseClicked);

    qobject_cast<QAbstractButton *>(button(&group, QStringLiteral("qx_MinimizeWindowButton")))->click();
    qobject_cast<QAbstractButton *>(button(&group, QStringLiteral("qx_MaximizeWindowButton")))->click();
    qobject_cast<QAbstractButton *>(button(&group, QStringLiteral("qx_CloseWindowButton")))->click();

    QCOMPARE(minimize.count(), 1);
    QCOMPARE(maximize.count(), 1);
    QCOMPARE(close.count(), 1);
    // Reporting the click is all it does: the host window is left standing, which is
    // the whole difference from the mode below.
    QVERIFY(host.isVisible());
}

void tst_WindowButtonGroup::disabledSignalsActOnTheParentWindow()
{
    QWidget host;
    WindowButtonGroup group(&host, Qt::WindowCloseButtonHint);
    host.show();

    QSignalSpy close(&group, &WindowButtonGroup::buttonCloseClicked);

    // Signals are off, so the group closes the host window itself instead of telling
    // anyone about it. Closing is the one action of the three that is observable
    // offscreen and reversible.
    qobject_cast<QAbstractButton *>(button(&group, QStringLiteral("qx_CloseWindowButton")))->click();

    QCOMPARE(close.count(), 0);
    QVERIFY(host.isHidden());
}

TEST_ADD(tst_WindowButtonGroup)

#include "tst_windowbuttongroup.moc"
