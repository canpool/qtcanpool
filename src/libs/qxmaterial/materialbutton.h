/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALBUTTON_H
#define MATERIALBUTTON_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialButtonPrivate;

/*!
 * Material's button: the command the page is built out of.
 *
 * The variants say how much surface a command claims. \c Filled is the one
 * command a view is allowed to point at, and it is the accent's own colour;
 * \c Outlined is a command of the same weight on the page's own colour; \c
 * Text is a command that sits down while others speak. The press is answered
 * by the ripple - the same one the chip and the floating action button give -
 * which starts where the pointer landed and travels outwards from there.
 */
class QX_MATERIAL_EXPORT MaterialButton : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(Variant variant READ variant WRITE setVariant)
public:
    /*!
     * The surface the command claims. \c Filled wears the accent; \c
     * Outlined wears the page and an outline; \c Text wears nothing.
     */
    enum class Variant {
        Filled,
        Outlined,
        Text
    };
    Q_ENUM(Variant)

    explicit MaterialButton(QWidget *parent = Q_NULLPTR);
    ~MaterialButton() override;

    QString text() const;
    QIcon icon() const;
    Variant variant() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setText(const QString &text);
    void setIcon(const QIcon &icon);
    void setVariant(Variant variant);
Q_SIGNALS:
    void textChanged(const QString &text);
    void iconChanged(const QIcon &icon);
protected:
    qreal cornerRadius() const override;
    QColor washColor() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(MaterialButton)
    QX_DECLARE_PRIVATE(MaterialButton)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALBUTTON_H
