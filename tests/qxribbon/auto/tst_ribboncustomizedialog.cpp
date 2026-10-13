#include "tst_global.h"

#include "qxribbon/ribboncustomizedialog.h"
#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonactionsmanager.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonpage.h"

#include <QAction>
#include <QModelIndex>
#include <QStandardItem>
#include <QStandardItemModel>
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
 * @brief 构建一个“一页一组两动作”的 ribbon，选中第 selectedRow 个动作并点击某个移动按钮，返回记录下来的xml
 * @param selectedRow 选中的动作行号(0表示第一个动作)
 * @param buttonName 移动按钮的对象名，pushButtonUp 为“上移”，pushButtonDown 为“下移”
 * @return 记录的自定义数据xml；如果按钮不可用，返回一个可读的哨兵字符串
 * @note RibbonCustomizeWidget 未导出，只能透过 RibbonCustomizeDialog 及其子控件对象名来驱动
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

class tst_RibbonCustomizeDialog : public QObject
{
    Q_OBJECT
public:
    QAction *createAction(const QString &text, QObject *parent = nullptr);
private slots:
    void actionsManager();
    void page();
    void moveAction();
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

TEST_ADD(tst_RibbonCustomizeDialog)

#include "tst_ribboncustomizedialog.moc"
