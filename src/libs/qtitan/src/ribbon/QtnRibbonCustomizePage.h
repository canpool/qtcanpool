/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#ifndef QTN_RIBBONCUSTOMIZEPAGE_H
#define QTN_RIBBONCUSTOMIZEPAGE_H

#include "QtnRibbonCustomizeDialog.h"

#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE


class RibbonBar;
class RibbonQuickAccessBarCustomizePagePrivate;
/* RibbonQuickAccessBarCustomizePage */ 
class QTITAN_EXPORT RibbonQuickAccessBarCustomizePage : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonQuickAccessBarCustomizePage(RibbonBar* ribbonBar);
    virtual ~RibbonQuickAccessBarCustomizePage();
public:
    RibbonBar* ribbonBar() const; 
    void addCustomCategory(const QString& strCategory);
    void addSeparatorCategory(const QString& strCategory = QString());

public Q_SLOTS:
    void accepted();
protected:
    virtual void showEvent(QShowEvent* event);
private:
    QTN_DECLARE_PRIVATE(RibbonQuickAccessBarCustomizePage)
    Q_DISABLE_COPY(RibbonQuickAccessBarCustomizePage)
};

class RibbonBarCustomizePagePrivate;
/* RibbonBarCustomizePage */ 
class QTITAN_EXPORT RibbonBarCustomizePage : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonBarCustomizePage(RibbonBar* ribbonBar);
    virtual ~RibbonBarCustomizePage();
public:
    RibbonBar* ribbonBar() const; 
    void addCustomCategory(const QString& strCategory);
    void addSeparatorCategory(const QString& strCategory = QString());

public Q_SLOTS:
    void accepted();
    void rejected();
protected:
    virtual void showEvent(QShowEvent* event);
    virtual void hideEvent(QHideEvent* event);
private:
    QTN_DECLARE_PRIVATE(RibbonBarCustomizePage)
    Q_DISABLE_COPY(RibbonBarCustomizePage)
};


QTITAN_END_NAMESPACE

#endif // QTN_RIBBONCUSTOMIZEPAGE_H
