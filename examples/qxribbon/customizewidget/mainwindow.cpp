#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribboncustomizedialog.h"
#include "qxribbon/ribbonactionsmanager.h"

#include <QAction>
#include <QToolBar>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QDebug>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonWindow(parent)
{
    QIcon icon(":/logo");

    RibbonBar *rb = ribbonBar();

    RibbonPage *p1 = rb->addPage(tr("page1"));
    RibbonGroup *g1 = p1->addGroup(tr("group1"));
    g1->addLargeAction(new QAction(icon, tr("La"), this));
    g1->addLargeAction(new QAction(icon, tr("Lala lalalala"), this));
    g1->addMediumAction(new QAction(icon, tr("Ma1"), this));
    g1->addMediumAction(new QAction(icon, tr("Maaaaaaa2"), this));
    g1->addSmallAction(new QAction(tr("Sa11111111"), this));
    g1->addSmallAction(new QAction(tr("Sa2222"), this));
    g1->addSmallAction(new QAction(tr("Sa3333333"), this));

    rb->addPage(tr("loooooooooooongpage2"));

    RibbonPageContext *pc1 = rb->addPageContext(tr("context1"));
    pc1->addPage(tr("page1"));
    pc1->addPage(tr("loooooooooooongpage2"));
    rb->showPageContext(pc1);
    RibbonPageContext *pc2 = rb->addPageContext(tr("looooooooooongcontext2"));
    pc2->addPage(tr("page1"));
    pc2->addPage(tr("loooooooooooongpage2"));
    rb->showPageContext(pc2);

    createActionsManager();

    QToolBar *tb = addToolBar(tr("customize"));
    QAction *a = tb->addAction(tr("customzie"));
    connect(a, &QAction::triggered, this, &MainWindow::onActionCustomizeTriggered);

    resize(800, 400);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onActionCustomizeTriggered()
{
    RibbonCustomizeDialog dlg(this->ribbonBar(), this);
    dlg.setupActionsManager(m_actMgr);
    dlg.fromXml("customize.xml");
    if (RibbonCustomizeDialog::Accepted == dlg.exec()) {
        dlg.apply();
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
        if (isOk) {
            QFile f("customize.xml");
            if (f.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
                QTextStream s(&f);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)   // QTextStream always encodes XML in UTF-8.
                s.setCodec("utf-8");
#endif
                s << str;
                s.flush();
            }
            qDebug() << "write xml:";
            qDebug() << QString::fromLatin1(str);
        }
    }
}

void MainWindow::createActionsManager()
{
    RibbonBar *bar = this->ribbonBar();

    QAction *acttext1 = new QAction(tr("text action1"), bar);
    QAction *acttext2 = new QAction(tr("text action2"), bar);
    QAction *acttext3 = new QAction(tr("text action3"), bar);
    QAction *acttext4 = new QAction(tr("text action4"), bar);
    QAction *acttext5 = new QAction(tr("text action5"), bar);

    QAction *actIcon1 = new QAction(QIcon(":/logo"), tr("action with icon"), bar);

    m_actionTagText = RibbonActionsManager::UserDefineActionTag + 1;
    m_actionTagWithIcon = RibbonActionsManager::UserDefineActionTag + 2;

    m_actMgr = new RibbonActionsManager(bar);

    m_actMgr->registerAction(acttext1, RibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registerAction(acttext3, RibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registerAction(acttext5, RibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registerAction(actIcon1, RibbonActionsManager::CommonlyUsedActionTag);

    m_actMgr->registerAction(acttext1, m_actionTagText);
    m_actMgr->registerAction(acttext2, m_actionTagText);
    m_actMgr->registerAction(acttext3, m_actionTagText);
    m_actMgr->registerAction(acttext4, m_actionTagText);
    m_actMgr->registerAction(acttext5, m_actionTagText);

    m_actMgr->registerAction(actIcon1, m_actionTagWithIcon);

    m_actMgr->setTagName(RibbonActionsManager::CommonlyUsedActionTag, tr("in common use"));
    m_actMgr->setTagName(m_actionTagText, tr("no icon action"));
    m_actMgr->setTagName(m_actionTagWithIcon, tr("have icon action"));
}
