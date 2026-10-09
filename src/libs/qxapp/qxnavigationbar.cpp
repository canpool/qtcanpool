/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxnavigationbar.h"

#include <QtCore/QList>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

QX_APP_BEGIN_NAMESPACE

/*! Icon size the entries start with, in device independent pixels. */
static const int s_defaultIconSize = 24;

class QxNavigationBarPrivate
{
    QX_DECLARE_PUBLIC(QxNavigationBar)
public:
    QxNavigationBarPrivate();
    void init();
    /*! Re-numbers the button group so that group ids match list indices. */
    void updateButtonIds();
    /*! Re-reads the selection from the buttons and announces a changed index. */
    void syncCurrentIndex();
    QToolButton *buttonAt(int index) const;
public:
    QVBoxLayout *m_layout;
    QButtonGroup *m_group;
    QList<QToolButton *> m_buttons;
    QSize m_iconSize;
    int m_currentIndex;
};

QxNavigationBarPrivate::QxNavigationBarPrivate()
    : m_layout(Q_NULLPTR)
    , m_group(Q_NULLPTR)
    , m_iconSize(s_defaultIconSize, s_defaultIconSize)
    , m_currentIndex(-1)
{
}

void QxNavigationBarPrivate::init()
{
    Q_Q(QxNavigationBar);

    m_layout = new QVBoxLayout(q);
    m_layout->setContentsMargins(2, 2, 2, 2);
    m_layout->setSpacing(2);
    // Keeps the entries packed at the top when the rail is taller than the list.
    m_layout->addStretch(1);

    m_group = new QButtonGroup(q);
    m_group->setExclusive(true);
    QObject::connect(m_group, &QButtonGroup::idClicked, q, &QxNavigationBar::setCurrentIndex);

    q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

QToolButton *QxNavigationBarPrivate::buttonAt(int index) const
{
    if (index < 0 || index >= m_buttons.count()) {
        return Q_NULLPTR;
    }
    return m_buttons.at(index);
}

void QxNavigationBarPrivate::updateButtonIds()
{
    for (int i = 0; i < m_buttons.count(); ++i) {
        m_group->setId(m_buttons.at(i), i);
    }
}

void QxNavigationBarPrivate::syncCurrentIndex()
{
    Q_Q(QxNavigationBar);

    int index = -1;
    for (int i = 0; i < m_buttons.count(); ++i) {
        if (m_buttons.at(i)->isChecked()) {
            index = i;
            break;
        }
    }
    if (index == m_currentIndex) {
        return;
    }
    m_currentIndex = index;
    Q_EMIT q->currentChanged(index);
}

QxNavigationBar::QxNavigationBar(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new QxNavigationBarPrivate())
{
    d_ptr->setPublic(this);
    Q_D(QxNavigationBar);
    d->init();
}

QxNavigationBar::~QxNavigationBar()
{
    QX_FINI_PRIVATE();
}

int QxNavigationBar::addItem(const QIcon &icon, const QString &text)
{
    Q_D(QxNavigationBar);
    const int index = d->m_buttons.count();
    insertItem(index, icon, text);
    return index;
}

void QxNavigationBar::insertItem(int index, const QIcon &icon, const QString &text)
{
    Q_D(QxNavigationBar);
    if (index < 0 || index > d->m_buttons.count()) {
        return;
    }

    QToolButton *button = new QToolButton(this);
    button->setCheckable(true);
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setIconSize(d->m_iconSize);
    button->setIcon(icon);
    button->setText(text);

    d->m_layout->insertWidget(index, button);
    d->m_buttons.insert(index, button);
    d->m_group->addButton(button);
    d->updateButtonIds();

    // An entry inserted in front of the selected one shifts its index; the
    // selection itself stays on the same button.
    d->syncCurrentIndex();
}

void QxNavigationBar::removeItem(int index)
{
    Q_D(QxNavigationBar);
    QToolButton *button = d->buttonAt(index);
    if (!button) {
        return;
    }

    d->m_buttons.removeAt(index);
    d->m_group->removeButton(button);
    d->m_layout->removeWidget(button);
    button->hide();
    button->deleteLater();
    d->updateButtonIds();

    d->syncCurrentIndex();
}

void QxNavigationBar::clear()
{
    Q_D(QxNavigationBar);
    while (!d->m_buttons.isEmpty()) {
        removeItem(d->m_buttons.count() - 1);
    }
}

int QxNavigationBar::count() const
{
    Q_D(const QxNavigationBar);
    return d->m_buttons.count();
}

int QxNavigationBar::currentIndex() const
{
    Q_D(const QxNavigationBar);
    return d->m_currentIndex;
}

QString QxNavigationBar::itemText(int index) const
{
    Q_D(const QxNavigationBar);
    QToolButton *button = d->buttonAt(index);
    return button ? button->text() : QString();
}

QIcon QxNavigationBar::itemIcon(int index) const
{
    Q_D(const QxNavigationBar);
    QToolButton *button = d->buttonAt(index);
    return button ? button->icon() : QIcon();
}

bool QxNavigationBar::isItemEnabled(int index) const
{
    Q_D(const QxNavigationBar);
    QToolButton *button = d->buttonAt(index);
    return button ? button->isEnabled() : false;
}

QSize QxNavigationBar::iconSize() const
{
    Q_D(const QxNavigationBar);
    return d->m_iconSize;
}

QToolButton *QxNavigationBar::button(int index) const
{
    Q_D(const QxNavigationBar);
    return d->buttonAt(index);
}

void QxNavigationBar::setCurrentIndex(int index)
{
    Q_D(QxNavigationBar);
    if (index < -1 || index >= d->m_buttons.count()) {
        return;
    }
    if (index == d->m_currentIndex) {
        return;
    }

    d->m_currentIndex = index;
    if (index < 0) {
        // An exclusive group refuses to uncheck its member, so exclusivity is
        // lifted for the moment it takes to clear the selection.
        const bool exclusive = d->m_group->exclusive();
        d->m_group->setExclusive(false);
        for (QToolButton *button : d->m_buttons) {
            button->setChecked(false);
        }
        d->m_group->setExclusive(exclusive);
    } else {
        d->m_buttons.at(index)->setChecked(true);
    }

    Q_EMIT currentChanged(index);
}

void QxNavigationBar::setItemText(int index, const QString &text)
{
    Q_D(QxNavigationBar);
    QToolButton *button = d->buttonAt(index);
    if (button) {
        button->setText(text);
    }
}

void QxNavigationBar::setItemIcon(int index, const QIcon &icon)
{
    Q_D(QxNavigationBar);
    QToolButton *button = d->buttonAt(index);
    if (button) {
        button->setIcon(icon);
    }
}

void QxNavigationBar::setItemEnabled(int index, bool enabled)
{
    Q_D(QxNavigationBar);
    QToolButton *button = d->buttonAt(index);
    if (button) {
        button->setEnabled(enabled);
    }
}

void QxNavigationBar::setItemToolTip(int index, const QString &toolTip)
{
    Q_D(QxNavigationBar);
    QToolButton *button = d->buttonAt(index);
    if (button) {
        button->setToolTip(toolTip);
    }
}

void QxNavigationBar::setIconSize(const QSize &size)
{
    Q_D(QxNavigationBar);
    d->m_iconSize = size;
    for (QToolButton *button : d->m_buttons) {
        button->setIconSize(size);
    }
}

QX_APP_END_NAMESPACE
