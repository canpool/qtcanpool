/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include <QDialog>

class QAbstractButton;

QRIBBON_BEGIN_NAMESPACE

class RibbonBar;
class RibbonCustomizeDialogPrivate;

/* RibbonCustomizeDialog */
class QRIBBON_EXPORT RibbonCustomizeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RibbonCustomizeDialog(QWidget *parent = Q_NULL);
    virtual ~RibbonCustomizeDialog();
public:
    void addPage(QWidget *page);
    void insertPage(int index, QWidget *page);

    int indexOf(QWidget *page) const;

    QWidget *currentPage() const;
    void setCurrentPage(QWidget *page);

    int currentPageIndex() const;
    void setCurrentPageIndex(int index);

    int pageCount() const;
    QWidget *pageByIndex(int index) const;
public:
    virtual void accept();
    virtual void reject();
protected:
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);
private:
    QRN_DECLARE_PRIVATE(RibbonCustomizeDialog)
    Q_DISABLE_COPY(RibbonCustomizeDialog)
};

QRIBBON_END_NAMESPACE
