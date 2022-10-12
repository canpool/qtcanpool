/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
/****************************************************************************
 * NOTES:
 *  1) the mainWidget is host-widget, FancyTitleBar is used as its event filter
 *  2) the titleWidget is the title bar area of mainWidget, used to guide the
 *     mouse event operation of FancyTitleBar.
 *
 *  -------------------------------------,
 *  |           titleWidget             | \
 *  |------------------------------------  \
 *  |                                   |    mainWindget
 *  |                                   |  /
 *  |                                   | /
 *  -------------------------------------'
 *
 *  3) systemToolBar contains min, max, close Buttons
 *
 *   --------------------------,--------,     ,--- systemToolBar
 *   |          titleWidget    | - [] x | <--'
 *   --------------------------'--------'
 *
 *****************************************************************************
 */
#ifndef FANCYTITLEBAR_H
#define FANCYTITLEBAR_H

#include "qcanpool.h"
#include <QObject>

class QEvent;
class QLabel;
class QToolButton;
class QAction;

QCANPOOL_BEGIN_NAMESPACE

class FancyTitleBarPrivate;

/* FancyTitleBar */
class QCANPOOL_SHARED_EXPORT FancyTitleBar : public QObject
{
    Q_OBJECT

public:
    enum ButtonRole {
        MinButton, MaxButton, NormalButton, CloseButton,
    };

public:
    explicit FancyTitleBar(QWidget *mainWidget);
    virtual ~FancyTitleBar();

    void setTitleWidget(QWidget *widget);

    QToolButton *logoButton() const;
    QLabel  *titleLabel() const;
    QWidget *systemToolBar() const;
    QWidget *mainWidget() const;

    QAction *addAction(const QIcon &icon, const QString &text);
    void     addAction(QAction *action);
    void     insertAction(QAction *before, QAction *action);
    QAction *addSeparator();
    QAction *insertSeparator(QAction *before);
    QAction *addWidget(QWidget *widget);
    QAction *insertWidget(QAction *before, QWidget *widget);

    void setButtonIcon(ButtonRole role, const QIcon &icon);
    void setButtonTip(ButtonRole role, const QString &tip);

    void setWidgetResizable(bool resizable);
    void setWidgetMovable(bool movable);
    void setWidgetMaximizable(bool maximizable);

    void setDisabled(bool disable);
    void updateWidgetFlags();

signals:
    void windowResizable(bool resizable);
    void windowMovable(bool movable);
    void windowIconChanged(QIcon &icon);

public:
    void setIconSize(const QSize &size);

protected:
    virtual bool eventFilter(QObject* object, QEvent* event);

private:
    FancyTitleBarPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYTITLEBAR_H
