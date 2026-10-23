/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CUPERTINOSEGMENTEDCONTROL_H
#define CUPERTINOSEGMENTEDCONTROL_H

#include "qxcupertino_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtCore/QStringList>

QX_CUPERTINO_BEGIN_NAMESPACE

class CupertinoSegmentedControlPrivate;

/*!
 * Cupertino's segmented control: one row of labels, one of them picked, and a
 * white pill under the pick that slides from one to the next.
 *
 * The row is the control. A set of radio buttons would be a stop per button and
 * a ring round each of them; here the whole row is one stop, the left and right
 * keys walk it, and what moves is the pill - the pick is the position of a thing
 * rather than the state of one of several.
 *
 * The pill slides over the separators between the segments, which is what makes
 * the movement read: the line the pill has arrived on disappears under it and
 * comes back when it leaves. That is also why the pill is drawn after the lines
 * rather than the lines being skipped for the picked segment - there is then one
 * place that decides where the lines are, and the pill covers what it covers.
 */
class QX_CUPERTINO_EXPORT CupertinoSegmentedControl : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
public:
    explicit CupertinoSegmentedControl(QWidget *parent = Q_NULLPTR);
    ~CupertinoSegmentedControl() override;

    /*! The labels, in order. */
    QStringList items() const;
    /*! How many segments there are. */
    int count() const;
    /*! The picked segment, or -1 while there is none to pick. */
    int currentIndex() const;
    /*! The label of the picked segment; empty while there is none. */
    QString currentText() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    /*! Replaces every label. The pick stays where it was while it still names a
     *  segment, and moves to the first one when it does not. */
    void setItems(const QStringList &items);
    /*! Appends a label, and picks it if nothing was picked yet. */
    void addItem(const QString &text);
    /*! Picks \a index, bounded to the segments there are; emits
     *  currentIndexChanged() when the pick moves. */
    void setCurrentIndex(int index);

Q_SIGNALS:
    /*! Emitted when the pick moves, by the pointer, the keyboard or the
     *  application. */
    void currentIndexChanged(int index);
protected:
    qreal cornerRadius() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    void fillAccessibleState(QAccessible::State *state) const override;
private:
    Q_DISABLE_COPY(CupertinoSegmentedControl)
    QX_DECLARE_PRIVATE(CupertinoSegmentedControl)
};

QX_CUPERTINO_END_NAMESPACE

#endif   // CUPERTINOSEGMENTEDCONTROL_H
