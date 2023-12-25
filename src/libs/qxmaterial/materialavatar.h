/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALAVATAR_H
#define QXMATERIALAVATAR_H

#include "materialnamespace.h"
#include "qxmaterial_global.h"
#include <QtWidgets/QWidget>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialAvatarPrivate;

class QX_MATERIAL_EXPORT MaterialAvatar : public QWidget
{
    Q_OBJECT
public:
    explicit MaterialAvatar(QWidget *parent = 0);
    explicit MaterialAvatar(const QIcon &icon, QWidget *parent = 0);
    explicit MaterialAvatar(const QChar &letter, QWidget *parent = 0);
    explicit MaterialAvatar(const QImage &image, QWidget *parent = 0);
    ~MaterialAvatar();

    void setUseThemeColors(bool value);
    bool useThemeColors() const;

    void setTextColor(const QColor &color);
    QColor textColor() const;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    QSize sizeHint() const Q_DECL_OVERRIDE;

    void setSize(int size);
    int size() const;

    void setLetter(const QChar &letter);
    void setImage(const QImage &image);
    void setIcon(const QIcon &icon);

    QxMaterial::AvatarType type() const;
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    const QScopedPointer<MaterialAvatarPrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialAvatar)
    Q_DECLARE_PRIVATE(MaterialAvatar)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALAVATAR_H
