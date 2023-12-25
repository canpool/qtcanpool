/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALTHEME_H
#define QXMATERIALTHEME_H

#include "materialnamespace.h"
#include "qxmaterial_global.h"
#include <QColor>
#include <QIcon>
#include <QObject>
#include <QScopedPointer>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialThemePrivate;

class QX_MATERIAL_EXPORT MaterialTheme : public QObject
{
    Q_OBJECT
public:
    explicit MaterialTheme(QObject *parent = 0);
    ~MaterialTheme();

    QList<QString> colorKeys() const;
    QColor getColor(const QString &key) const;

    void setColor(const QString &key, const QColor &color);
    void setColor(const QString &key, QxMaterial::Color color);

    static QIcon icon(QString category, QString icon);

    static const QColor &color(QxMaterial::Color color);
    static QColor rgba(int r, int g, int b, qreal a);
protected:
    const QScopedPointer<MaterialThemePrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialTheme)
    Q_DECLARE_PRIVATE(MaterialTheme)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALTHEME_H
