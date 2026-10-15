/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"

#include <QWidget>

QX_RIBBON_BEGIN_NAMESPACE

/**
 * @brief KeyTip badge: a letter hint overlaid on each triggerable ribbon element
 *        while the keyboard-cascade (Alt) mode is active.
 *
 * Display-only widget with a mouse-transparent surface (WA_TransparentForMouseEvents);
 * it never takes part in normal interaction. RibbonBar creates, positions and destroys
 * it uniformly when keytip mode is entered.
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
