/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#ifndef WINDOWBARPRIVATE_H
#define WINDOWBARPRIVATE_H

#include "windowbar.h"
#include <QBoxLayout>

QX_WINDOW_BEGIN_NAMESPACE

class WindowBarPrivate
{
    Q_DECLARE_PUBLIC(WindowBar)
public:
    WindowBarPrivate();
    virtual ~WindowBarPrivate();

    void init();

    WindowBar *q_ptr;

    QWidget *w;
    bool autoTitle;
    bool autoIcon;

    enum WindowBarItem {
        IconButton,
        MenuWidget,
        TitleLabel,
        MinimumButton,
        MaximumButton,
        CloseButton,
    };

    QHBoxLayout *layout;

    inline QWidget *widgetAt(int index) const
    {
        return layout->itemAt(index)->widget();
    }

    void setWidgetAt(int index, QWidget *widget);

    QWidget *takeWidgetAt(int index);

    inline void insertDefaultSpace(int index)
    {
        layout->insertSpacerItem(index, new QSpacerItem(0, 0));
    }
private:
    Q_DISABLE_COPY(WindowBarPrivate)
};

QX_WINDOW_END_NAMESPACE

#endif   // WINDOWBARPRIVATE_H