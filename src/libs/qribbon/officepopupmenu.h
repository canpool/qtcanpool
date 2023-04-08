/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include <QMenu>

QRIBBON_BEGIN_NAMESPACE

class OfficePopupMenuPrivate;

class QRIBBON_EXPORT OfficePopupMenu : public QMenu
{
    Q_OBJECT
    Q_PROPERTY(bool gripVisible READ isGripVisible WRITE setGripVisible)
public:
    OfficePopupMenu(QWidget *parent = Q_NULL);
    virtual ~OfficePopupMenu();

    static OfficePopupMenu *createPopupMenu(QWidget *parent = Q_NULL);
public:
    QAction *addWidget(QWidget *widget);

    void setGripVisible(bool visible);
    bool isGripVisible() const;
public:
    virtual QSize sizeHint() const;
protected:
    void setWidgetBar(QWidget *widget);

private Q_SLOTS:
    void aboutToShowBar();
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void moveEvent(QMoveEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
private:
    friend class RibbonGalleryPrivate;
    QTC_DECLARE_PRIVATE(OfficePopupMenu)
    Q_DISABLE_COPY(OfficePopupMenu)
};

QRIBBON_END_NAMESPACE
