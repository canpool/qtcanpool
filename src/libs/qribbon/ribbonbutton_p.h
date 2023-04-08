/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include <QStyle>
#include <QToolButton>

QRIBBON_BEGIN_NAMESPACE

class RibbonButton : public QToolButton
{
    Q_OBJECT
public:
    RibbonButton(QWidget *parent = Q_NULL);
    virtual ~RibbonButton();
public:
    virtual QSize sizeHint() const;
protected:
    virtual bool event(QEvent *event);
    virtual void actionEvent(QActionEvent *event);
private:
    QSize m_sizeHint;
    Qt::ToolButtonStyle m_saveButtonStyle;
    Qt::ArrowType m_saveArrowType;
private:
    Q_DISABLE_COPY(RibbonButton)
};

/* RibbonTitleButton */
class RibbonTitleButton : public QToolButton
{
    Q_OBJECT
public:
    RibbonTitleButton(QWidget *parent, QStyle::SubControl subControl);
    virtual ~RibbonTitleButton();
public:
    QStyle::SubControl subControl() const;
protected:
    virtual void paintEvent(QPaintEvent *);
private:
    QStyle::SubControl m_subControl;
private:
    Q_DISABLE_COPY(RibbonTitleButton)
};

QRIBBON_END_NAMESPACE
