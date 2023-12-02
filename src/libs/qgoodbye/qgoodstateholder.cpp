/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2022-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#include "qgoodstateholder.h"

QGoodStateHolder *QGoodStateHolder::instance()
{
    static QGoodStateHolder instance;
    return &instance;
}

QGoodStateHolder::QGoodStateHolder() : QObject()
{
    m_dark = false;
}

QGoodStateHolder::~QGoodStateHolder()
{
}

bool QGoodStateHolder::isCurrentThemeDark() const
{
    return m_dark;
}

void QGoodStateHolder::setCurrentThemeDark(bool dark)
{
    m_dark = dark;

    Q_EMIT currentThemeChanged();
}
