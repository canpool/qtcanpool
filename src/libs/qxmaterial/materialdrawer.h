/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALDRAWER_H
#define QXMATERIALDRAWER_H

#include "materialoverlaywidget.h"
#include "qxmaterial_global.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialDrawerPrivate;
class MaterialDrawerStateMachine;

class QX_MATERIAL_EXPORT MaterialDrawer : public MaterialOverlayWidget
{
    Q_OBJECT
public:
    explicit MaterialDrawer(QWidget *parent = 0);
    ~MaterialDrawer();

    void setDrawerWidth(int width);
    int drawerWidth() const;

    void setDrawerLayout(QLayout *layout);
    QLayout *drawerLayout() const;

    void setClickOutsideToClose(bool state);
    bool clickOutsideToClose() const;

    void setAutoRaise(bool state);
    bool autoRaise() const;

    void setOverlayMode(bool value);
    bool overlayMode() const;

public slots:
    void openDrawer();
    void closeDrawer();
protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    const QScopedPointer<MaterialDrawerPrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialDrawer)
    Q_DECLARE_PRIVATE(MaterialDrawer)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALDRAWER_H
