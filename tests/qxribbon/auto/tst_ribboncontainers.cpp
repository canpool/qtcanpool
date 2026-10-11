#include "tst_global.h"

#include "qxribbon/ribboncontainers.h"
#include "qxribbon/ribbonmenu.h"

#include <QAction>
#include <QApplication>

QX_RIBBON_USE_NAMESPACE

/*
 * The first time the pointer enters a RibbonGridContainer, the container walks up to the
 * RibbonMenu it lives in and drops that menu's active action, so that hovering inside a
 * menu does not leave two entries looking current at once.
 *
 * The handler is a QWidget virtual whose parameter changed in Qt 6: QEvent became
 * QEnterEvent. A handler declared with the Qt 5 signature does not override it on Qt 6,
 * it hides it, and the fallout is silent - the code compiles, nothing crashes, the
 * handler just never runs. clang's "hides overloaded virtual function" was the only sign
 * that it had happened at all. These tests deliver the event the way Qt delivers it,
 * which is what makes the difference observable.
 */

class tst_RibbonContainers : public QObject
{
    Q_OBJECT
private:
    static void sendEnter(QWidget *widget)
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QEnterEvent event(QPointF(1, 1), QPointF(1, 1), QPointF(1, 1));
#else
        QEvent event(QEvent::Enter);
#endif
        QApplication::sendEvent(widget, &event);
    }

private slots:
    void enterClearsTheMenuActiveAction();
    void leaveLetsTheNextEnterClearAgain();
};

void tst_RibbonContainers::enterClearsTheMenuActiveAction()
{
    RibbonMenu menu;
    RibbonGridContainer container(&menu);
    QAction *action = menu.addAction(QStringLiteral("action"));
    menu.setActiveAction(action);
    QCOMPARE(menu.activeAction(), action);

    sendEnter(&container);

    QVERIFY2(menu.activeAction() == Q_NULLPTR, "the container did not reach the menu on the first enter");
}

void tst_RibbonContainers::leaveLetsTheNextEnterClearAgain()
{
    RibbonMenu menu;
    RibbonGridContainer container(&menu);

    sendEnter(&container);

    QAction *action = menu.addAction(QStringLiteral("action"));
    menu.setActiveAction(action);
    QCOMPARE(menu.activeAction(), action);

    QEvent leave(QEvent::Leave);
    QApplication::sendEvent(&container, &leave);

    sendEnter(&container);

    QVERIFY2(menu.activeAction() == Q_NULLPTR, "the container only clears the menu once, and that once was spent");
}

TEST_ADD(tst_RibbonContainers)

#include "tst_ribboncontainers.moc"
