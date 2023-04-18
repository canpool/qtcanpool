/**
 * Copyright (C) 2022-2023 maminjie <canpool@163.com>
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
    // only for Qt::ToolButtonTextUnderIcon style
    enum MenuArea {
        RightMenuArea,  // unique value for other style
        BottomMenuArea, // default value
    };

public:
    explicit FancyToolButton(QWidget *parent = nullptr);
    virtual ~FancyToolButton();

    Qt::ArrowType menuArrowType() const;
    void setMenuArrowType(Qt::ArrowType type);

    MenuArea menuArea() const;
    void setMenuArea(MenuArea area);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    FancyToolButtonPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYTOOLBUTTON_H
