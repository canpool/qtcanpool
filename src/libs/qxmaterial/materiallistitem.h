/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALLISTITEM_H
#define MATERIALLISTITEM_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialListItemPrivate;

/*!
 * Material's list item: one row of a list, saying one thing or two.
 *
 * The row is forty-eight tall while it says one line and seventy-two while it
 * says two, an icon standing before the words when it was given one, and a
 * widget of the application's standing after them when it was given that - a
 * trailing action is the application's, not the row's. The row is all one
 * thing: the wash of being pointed at or pressed covers it whole, and the
 * state of being chosen is the accent, in the wash and in the words.
 *
 * A row answers a press with \c clicked, the way the base answers for every
 * control; whether a click chooses the row is the list's answer, not the
 * row's.
 */
class QX_MATERIAL_EXPORT MaterialListItem : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString secondaryText READ secondaryText WRITE setSecondaryText NOTIFY secondaryTextChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
public:
    explicit MaterialListItem(QWidget *parent = Q_NULLPTR);
    ~MaterialListItem() override;

    /*! The first line - the one a one-line row has. */
    QString text() const;
    /*! The second line; empty is a row that says one line. */
    QString secondaryText() const;
    /*! The icon standing before the words, if it was given one. */
    QIcon icon() const;
    /*! Whether the row is the chosen one. */
    bool isSelected() const;

    /*! The widget of the application's standing after the words, if any. */
    QWidget *trailingWidget() const;
    /*! Puts \a widget in the trailing slot. The row keeps it alive and
     *  placed; what it does is the application's. */
    void setTrailingWidget(QWidget *widget);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setText(const QString &text);
    void setSecondaryText(const QString &text);
    void setIcon(const QIcon &icon);
    void setSelected(bool selected);
Q_SIGNALS:
    void textChanged(const QString &text);
    void secondaryTextChanged(const QString &text);
    void iconChanged(const QIcon &icon);
    /*! The row's being chosen changed. */
    void selectedChanged(bool selected);
protected:
    qreal cornerRadius() const override;
    QColor washColor() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    void fillAccessibleState(QAccessible::State *state) const override;
private:
    Q_DISABLE_COPY(MaterialListItem)
    QX_DECLARE_PRIVATE(MaterialListItem)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALLISTITEM_H
