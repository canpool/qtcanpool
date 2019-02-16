#ifndef BACKSTAGEPAGES_H
#define BACKSTAGEPAGES_H

#include <QtnRibbonBackstageView.h>

#include "ui_infoPage.h"
#include "ui_savepage.h"
#include "ui_helppage.h"
#include "ui_saveemailpage.h"
#include "ui_savewebpage.h"

/* BackstagePageInfo */
class BackstagePageInfo : public Qtitan::RibbonBackstagePage
{
public:
    BackstagePageInfo(QWidget* parent);
    virtual ~BackstagePageInfo();

protected:
    void createActions();
    void createMenus();

private:
    QAction* m_actMark;
    QAction* m_actPassword;

    QAction* m_actInspect;
    QAction* m_actAccessibility;

    Ui::InfoPage ui;
};


/* BackstagePageSaveEmail */
class BackstagePageSaveEmail : public Qtitan::RibbonBackstagePage
{
public:
    BackstagePageSaveEmail(QWidget* parent);
    virtual ~BackstagePageSaveEmail();

private:
    Ui::SaveEmailPage ui;
};


/* BackstagePageSaveWeb */
class BackstagePageSaveWeb : public Qtitan::RibbonBackstagePage
{
public:
    BackstagePageSaveWeb(QWidget* parent);
    virtual ~BackstagePageSaveWeb();

private:
    Ui::SaveWebPage ui;
};


/* BackstagePageInfo */
class BackstagePageSave : public Qtitan::RibbonBackstagePage
{
    Q_OBJECT
public:
    BackstagePageSave(QWidget* parent);
    virtual ~BackstagePageSave();

protected slots:
    void triggeredPage(QAction* act);

protected:
    virtual void resizeEvent(QResizeEvent* event);

protected:
    Qtitan::RibbonBackstageSeparator* m_separator;

    QAction* m_actSendEmailButton;
    QAction* m_actSaveWebButton;
    QAction* m_actSharePointButton;
    QAction* m_actBlogPostButton;

    QAction* m_actFileTypeButton;
    QAction* m_actCreateDocButton;

    BackstagePageSaveEmail* m_pageSaveEmail;
    BackstagePageSaveWeb* m_pageSaveWeb;

private:
    Ui::SaveSendPage ui;
};

/* BackstagePageAbout */
class BackstagePageAbout : public Qtitan::RibbonBackstagePage
{
public:
    BackstagePageAbout(QWidget* parent);
    virtual ~BackstagePageAbout();

protected:

private:
    Ui::AboutPage ui;
};


#endif // BACKSTAGEPAGES_H
