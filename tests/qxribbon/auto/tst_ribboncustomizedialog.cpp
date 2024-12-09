#include "tst_global.h"

#include "qxribbon/ribboncustomizedialog.h"
#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonactionsmanager.h"

#include <QAction>

QX_RIBBON_USE_NAMESPACE

class tst_RibbonCustomizeDialog : public QObject
{
    Q_OBJECT
public:
    QAction *createAction(const QString &text, QObject *parent = nullptr);
private slots:
    void actionsManager();
    void page();
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

    dlg.apply();

    QByteArray str;
    QXmlStreamWriter xml(&str);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    xml.setCodec("utf-8");
    xml.writeStartDocument();
    bool isOk = dlg.toXml(&xml);
    xml.writeEndDocument();

    QCOMPARE(isOk, false);  // dialog can not customize widget
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

    dlg.apply();

    QByteArray str;
    QXmlStreamWriter xml(&str);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    xml.setCodec("utf-8");
    xml.writeStartDocument();
    bool isOk = dlg.toXml(&xml);
    xml.writeEndDocument();

    QCOMPARE(isOk, false);  // dialog can not customize widget
}

TEST_ADD(tst_RibbonCustomizeDialog)

#include "tst_ribboncustomizedialog.moc"
