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
#ifndef QTN_OFFICEOPTIONSDIALOG_H
#define QTN_OFFICEOPTIONSDIALOG_H

#include <QDialog>
#include "QtitanDef.h"

class QAbstractButton;

QTITAN_BEGIN_NAMESPACE

class RibbonBar;
class RibbonCustomizeDialogPrivate;

/* RibbonCustomizeDialog */
class QTITAN_EXPORT RibbonCustomizeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RibbonCustomizeDialog(QWidget* parent = Q_NULL);
    virtual ~RibbonCustomizeDialog();

public:
    void addPage(QWidget* page);
    void insertPage(int index, QWidget* page);

    int indexOf(QWidget* page) const;

    QWidget* currentPage() const;
    void setCurrentPage(QWidget* page);

    int currentPageIndex() const;
    void setCurrentPageIndex(int index);

    int pageCount() const;
    QWidget* pageByIndex(int index) const;

public:
    virtual void accept();
    virtual void reject();

protected:
    virtual void showEvent(QShowEvent* event);
    virtual void hideEvent(QHideEvent* event);

private:
    QTN_DECLARE_PRIVATE(RibbonCustomizeDialog)
    Q_DISABLE_COPY(RibbonCustomizeDialog)
};

QTITAN_END_NAMESPACE

#endif // QTN_OFFICEOPTIONSDIALOG_H
