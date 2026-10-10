/**
 * Copyright (C) 2022-2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTOOLBUTTON_H
#define QXTOOLBUTTON_H

#include "qxapp_global.h"
#include <QToolButton>

QX_APP_BEGIN_NAMESPACE

class QxToolButtonPrivate;

/* QxToolButton */
class QX_APP_EXPORT QxToolButton : public QToolButton
{
    Q_OBJECT
public:
    // only for Qt::ToolButtonTextUnderIcon style
    enum MenuArea {
        RightMenuArea,    // unique value for other style
        BottomMenuArea,   // default value
    };
public:
    explicit QxToolButton(QWidget *parent = nullptr);
    explicit QxToolButton(const QString &text, QWidget *parent = nullptr);
    QxToolButton(const QIcon &icon, const QString &text, QWidget *parent = nullptr);
    virtual ~QxToolButton();

    Qt::ArrowType menuArrowType() const;
    void setMenuArrowType(Qt::ArrowType type);

    MenuArea menuArea() const;
    void setMenuArea(MenuArea area);

    QSize sizeHint() const override;

    void setForceAlignCenter(bool b = true);
    void setForceDefaultShowMenu(bool b = false);

public Q_SLOTS:
    void setToolButtonStyle(Qt::ToolButtonStyle style);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *e) override;
private:
    QxToolButtonPrivate *d;
};

QX_APP_END_NAMESPACE

#endif   // QXTOOLBUTTON_H
