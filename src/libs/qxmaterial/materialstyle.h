/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALSTYLE_H
#define QXMATERIALSTYLE_H

#include "qxmaterial_global.h"
#include <QtWidgets/QCommonStyle>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialTheme;
class MaterialStylePrivate;

class QX_MATERIAL_EXPORT MaterialStyle : public QCommonStyle
{
    Q_OBJECT
public:
    inline static MaterialStyle &instance();

    void setTheme(MaterialTheme *theme);
    QColor themeColor(const QString &key) const;
private:
    MaterialStyle();
    ~MaterialStyle();
private:
    QX_DECLARE_PRIVATE(MaterialStyle)
};

inline MaterialStyle &MaterialStyle::instance()
{
    static MaterialStyle instance;
    return instance;
}

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALSTYLE_H
