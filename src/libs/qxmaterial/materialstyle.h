/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALSTYLE_H
#define QXMATERIALSTYLE_H

#include "materialstyle_p.h"
#include <QtWidgets/QCommonStyle>

QX_MATERIAL_BEGIN_NAMESPACE

#define MATERIAL_DISABLE_THEME_COLORS                                                                                  \
    if (d->useThemeColors == true) {                                                                                   \
        d->useThemeColors = false;                                                                                     \
    }

class MaterialTheme;

class MaterialStyle : public QCommonStyle
{
    Q_OBJECT
public:
    inline static MaterialStyle &instance();

    void setTheme(MaterialTheme *theme);
    QColor themeColor(const QString &key) const;
protected:
    const QScopedPointer<MaterialStylePrivate> d_ptr;
private:
    Q_DECLARE_PRIVATE(MaterialStyle)

    MaterialStyle();

    MaterialStyle(MaterialStyle const &);
    void operator=(MaterialStyle const &);
};

inline MaterialStyle &MaterialStyle::instance()
{
    static MaterialStyle instance;
    return instance;
}

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALSTYLE_H
