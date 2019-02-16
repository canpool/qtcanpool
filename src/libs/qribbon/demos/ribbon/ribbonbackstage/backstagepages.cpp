
#include <QApplication>
#include <QMenu>

#include <QtnOfficeDefines.h>
#include "backstagepages.h"


/* BackstagePageInfo */
BackstagePageInfo::BackstagePageInfo(QWidget* parent)
    : Qtitan::RibbonBackstagePage(parent)
{
    ui.setupUi(this);
    createActions();
    createMenus();
}

BackstagePageInfo::~BackstagePageInfo()
{
}

void BackstagePageInfo::createActions()
{
    m_actMark = new QAction(tr("Mark as &Final"), this);
    m_actMark->setStatusTip(tr("Let readers know the document is final and make it read-only."));
    m_actPassword = new QAction(QIcon(":/res/ProtectDocument.png"), tr("Encrypt with Password"), this);
    m_actMark->setStatusTip(tr("Requare a password to open this document"));

    m_actInspect = new QAction(tr("&Inspect Document"), this);
    m_actMark->setStatusTip(tr("Check the document for hidden properties or personal information."));
    m_actAccessibility = new QAction(tr("Check Accessibility"), this);
    m_actMark->setStatusTip(tr("Check the document for content that people with disabilities might find difficult to read."));

}

void BackstagePageInfo::createMenus()
{
    QMenu* menu = new QMenu(this);
    menu->addAction(m_actMark);
    menu->addAction(m_actPassword);
    ui.protectDocumentBut->setMenu(menu);

    menu = new QMenu(this);
    menu->addAction(m_actInspect);
    menu->addAction(m_actAccessibility);
    ui.checkIssuesBut->setMenu(menu);
}


/* BackstagePageSaveEmail */
BackstagePageSaveEmail::BackstagePageSaveEmail(QWidget* parent)
    : Qtitan::RibbonBackstagePage(parent)
{
    ui.setupUi(this);
}

BackstagePageSaveEmail::~BackstagePageSaveEmail()
{
}


/* BackstagePageSaveEmail */
BackstagePageSaveWeb::BackstagePageSaveWeb(QWidget* parent)
    : Qtitan::RibbonBackstagePage(parent)
{
    ui.setupUi(this);
}

BackstagePageSaveWeb::~BackstagePageSaveWeb()
{
}


/* BackstagePageSave */
BackstagePageSave::BackstagePageSave(QWidget* parent)
    : Qtitan::RibbonBackstagePage(parent)
{
    m_separator = new Qtitan::RibbonBackstageSeparator(this);
    m_separator->setOrientation(Qt::Vertical);

    ui.setupUi(this);

    ui.pageWrapper->setText("");

    m_pageSaveEmail = new BackstagePageSaveEmail(ui.pageWrapper);
    m_pageSaveWeb = new BackstagePageSaveWeb(ui.pageWrapper);
    m_pageSaveWeb->hide();

    QActionGroup* actGroup = new QActionGroup(this);

    m_actSendEmailButton = actGroup->addAction(ui.sendEmailButton->icon(), ui.sendEmailButton->text());
    m_actSendEmailButton->setCheckable(true);
    m_actSaveWebButton = actGroup->addAction(ui.saveWebButton->icon(), ui.saveWebButton->text());
    m_actSaveWebButton->setCheckable(true);
    m_actSharePointButton = actGroup->addAction(ui.sharePointButton->icon(), ui.sharePointButton->text());
    m_actSharePointButton->setCheckable(true);
    m_actBlogPostButton = actGroup->addAction(ui.blogPostButton->icon(), ui.blogPostButton->text());
    m_actBlogPostButton->setCheckable(true);
    m_actFileTypeButton = actGroup->addAction(ui.fileTypeButton->icon(), ui.fileTypeButton->text());
    m_actFileTypeButton->setCheckable(true);
    m_actCreateDocButton = actGroup->addAction(ui.createDocButton->icon(), ui.createDocButton->text());
    m_actCreateDocButton->setCheckable(true);

    ui.sendEmailButton->setDefaultAction(m_actSendEmailButton);
    ui.saveWebButton->setDefaultAction(m_actSaveWebButton);
    ui.sharePointButton->setDefaultAction(m_actSharePointButton);
    ui.blogPostButton->setDefaultAction(m_actBlogPostButton);
    ui.fileTypeButton->setDefaultAction(m_actFileTypeButton);
    ui.createDocButton->setDefaultAction(m_actCreateDocButton);

    m_actSendEmailButton->setChecked(true);
    connect(actGroup, SIGNAL(triggered(QAction*)), this, SLOT(triggeredPage(QAction*)));
}

BackstagePageSave::~BackstagePageSave()
{
}

void BackstagePageSave::triggeredPage(QAction* act)
{
    if (m_actSendEmailButton == act)
    {
        m_pageSaveWeb->hide();
        m_pageSaveEmail->show();
    }
    else if (m_actSaveWebButton == act)
    {
        m_pageSaveWeb->show();
        m_pageSaveEmail->hide();
    }
    else
    {
        m_pageSaveWeb->hide();
        m_pageSaveEmail->hide();
    }
}

void BackstagePageSave::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    m_pageSaveEmail->setGeometry(QRect(QPoint(0, 0), ui.pageWrapper->size()));
    m_pageSaveWeb->setGeometry(QRect(QPoint(0, 0), ui.pageWrapper->size()));

    QRect rc = ui.verticalLayoutTabs->geometry();
    m_separator->setGeometry(QRect(QPoint(rc.right() - 1, 0), QSize(3, size().height())));
}


/* BackstagePageSave */
BackstagePageAbout::BackstagePageAbout(QWidget* parent)
    : Qtitan::RibbonBackstagePage(parent)
{
    ui.setupUi(this);

    QString str = ui.labelVersion->text();
    if (!str.isEmpty())
    {
        QString strVersion(tr("Version:"));
        int index = str.indexOf(strVersion);
        if (index != -1)
            ui.labelVersion->setText(str.insert(index + strVersion.length(), 
                QLatin1String(" ") + QLatin1String(QTN_VERSION_RIBBON_STR)));
    }
}

BackstagePageAbout::~BackstagePageAbout()
{
}
