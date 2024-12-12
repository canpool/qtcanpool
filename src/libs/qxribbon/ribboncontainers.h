/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon_global.h"
#include <QWidget>

class QAbstractButton;

QX_RIBBON_BEGIN_NAMESPACE

/**
 * @brief Control Widget Container
 *
 * Qt::Horizontal (default)
 *  ------------------------
 *  | icon | text | widget |
 *  ------------------------
 *
 * Qt::Vertical
 *  ------------------------
 *  | icon | text          |
 *  ------------------------
 *  |                      |
 *  |        widget        |
 *  |                      |
 *  ------------------------
 */
class RibbonCtrlContainerPrivate;
class QX_RIBBON_EXPORT RibbonCtrlContainer : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonCtrlContainer(QWidget *parent = Q_NULLPTR);
    explicit RibbonCtrlContainer(Qt::Orientation orientation, QWidget *parent = Q_NULLPTR);
    virtual ~RibbonCtrlContainer();

    void setIconVisible(bool b);
    void setTitleVisible(bool b);

    void setIcon(const QIcon &i);
    QIcon icon() const;

    void setIconSize(const QSize &size);
    QSize iconSize() const;

    void setText(const QString &t);
    QString text() const;

    void setWidget(QWidget *w);
    QWidget *widget() const;

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);
private:
    QX_DECLARE_PRIVATE(RibbonCtrlContainer)
};

/* RibbonGridContainer */
class RibbonGridContainerPrivate;
class QX_RIBBON_EXPORT RibbonGridContainer : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonGridContainer(QWidget *parent = nullptr);
    virtual ~RibbonGridContainer();

public:
    QAction *addAction(const QIcon &icon, const QString &text);
    void addButton(QAbstractButton *button);

    void setColumnCount(int count);
    int columnCount() const;

private:
    QX_DECLARE_PRIVATE(RibbonGridContainer)
};

QX_RIBBON_END_NAMESPACE
