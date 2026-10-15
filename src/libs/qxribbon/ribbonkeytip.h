/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"

#include <QWidget>

QX_RIBBON_BEGIN_NAMESPACE

/**
 * @brief KeyTip 角标：在键盘级联（Alt）模式下覆盖在 ribbon 各可触发元素上的字母提示
 *
 * 纯展示控件，鼠标穿透（WA_TransparentForMouseEvents），不参与正常交互。
 * 由 RibbonBar 在 keytip 模式进入时统一创建/定位/销毁。
 */
class RibbonKeyTip : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonKeyTip(QWidget *parent = Q_NULLPTR);

    void setKey(const QString &key);
    QString key() const;

    void setTarget(QWidget *target);
    QWidget *target() const;

    QSize sizeHint() const Q_DECL_OVERRIDE;
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    QString m_key;
    QWidget *m_target;
};

QX_RIBBON_END_NAMESPACE
