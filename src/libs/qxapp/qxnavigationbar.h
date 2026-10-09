/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXNAVIGATIONBAR_H
#define QXNAVIGATIONBAR_H

#include "qxapp_global.h"

#include <QtCore/QSize>
#include <QtGui/QIcon>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
class QButtonGroup;
class QToolButton;
class QVBoxLayout;
QT_END_NAMESPACE

QX_APP_BEGIN_NAMESPACE

class QxNavigationBarPrivate;

/*!
 * A vertical rail of exclusive, checkable entries.
 *
 * It is the navigation half of QxAppShell, which pairs it with a page stack so
 * that selecting an entry brings the matching page to the front. On its own the
 * bar carries no policy: adding the first entry does not select it, and only the
 * caller - or a click - moves the selection.
 *
 * Entries are addressed by index, and an out-of-range index makes every
 * index-based call a no-op instead of an assertion, so a caller that caches an
 * index cannot crash the bar.
 */
class QX_APP_EXPORT QxNavigationBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
public:
    explicit QxNavigationBar(QWidget *parent = Q_NULLPTR);
    ~QxNavigationBar() override;

    /*! Appends an entry and returns its index. */
    int addItem(const QIcon &icon, const QString &text);
    /*! Inserts an entry at the given index, which may also be count(). */
    void insertItem(int index, const QIcon &icon, const QString &text);
    /*! Removes the entry at the given index; no-op when it is out of range. */
    void removeItem(int index);
    /*! Removes every entry. */
    void clear();

    /*! Number of entries. */
    int count() const;
    /*! Index of the selected entry, or -1 when nothing is selected. */
    int currentIndex() const;
    QString itemText(int index) const;
    QIcon itemIcon(int index) const;
    bool isItemEnabled(int index) const;

    /*! Icon size shared by every entry; 24x24 by default. */
    QSize iconSize() const;

    /*!
     * The button behind the given index, for the rare case that needs to tweak
     * something the bar does not expose. Null when the index is out of range.
     */
    QToolButton *button(int index) const;

public Q_SLOTS:
    /*! Selects the entry at the given index; pass -1 to clear the selection. */
    void setCurrentIndex(int index);
    void setItemText(int index, const QString &text);
    void setItemIcon(int index, const QIcon &icon);
    void setItemEnabled(int index, bool enabled);
    void setItemToolTip(int index, const QString &toolTip);
    void setIconSize(const QSize &size);

Q_SIGNALS:
    /*!
     * Emitted when the selection moves: on a click, on an explicit
     * setCurrentIndex(), and also when an entry is inserted or removed in front
     * of the selected one and the index shifts. Treat the index as an address
     * into the bar, not as the identity of an entry.
     */
    void currentChanged(int index);
private:
    Q_DISABLE_COPY(QxNavigationBar)
    QX_DECLARE_PRIVATE(QxNavigationBar)
};

QX_APP_END_NAMESPACE

#endif   // QXNAVIGATIONBAR_H
