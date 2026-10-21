/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTBREADCRUMBBAR_H
#define FLUENTBREADCRUMBBAR_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentBreadcrumbBarPrivate;

/*!
 * Fluent's breadcrumb bar: the path to where the user is, and each step of it
 * is a way back.
 *
 * Qt has no breadcrumb, and the nearest thing - a row of flat buttons with
 * ">" labels between them - gets three things wrong: the last step is not a
 * destination, the separators are not part of the clickable thing, and the
 * whole row is announced as a row of buttons rather than as a path. Here the
 * last item is where the user is and answers nothing; the ones before it are
 * how to go back; and what an assistive tool reads is the path.
 */
class QX_FLUENT_EXPORT FluentBreadcrumbBar : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QStringList items READ items WRITE setItems)
public:
    explicit FluentBreadcrumbBar(QWidget *parent = Q_NULLPTR);
    ~FluentBreadcrumbBar() override;

    QStringList items() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setItems(const QStringList &items);

Q_SIGNALS:
    /*! Emitted for a step the user chose; the last step never is, because it
     *  is where they already are. */
    void itemClicked(int index, const QString &text);
protected:
    QRectF chromeRect() const override;
    void paintContent(QPainter *painter) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(FluentBreadcrumbBar)
    QX_DECLARE_PRIVATE(FluentBreadcrumbBar)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTBREADCRUMBBAR_H
