/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include <QToolButton>

class QStyleOption;

QRIBBON_BEGIN_NAMESPACE
/* PopupColorButton */
class QRIBBON_EXPORT PopupColorButton : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)
public:
    PopupColorButton(QWidget *parent = Q_NULL);
    virtual ~PopupColorButton();
public:
    const QColor &color() const;
    void setColor(const QColor &color);
public:
    virtual QSize sizeHint() const;

Q_SIGNALS:
    void colorChanged(const QColor &color);
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
protected:
    QColor m_color;
};

QRIBBON_END_NAMESPACE
