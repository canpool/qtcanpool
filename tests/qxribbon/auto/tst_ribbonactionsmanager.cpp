#include "tst_global.h"

#include "qxribbon/ribbonactionsmanager.h"
#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonquickaccessbar.h"
#include "qxribbon/ribbonbuttongroup.h"

#include <QAction>

QX_RIBBON_USE_NAMESPACE

class tst_RibbonActionsManager : public QObject
{
    Q_OBJECT
public:
    QAction *createAction(const QString &text, QObject *parent = nullptr);
private slots:
    void tag();
    void action();
    void autoRegister();
};

QAction *tst_RibbonActionsManager::createAction(const QString &text, QObject *parent)
{
    QAction *a = new QAction(text, parent);
    a->setObjectName(text);
    return a;
}

void tst_RibbonActionsManager::tag()
{
    RibbonBar rb;

    RibbonActionsManager mgr(&rb);
    QCOMPARE(mgr.ribbonBar(), &rb);

    mgr.setTagName(RibbonActionsManager::UnknowActionTag, tr("tag1"));
    mgr.setTagName(RibbonActionsManager::CommonlyUsedActionTag, tr("tag2"));
    mgr.setTagName(RibbonActionsManager::NotInFunctionalAreaActionTag, tr("tag3"));
    mgr.setTagName(RibbonActionsManager::AutoPageDistinguishBeginTag, tr("tag4"));
    mgr.setTagName(RibbonActionsManager::AutoPageDistinguishEndTag, tr("tag5"));
    mgr.setTagName(RibbonActionsManager::NotInRibbonPageTag, tr("tag6"));

    QCOMPARE(mgr.tagName(RibbonActionsManager::UnknowActionTag), tr("tag1"));
    QCOMPARE(mgr.tagName(RibbonActionsManager::NotInRibbonPageTag), tr("tag6"));

    mgr.removeTag(RibbonActionsManager::NotInFunctionalAreaActionTag);

    QCOMPARE(mgr.tagName(RibbonActionsManager::NotInFunctionalAreaActionTag).isEmpty(), true);
}

void tst_RibbonActionsManager::action()
{
    RibbonActionsManager mgr(nullptr);

    QAction *a1 = new QAction(tr("action1"));
    QAction *a2 = new QAction(tr("action2"));
    QAction *a3 = new QAction(tr("action3"));
    QAction *a4 = new QAction(tr("action4"));
    QAction *a5 = new QAction(tr("action5"));
    QAction *a6 = new QAction(tr("action6"));
    a6->setObjectName("key6");

    QCOMPARE(mgr.registerAction(a1, RibbonActionsManager::CommonlyUsedActionTag), true);
    QCOMPARE(mgr.registerAction(a2, RibbonActionsManager::NotInRibbonPageTag, "key1"), true);
    QCOMPARE(mgr.registerAction(a3, RibbonActionsManager::NotInRibbonPageTag, "key1"), false);
    QCOMPARE(mgr.registerAction(a3, RibbonActionsManager::NotInRibbonPageTag, "key2"), true);
    QCOMPARE(mgr.registerAction(a4, RibbonActionsManager::UnknowActionTag), true);
    QCOMPARE(mgr.registerAction(a5, RibbonActionsManager::NotInFunctionalAreaActionTag), true);
    QCOMPARE(mgr.registerAction(a6, RibbonActionsManager::UserDefineActionTag + 1), true);

    QCOMPARE(mgr.actionTags().count(), 5);
    QCOMPARE(mgr.count(), 6);
    QCOMPARE(mgr.count(), mgr.allActions().count());

    QString s = "";
    for (QAction *a : mgr.actions(RibbonActionsManager::NotInRibbonPageTag)) {
        s += a->text() + ",";
    }
    QCOMPARE("action2,action3,", s);

    QCOMPARE(mgr.actions(RibbonActionsManager::UnknowActionTag).count(), 1);
    mgr.unregisterAction(a4);
    QCOMPARE(mgr.actions(RibbonActionsManager::UnknowActionTag).count(), 0);
    QCOMPARE(mgr.actionTags().count(), 4);
    QCOMPARE(mgr.count(), 5);

    QList<QAction *> acts = mgr.search(tr("action1"));
    QCOMPARE(acts.contains(a1), true);
    QCOMPARE(acts.count(), 1);

    QCOMPARE(mgr.key(a2), "key1");
    QCOMPARE(mgr.action("key1"), a2);

    QCOMPARE(mgr.key(a1), "id_0_");
    QCOMPARE(mgr.key(a6), "id_3_key6");
}

void tst_RibbonActionsManager::autoRegister()
{
    RibbonBar rb;

    RibbonQuickAccessBar *qab = rb.quickAccessBar();
    QAction *a1 = createAction(tr("action1"), &rb);
    QAction *a2 = createAction(tr("action2"));
    qab->addAction(a1);
    qab->addAction(a2);

    RibbonButtonGroup *bg = rb.cornerButtonGroup(Qt::TopLeftCorner);
    QAction *a3 = createAction(tr("action3"), &rb);
    QAction *a4 = createAction(tr("action4"));
    bg->addAction(a3);
    bg->addAction(a4);

    RibbonPage *page1 = rb.addPage(tr("page1"));
    RibbonGroup *g1 = page1->addGroup(tr("group1"));
    QAction *a5 = createAction(tr("action5"));
    QAction *a6 = createAction(tr("action6"));
    g1->addAction(a5, RibbonGroup::Auto);
    g1->addAction(a6, RibbonGroup::Auto);
    RibbonGroup *g2 = page1->addGroup(tr("group2"));
    QAction *a7 = createAction(tr("action7"));
    QAction *a8 = createAction(tr("action8"));
    g2->addAction(a7, RibbonGroup::Large);
    g2->addAction(a8, RibbonGroup::Large);

    RibbonPage *page2 = rb.addPage(tr("page2"));
    RibbonGroup *g3 = page2->addGroup(tr("group3"));
    QAction *a9 = createAction(tr("action9"));
    QAction *a10 = createAction(tr("action10"));
    g3->addAction(a9, RibbonGroup::Medium);
    g3->addAction(a10, RibbonGroup::Medium);
    RibbonGroup *g4 = page2->addGroup(tr("group4"));
    QAction *a11 = createAction(tr("action11"));
    QAction *a12 = createAction(tr("action12"));
    g4->addAction(a11, RibbonGroup::Small);
    g4->addAction(a12, RibbonGroup::Small);

    RibbonActionsManager mgr(&rb);

    QList<int> tags = {
        RibbonActionsManager::AutoPageDistinguishBeginTag,
        RibbonActionsManager::AutoPageDistinguishBeginTag + 1,
        RibbonActionsManager::NotInRibbonPageTag
    };
    QCOMPARE(mgr.actionTags(), tags);

    QCOMPARE(mgr.tagName(RibbonActionsManager::AutoPageDistinguishBeginTag), tr("page1"));
    QCOMPARE(mgr.tagName(RibbonActionsManager::AutoPageDistinguishBeginTag + 1), tr("page2"));
    QCOMPARE(mgr.tagName(RibbonActionsManager::NotInRibbonPageTag), tr("not in ribbon"));

    QCOMPARE(mgr.actions(RibbonActionsManager::AutoPageDistinguishBeginTag).count(), 4);
    QCOMPARE(mgr.actions(RibbonActionsManager::AutoPageDistinguishBeginTag + 1).count(), 4);
    QCOMPARE(mgr.actions(RibbonActionsManager::NotInRibbonPageTag).count(), 2);

    QCOMPARE(mgr.count(), 10); // not 12

    QCOMPARE(mgr.key(a1), "action1");
    QCOMPARE(mgr.key(a2), "");
    QCOMPARE(mgr.key(a3), "action3");
    QCOMPARE(mgr.key(a4), "");
    QCOMPARE(mgr.key(a5), "action5");
    QCOMPARE(mgr.key(a6), "action6");
    QCOMPARE(mgr.key(a7), "action7");
    QCOMPARE(mgr.key(a8), "action8");
    QCOMPARE(mgr.key(a9), "action9");
    QCOMPARE(mgr.key(a10), "action10");
    QCOMPARE(mgr.key(a11), "action11");
    QCOMPARE(mgr.key(a12), "action12");

    mgr.unregisterAction(a2); // invalid
    QCOMPARE(mgr.count(), 10);
    mgr.unregisterAction(a12);
    QCOMPARE(mgr.key(a12), "");
    QCOMPARE(mgr.count(), 9);
}

TEST_ADD(tst_RibbonActionsManager)

#include "tst_ribbonactionsmanager.moc"
