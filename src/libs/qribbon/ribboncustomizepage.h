/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include "ribboncustomizedialog.h"

QRIBBON_BEGIN_NAMESPACE

class RibbonBar;
class RibbonQuickAccessBarCustomizePagePrivate;
/* RibbonQuickAccessBarCustomizePage */
class QRIBBON_EXPORT RibbonQuickAccessBarCustomizePage : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonQuickAccessBarCustomizePage(RibbonBar *ribbonBar);
    virtual ~RibbonQuickAccessBarCustomizePage();
public:
    RibbonBar *ribbonBar() const;
    void addCustomCategory(const QString &strCategory);
    void addSeparatorCategory(const QString &strCategory = QString());

public Q_SLOTS:
    void accepted();
protected:
    virtual void showEvent(QShowEvent *event);
private:
    QTC_DECLARE_PRIVATE(RibbonQuickAccessBarCustomizePage)
    Q_DISABLE_COPY(RibbonQuickAccessBarCustomizePage)
};

class RibbonBarCustomizePagePrivate;
/* RibbonBarCustomizePage */
class QRIBBON_EXPORT RibbonBarCustomizePage : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonBarCustomizePage(RibbonBar *ribbonBar);
    virtual ~RibbonBarCustomizePage();
public:
    RibbonBar *ribbonBar() const;
    void addCustomCategory(const QString &strCategory);
    void addSeparatorCategory(const QString &strCategory = QString());

public Q_SLOTS:
    void accepted();
    void rejected();
protected:
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);
private:
    QTC_DECLARE_PRIVATE(RibbonBarCustomizePage)
    Q_DISABLE_COPY(RibbonBarCustomizePage)
};

QRIBBON_END_NAMESPACE
