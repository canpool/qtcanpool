/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALCHIP_H
#define MATERIALCHIP_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialChipPrivate;

/*!
 * Material's chip: a small thing that stands for one choice, or does one
 * thing.
 *
 * Qt has buttons for both, and a button is what a chip is made of - but a
 * button in a row of buttons reads as a toolbar, while a row of chips reads as
 * a set of things the user is carrying: a filter applied, a tag on a message, a
 * suggestion offered. The difference is the shape and the size, and the shape
 * is what makes a row of them a set.
 *
 * The two kinds are not the same answer to the same question:
 *
 * - an \c Assist chip does something when it is pressed, and holds nothing -
 *   it is a button, in a chip's shape;
 * - a \c Filter chip holds a state: pressing it puts a filter in force or
 *   takes it back, and while it is in force it is filled and carries a mark,
 *   because "which of these are on" has to be readable without pressing them
 *   to find out.
 */
class QX_MATERIAL_EXPORT MaterialChip : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(Kind kind READ kind WRITE setKind)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY toggled)
public:
    enum class Kind {
        Assist, /*!< Does something when pressed; holds nothing. */
        Filter  /*!< Holds a state: a filter in force, or not. */
    };
    Q_ENUM(Kind)

    explicit MaterialChip(Kind kind = Kind::Assist, QWidget *parent = Q_NULLPTR);
    explicit MaterialChip(const QString &text, QWidget *parent = Q_NULLPTR);
    ~MaterialChip() override;

    Kind kind() const;
    QString text() const;

    /*!
     * True while a filter chip has its filter in force. An assist chip holds
     * nothing, so it is never checked and asking it to be does nothing.
     */
    bool isChecked() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setKind(Kind kind);
    void setText(const QString &text);
    void setChecked(bool checked);
    void toggle();

Q_SIGNALS:
    /*! Emitted when a filter chip's state changed, whoever changed it. */
    void toggled(bool checked);
protected:
    qreal cornerRadius() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    void fillAccessibleState(QAccessible::State *state) const override;
private:
    Q_DISABLE_COPY(MaterialChip)
    QX_DECLARE_PRIVATE(MaterialChip)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALCHIP_H
