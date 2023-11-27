/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2022-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifndef QGOODSTYLEHELPER_H
#define QGOODSTYLEHELPER_H

#ifdef _WIN32

#include <QStyle>

namespace QGoodStyleHelper {
QPixmap winStandardPixmap(QStyle::StandardPixmap standardPixmap);
}

#endif // _WIN32

#endif   // QGOODSTYLEHELPER_H
