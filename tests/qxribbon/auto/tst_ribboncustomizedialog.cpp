/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "tst_probe.h"

#include "qxribbon/ribbonbutton.h"
#include "qxribbon/ribboncustomizedialog.h"
#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonactionsmanager.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonpage.h"

#include <QAction>
#include <QModelIndex>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTemporaryDir>
#include <QToolButton>
#include <QTreeView>
#include <QXmlStreamWriter>

QX_RIBBON_USE_NAMESPACE

static QString dialogToXml(RibbonCustomizeDialog &dlg)
{
    QByteArray str;
    QXmlStreamWriter xml(&str);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    xml.writeStartDocument();
    dlg.toXml(&xml);
    xml.writeEndDocument();
    return QString::fromUtf8(str);
}

/**
 * @brief Builds a Ribbon with "one page, one group, two actions", selects the selectedRow-th action, clicks a move
 * button, and returns the recorded XML
 * @param selectedRow row index of the selected action (0 means the first action)
 * @param buttonName object name of the move button; pushButtonUp means "move up", pushButtonDown means "move down"
 * @return the recorded customization XML; if the button is unavailable, returns a readable sentinel string
 * @note RibbonCustomizeWidget is not exported, so it can only be driven through RibbonCustomizeDialog and the object
 * names of its child widgets
 */
static QString recordMove(int selectedRow, const char *buttonName)
{
    RibbonBar rb;

    RibbonPage *page = rb.addPage(QStringLiteral("page1"));
    RibbonGroup *grp = page->addGroup(QStringLiteral("group1"));
    QAction *a1 = new QAction(QStringLiteral("action1"), &rb);
    QAction *a2 = new QAction(QStringLiteral("action2"), &rb);
    a1->setObjectName(QStringLiteral("action1"));
    a2->setObjectName(QStringLiteral("action2"));
    grp->addAction(a1, RibbonGroup::Small);
    grp->addAction(a2, RibbonGroup::Small);

    RibbonActionsManager mgr(&rb);
    mgr.registerAction(a1, RibbonActionsManager::CommonlyUsedActionTag);
    mgr.registerAction(a2, RibbonActionsManager::CommonlyUsedActionTag);

    RibbonCustomizeDialog dlg(&rb);
    dlg.setupActionsManager(&mgr);

    // The dialog hosts a RibbonCustomizeWidget, which is not exported, so it is
    // reached through the object names it installs on its children.
    QWidget *custom = dlg.findChild<QWidget *>(QStringLiteral("customWidget"));
    if (custom == Q_NULLPTR) {
        return QStringLiteral("<no customWidget>");
    }
    QTreeView *tree = custom->findChild<QTreeView *>(QStringLiteral("treeViewResult"));
    QToolButton *button = custom->findChild<QToolButton *>(QString::fromLatin1(buttonName));
    if ((tree == Q_NULLPTR) || (button == Q_NULLPTR)) {
        return QStringLiteral("<no tree or button>");
    }
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(tree->model());
    if ((model == Q_NULLPTR) || (model->rowCount() != 1)) {
        return QStringLiteral("<unexpected model>");
    }
    QStandardItem *groupItem = model->item(0)->child(0);
    if ((groupItem == Q_NULLPTR) || (groupItem->rowCount() != 2)) {
        return QStringLiteral("<unexpected group>");
    }

    // Select the action and tell the tree it was clicked - that is what enables
    // the move buttons in the real dialog.
    const QModelIndex index = groupItem->child(selectedRow)->index();
    tree->setCurrentIndex(index);
    if (!QMetaObject::invokeMethod(tree, "clicked", Q_ARG(QModelIndex, index))) {
        return QStringLiteral("<clicked not delivered>");
    }
    if (!button->isEnabled()) {
        return QStringLiteral("<button disabled>");
    }
    button->click();

    return dialogToXml(dlg);
}

/*! One page, one group, two small actions, both registered - the smallest
 *  ribbon an arrangement can be edited in. */
struct Scene {
    RibbonBar bar;
    RibbonActionsManager manager;
    QAction *first = nullptr;
    QAction *second = nullptr;

    Scene()
        : manager(&bar)
    {
        RibbonPage *page = bar.addPage(QStringLiteral("page1"));
        RibbonGroup *group = page->addGroup(QStringLiteral("group1"));
        first = new QAction(QStringLiteral("action1"), &bar);
        second = new QAction(QStringLiteral("action2"), &bar);
        first->setObjectName(QStringLiteral("action1"));
        second->setObjectName(QStringLiteral("action2"));
        group->addAction(first, RibbonGroup::Small);
        group->addAction(second, RibbonGroup::Small);
        manager.registerAction(first, RibbonActionsManager::CommonlyUsedActionTag);
        manager.registerAction(second, RibbonActionsManager::CommonlyUsedActionTag);
    }

    RibbonGroup *group() const
    {
        return bar.findChild<RibbonGroup *>();
    }
};

/*! The order the two actions are *laid out* in, read off the buttons rather
 *  than off the group's list of children: a group keeps its buttons in the
 *  order they were made, and it is their positions that move.
 *
 *  Small actions are stacked in a group rather than stood side by side, so the
 *  first one is the higher one - a row before a column. */
QString orderOf(const Scene &scene)
{
    RibbonGroup *group = scene.group();
    RibbonButton *firstButton = group->ribbonButtonForAction(scene.first);
    RibbonButton *secondButton = group->ribbonButtonForAction(scene.second);
    if (!firstButton || !secondButton) {
        return QStringLiteral("<a button is missing>");
    }
    const QPoint first = firstButton->pos();
    const QPoint second = secondButton->pos();
    if (first == second) {
        return QStringLiteral("<the two buttons are on top of each other>");
    }
    const bool firstIsEarlier = (first.y() != second.y()) ? (first.y() < second.y()) : (first.x() < second.x());
    return firstIsEarlier ? QStringLiteral("action1,action2") : QStringLiteral("action2,action1");
}

/*! Moves the second action up one place, which is the edit the dialog's own
 *  tests already know how to make. */
void moveSecondActionUp(RibbonCustomizeDialog &dlg)
{
    QWidget *custom = dlg.findChild<QWidget *>(QStringLiteral("customWidget"));
    QTreeView *tree = custom->findChild<QTreeView *>(QStringLiteral("treeViewResult"));
    QToolButton *button = custom->findChild<QToolButton *>(QStringLiteral("pushButtonUp"));
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(tree->model());
    QStandardItem *groupItem = model->item(0)->child(0);
    const QModelIndex index = groupItem->child(1)->index();
    tree->setCurrentIndex(index);
    QMetaObject::invokeMethod(tree, "clicked", Q_ARG(QModelIndex, index));
    button->click();
}

class tst_RibbonCustomizeDialog : public QObject
{
    Q_OBJECT
public:
    QAction *createAction(const QString &text, QObject *parent = nullptr);
private slots:
    void actionsManager();
    void page();
    void moveAction();

    void anEditedArrangementIsAppliedToTheBarAndComesBackFromAFile();
};

QAction *tst_RibbonCustomizeDialog::createAction(const QString &text, QObject *parent)
{
    QAction *a = new QAction(text, parent);
    a->setObjectName(text);
    return a;
}

void tst_RibbonCustomizeDialog::actionsManager()
{
    RibbonBar rb;

    RibbonCustomizeDialog dlg(&rb);

    RibbonActionsManager mgr(nullptr);

    QAction *a1 = new QAction(tr("action1"));
    QAction *a2 = new QAction(tr("action2"));

    QCOMPARE(mgr.registerAction(a1, RibbonActionsManager::CommonlyUsedActionTag), true);
    QCOMPARE(mgr.registerAction(a2, RibbonActionsManager::NotInRibbonPageTag, "key1"), true);
    dlg.setupActionsManager(&mgr);

    QByteArray str;
    QXmlStreamWriter xml(&str);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)   // QXmlStreamWriter always encodes XML in UTF-8.
    xml.setCodec("utf-8");
#endif
    xml.writeStartDocument();
    bool isOk = dlg.toXml(&xml);
    xml.writeEndDocument();

    QCOMPARE(isOk, false);   // dialog can not customize widget
}

void tst_RibbonCustomizeDialog::page()
{
    RibbonBar rb;

    RibbonPage *page1 = rb.addPage(tr("page1"));
    RibbonGroup *g1 = page1->addGroup(tr("group1"));
    QAction *a1 = createAction(tr("action1"));
    g1->addAction(a1, RibbonGroup::Auto);
    RibbonGroup *g2 = page1->addGroup(tr("group2"));
    QAction *a2 = createAction(tr("action2"));
    g2->addAction(a2, RibbonGroup::Large);

    RibbonCustomizeDialog dlg(&rb);

    QByteArray str;
    QXmlStreamWriter xml(&str);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)   // QXmlStreamWriter always encodes XML in UTF-8.
    xml.setCodec("utf-8");
#endif
    xml.writeStartDocument();
    bool isOk = dlg.toXml(&xml);
    xml.writeEndDocument();

    QCOMPARE(isOk, false);   // dialog can not customize widget
}

void tst_RibbonCustomizeDialog::moveAction()
{
    // Moving an action down is recorded as +1; recording -1 would mean "move up"
    // and the change would be applied in the opposite direction.
    const QString down = recordMove(0, "pushButtonDown");
    QVERIFY2(down.contains(QStringLiteral("type=\"9\"")), qPrintable(down));   // ChangeActionOrderActionType
    QVERIFY2(down.contains(QStringLiteral("index=\"1\"")), qPrintable(down));

    // The mirror case: moving the last action up is recorded as -1.
    const QString up = recordMove(1, "pushButtonUp");
    QVERIFY2(up.contains(QStringLiteral("type=\"9\"")), qPrintable(up));
    QVERIFY2(up.contains(QStringLiteral("index=\"-1\"")), qPrintable(up));
}

/*!
 * Customization is a flow and not a call: an arrangement is edited, applied to
 * the bar that is standing there, written down, and read back by a window that
 * has never seen the edit. Each of those steps is covered on its own - the
 * dialog records a move, the XML carries the number - and what was never
 * covered is that they add up to a ribbon that looks the way the user left it.
 *
 * So this is the whole flow in one case, and the thing it reads at every step
 * is the order the two actions are laid out in, which is the only record of the
 * arrangement a user can see.
 */
void tst_RibbonCustomizeDialog::anEditedArrangementIsAppliedToTheBarAndComesBackFromAFile()
{
    Scene scene;
    scene.bar.show();
    QVERIFY(scene.group());
    QCOMPARE(orderOf(scene), QStringLiteral("action1,action2"));

    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString path = dir.filePath(QStringLiteral("customize.xml"));

    RibbonCustomizeDialog dlg(&scene.bar);
    dlg.setupActionsManager(&scene.manager);
    moveSecondActionUp(dlg);

    // Applied: the bar that is standing there has been rearranged. A layout has
    // to run before the buttons are anywhere new, so the arrangement is read
    // after one.
    QVERIFY(dlg.apply());
    Probe::settle(&scene.bar);
    QCOMPARE(orderOf(scene), QStringLiteral("action2,action1"));

    // Written down.
    QVERIFY(dlg.toXml(path));

    // And read back by a window that never saw the edit: same names, same
    // arrangement. This is the step a user meets as "my ribbon is how I left
    // it" on the next launch - and it is not the dialog's fromXml(), which only
    // tells a dialog what was already there; the door that applies a file is
    // this one.
    Scene second;
    // The window has to be up for the group to have laid its buttons out at
    // all: an arrangement is a set of positions, and a widget that was never
    // shown has none.
    second.bar.show();
    QVERIFY(QxRibbonCustomizeApplyFromXmlFile(path, &second.bar, &second.manager));
    Probe::settle(&second.bar);
    QCOMPARE(orderOf(second), QStringLiteral("action2,action1"));
}

TEST_ADD(tst_RibbonCustomizeDialog)

#include "tst_ribboncustomizedialog.moc"
