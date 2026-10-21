/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALSNACKBAR_H
#define MATERIALSNACKBAR_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialSnackbarPrivate;

/*!
 * Material's snackbar: a sentence at the bottom of what is going on, that goes
 * away on its own.
 *
 * Qt has status bars and message boxes, and neither is this: a status bar is a
 * place that is always there, and a message box stops everything until it is
 * answered. A snackbar is what says a thing happened without asking anything -
 * it arrives over the content, offers at most one thing to do about it, and
 * leaves when it has been read.
 *
 * It is the page's own colours turned over, which is what makes it a surface
 * from somewhere else: the ink becomes the surface and the surface becomes the
 * ink. That is not the Fluent info bar, which stays a surface of the page and
 * says what it is with a severity colour and a place in the layout - a snackbar
 * is not in the layout at all, and does not stay.
 *
 * Give it the widget it belongs to as its parent; it puts itself at the bottom
 * of that widget, and goes back there when the widget changes size.
 */
class QX_MATERIAL_EXPORT MaterialSnackbar : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString message READ message WRITE setMessage)
    Q_PROPERTY(QString actionText READ actionText WRITE setActionText)
    Q_PROPERTY(int duration READ duration WRITE setDuration)
public:
    explicit MaterialSnackbar(QWidget *parent = Q_NULLPTR);
    ~MaterialSnackbar() override;

    QString message() const;
    /*! The one thing to do about it; empty while there is nothing to do. */
    QString actionText() const;
    /*! How long it stays, in milliseconds; 0 means it stays until it is
     *  dismissed. */
    int duration() const;

    QSize sizeHint() const override;

public Q_SLOTS:
    void setMessage(const QString &message);
    void setActionText(const QString &actionText);
    void setDuration(int duration);

    /*!
     * Put \a message up, and take it down after \a duration milliseconds - or
     * after the duration it was given, when none is asked for here. A snackbar
     * already up does not rise again: it says the new thing where it is.
     */
    void showMessage(const QString &message, int duration = -1);

Q_SIGNALS:
    /*! Emitted when the one thing to do about it was asked for. */
    void actionTriggered();
protected:
    qreal cornerRadius() const override;
    QRectF chromeRect() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void hideEvent(QHideEvent *event) override;

    bool eventFilter(QObject *watched, QEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(MaterialSnackbar)
    QX_DECLARE_PRIVATE(MaterialSnackbar)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALSNACKBAR_H
