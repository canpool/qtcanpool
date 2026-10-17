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
protected:
    // Qt 6 changed the parameter of this virtual from QEvent to QEnterEvent. Declaring
    // it with the Qt 5 type on Qt 6 does not override QWidget's virtual, it hides it -
    // and unlike a plain warning, the consequence is silent: the handler simply never
    // runs and the menu it belongs to keeps two entries looking current. The override
    // specifier is what turns that mismatch into a compile error; clang is the only
    // compiler that would otherwise have said anything at all.
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) Q_DECL_OVERRIDE;
#else
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
#endif
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
private:
    QX_DECLARE_PRIVATE(RibbonGridContainer)
};

QX_RIBBON_END_NAMESPACE
