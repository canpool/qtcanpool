/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALPROGRESS_P_H
#define QXMATERIALPROGRESS_P_H

#include "qxmaterial_global.h"
#include "materialnamespace.h"
#include "materialprogress.h"
#include <QColor>
#include <QObject>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialProgress;
class MaterialProgressDelegate;

class MaterialProgressPrivate
{
    Q_DISABLE_COPY(MaterialProgressPrivate)
    Q_DECLARE_PUBLIC(MaterialProgress)
public:
    MaterialProgressPrivate(MaterialProgress *q);
    ~MaterialProgressPrivate();

    void init();

    MaterialProgress *const q_ptr;
    MaterialProgressDelegate *delegate;
    QxMaterial::ProgressType progressType;
    QColor progressColor;
    QColor backgroundColor;
    bool useThemeColors;
};

class MaterialProgressDelegate : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal offset WRITE setOffset READ offset)
public:
    MaterialProgressDelegate(MaterialProgress *parent);
    ~MaterialProgressDelegate();

    inline void setOffset(qreal offset);
    inline qreal offset() const;
private:
    Q_DISABLE_COPY(MaterialProgressDelegate)

    MaterialProgress *const m_progress;
    qreal m_offset;
};

inline void MaterialProgressDelegate::setOffset(qreal offset)
{
    m_offset = offset;
    m_progress->update();
}

inline qreal MaterialProgressDelegate::offset() const
{
    return m_offset;
}

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALPROGRESS_P_H
