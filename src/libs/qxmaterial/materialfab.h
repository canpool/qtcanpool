/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALFAB_H
#define MATERIALFAB_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialFABPrivate;

/*!
 * Material's floating action button: the one thing a screen is for, floated
 * over it.
 *
 * Qt has buttons, and a button is what this is - but a button in a layout is
 * one of the things on the screen, while a FAB is *the* thing, and it says so
 * by where it is: over the content, clear of it, lifted off it, and round.
 * It is the highest surface a screen has apart from the one that interrupts
 * it, and it is the accent's own colour, because what it stands for is what
 * the screen is for.
 *
 * What it carries is an icon, and an icon is what the application gives it: a
 * FAB with nothing on it is a circle. It says what it is to an assistive tool
 * through its \a text, which is not drawn.
 */
class QX_MATERIAL_EXPORT MaterialFAB : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(QString text READ text WRITE setText)
public:
    explicit MaterialFAB(QWidget *parent = Q_NULLPTR);
    ~MaterialFAB() override;

    QIcon icon() const;
    /*! What it says, for an assistive tool; not drawn. */
    QString text() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setIcon(const QIcon &icon);
    void setText(const QString &text);
protected:
    qreal cornerRadius() const override;
    QRectF chromeRect() const override;
    QColor washColor() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(MaterialFAB)
    QX_DECLARE_PRIVATE(MaterialFAB)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALFAB_H
