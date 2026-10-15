#include "tst_global.h"

#include "qxribbon/ribbonquickaccessbar.h"

#include <QAction>
#include <QMenu>

QX_RIBBON_USE_NAMESPACE

class tst_RibbonQuickAccessBar : public QObject
{
    Q_OBJECT
private slots:
    void visibleCount();
    void addAction();
    void customizeDialogEntry();
};

void tst_RibbonQuickAccessBar::visibleCount()
{
    RibbonQuickAccessBar qab;
    QVERIFY(qab.visibleCount() == 0);
}

void tst_RibbonQuickAccessBar::addAction()
{
    RibbonQuickAccessBar qab;

    {
        QAction action(0);

        QCOMPARE(qab.actions().count(), 1);   // actionCustomizeButton
        qab.addAction(&action);
        QCOMPARE(qab.actions().count(), 2);
        QCOMPARE(qab.actions()[0], &action);
        QCOMPARE(qab.visibleCount(), 1);

        qab.clear();
        QCOMPARE(qab.actions().count(), 1);
        QCOMPARE(qab.visibleCount(), 0);
    }
}

void tst_RibbonQuickAccessBar::customizeDialogEntry()
{
    RibbonQuickAccessBar qab;
    QMenu *menu = qab.findChild<QMenu *>();
    QVERIFY(menu);

    QSignalSpy spy(&qab, &RibbonQuickAccessBar::customizeDialogRequested);

    // The menu is rebuilt on every show, and the customization entry is appended last,
    // behind a separator - the entry the application wires to its customize dialog.
    menu->popup(QPoint(0, 0));
    QAction *entry = nullptr;
    for (QAction *action : menu->actions()) {
        if (action->objectName() == QStringLiteral("qx_RibbonCustomizeQabAction")) {
            entry = action;
        }
    }
    QVERIFY(entry);
    QCOMPARE(menu->actions().last(), entry);
    QCOMPARE(entry->text(), QStringLiteral("Customize Quick Access Toolbar..."));

    entry->trigger();
    QCOMPARE(spy.count(), 1);
}

TEST_ADD(tst_RibbonQuickAccessBar)

#include "tst_ribbonquickaccessbar.moc"
