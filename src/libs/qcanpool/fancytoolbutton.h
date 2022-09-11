/**
 * Copyright (C) 2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef FANCYTOOLBUTTON_H
#define FANCYTOOLBUTTON_H

#include "qcanpool.h"
#include <QToolButton>

QCANPOOL_BEGIN_NAMESPACE

class FancyToolButtonPrivate;

/* FancyToolButton */
class QCANPOOL_SHARED_EXPORT FancyToolButton : public QToolButton
{
    Q_OBJECT

public:
    enum MenuArea {
        RightMenuArea,
        BottomMenuArea,
    };

public:
    explicit FancyToolButton(QWidget *parent = nullptr);
    virtual ~FancyToolButton();

    Qt::ArrowType menuArrowType() const;
    void setMenuArrowType(Qt::ArrowType type);

    MenuArea menuArea() const;
    void setMenuArea(MenuArea area) ;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void changeEvent(QEvent *e) override;
    void actionEvent(QActionEvent *e) override;
    bool event(QEvent *e) override;

    bool hitButton(const QPoint &pos) const override;

private:
    FancyToolButtonPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYTOOLBUTTON_H
