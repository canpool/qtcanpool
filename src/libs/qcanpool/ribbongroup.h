/**
 * Copyright (C) 2020-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef RIBBONGROUP_H
#define RIBBONGROUP_H

#include "qcanpool.h"
#include <QWidget>
#include <QIcon>

class QToolButton;

QCANPOOL_BEGIN_NAMESPACE

class RibbonGroupPrivate;
class QCANPOOL_SHARED_EXPORT RibbonGroup : public QWidget
{
    Q_OBJECT
public:
    enum GroupSize {
        GroupLarge, GroupMedium, GroupSmall
    };

public:
    explicit RibbonGroup(const QString &title, QWidget *parent = nullptr);
    ~RibbonGroup();

    void setTitle(const QString &title);
    QString title() const;

    void setTitleFont(const QFont &font);
    QFont titleFont() const;

    void setOptionButtonVisible(bool visible = true);
    bool isOptionButtonVisible() const;
    void setOptionButtonToolTip(const QString &tip);

    QAction *addAction(const QIcon &icon, const QString &text, GroupSize size);
    void addAction(QAction *action, GroupSize size); // for action that has menu
    void addToolButton(QToolButton *button, GroupSize size);

    void addWidget(QWidget *widget);
    void removeWidget(QWidget *widget);

    virtual QSize sizeHint() const;

Q_SIGNALS:
    void optionClicked();

private:
    RibbonGroupPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // RIBBONGROUP_H
