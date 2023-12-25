/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALCIRCULARPROGRESS_P_H
#define QXMATERIALCIRCULARPROGRESS_P_H

#include "qxmaterial_global.h"
#include "materialnamespace.h"
#include "materialcircularprogress.h"
#include <QColor>
#include <QObject>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialCircularProgress;
class MaterialCircularProgressDelegate;

class MaterialCircularProgressPrivate
{
    Q_DISABLE_COPY(MaterialCircularProgressPrivate)
    Q_DECLARE_PUBLIC(MaterialCircularProgress)
public:
    MaterialCircularProgressPrivate(MaterialCircularProgress *q);
    ~MaterialCircularProgressPrivate();

    void init();

    MaterialCircularProgress *const q_ptr;
    MaterialCircularProgressDelegate *delegate;
    QxMaterial::ProgressType progressType;
    QColor color;
    qreal penWidth;
    int size;
    bool useThemeColors;
};

class MaterialCircularProgressDelegate : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal dashOffset WRITE setDashOffset READ dashOffset)
    Q_PROPERTY(qreal dashLength WRITE setDashLength READ dashLength)
    Q_PROPERTY(int angle WRITE setAngle READ angle)
public:
    MaterialCircularProgressDelegate(MaterialCircularProgress *parent);
    ~MaterialCircularProgressDelegate();

    inline void setDashOffset(qreal offset);
    inline qreal dashOffset() const;

    inline void setDashLength(qreal length);
    inline qreal dashLength() const;

    inline void setAngle(int angle);
    inline int angle() const;
private:
    Q_DISABLE_COPY(MaterialCircularProgressDelegate)

    MaterialCircularProgress *const m_progress;
    qreal m_dashOffset;
    qreal m_dashLength;
    int m_angle;
};

inline void MaterialCircularProgressDelegate::setDashOffset(qreal offset)
{
    m_dashOffset = offset;
    m_progress->update();
}

inline qreal MaterialCircularProgressDelegate::dashOffset() const
{
    return m_dashOffset;
}

inline void MaterialCircularProgressDelegate::setDashLength(qreal length)
{
    m_dashLength = length;
    m_progress->update();
}

inline qreal MaterialCircularProgressDelegate::dashLength() const
{
    return m_dashLength;
}

inline void MaterialCircularProgressDelegate::setAngle(int angle)
{
    m_angle = angle;
    m_progress->update();
}

inline int MaterialCircularProgressDelegate::angle() const
{
    return m_angle;
}


QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALCIRCULARPROGRESS_P_H
