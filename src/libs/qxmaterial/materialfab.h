/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALFAB_H
#define QXMATERIALFAB_H

#include "materialraisedbutton.h"
#include "qxmaterial_global.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialFloatingActionButtonPrivate;

class QX_MATERIAL_EXPORT MaterialFloatingActionButton : public MaterialRaisedButton
{
    Q_OBJECT
public:
    explicit MaterialFloatingActionButton(const QIcon &icon, QWidget *parent = 0);
    ~MaterialFloatingActionButton();

    QSize sizeHint() const Q_DECL_OVERRIDE;

    void setMini(bool state);
    bool isMini() const;

    void setCorner(Qt::Corner corner);
    Qt::Corner corner() const;

    void setOffset(int x, int y);
    QSize offset() const;

    void setXOffset(int x);
    int xOffset() const;

    void setYOffset(int y);
    int yOffset() const;
protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    void updateClipPath() Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialFloatingActionButton)
    Q_DECLARE_PRIVATE(MaterialFloatingActionButton)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALFAB_H
