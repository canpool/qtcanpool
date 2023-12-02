/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2022-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifndef QGOODSTATEHOLDER
#define QGOODSTATEHOLDER

#include "qgoodbye_global.h"
#include <QObject>

class QGOODBYE_SHARED_EXPORT QGoodStateHolder : public QObject
{
    Q_OBJECT
public:
    static QGoodStateHolder *instance();
private:
    explicit QGoodStateHolder();
    ~QGoodStateHolder();

Q_SIGNALS:
    void currentThemeChanged();

public Q_SLOTS:
    bool isCurrentThemeDark() const;
    void setCurrentThemeDark(bool dark);
private:
    bool m_dark;
};

#define qGoodStateHolder QGoodStateHolder::instance()

#endif   // QGOODSTATEHOLDER
