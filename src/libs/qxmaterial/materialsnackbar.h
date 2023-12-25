/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALSNACKBAR_H
#define QXMATERIALSNACKBAR_H

#include "materialoverlaywidget.h"
#include "qxmaterial_global.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialSnackbarPrivate;

class QX_MATERIAL_EXPORT MaterialSnackbar : public MaterialOverlayWidget
{
    Q_OBJECT
public:
    explicit MaterialSnackbar(QWidget *parent = 0);
    ~MaterialSnackbar();

    void setAutoHideDuration(int duration);
    int autoHideDuration() const;

    void setUseThemeColors(bool value);
    bool useThemeColors() const;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    void setBackgroundOpacity(qreal opacity);
    qreal backgroundOpacity() const;

    void setTextColor(const QColor &color);
    QColor textColor() const;

    void setFontSize(qreal size);
    qreal fontSize() const;

    void setBoxWidth(int width);
    int boxWidth() const;

    void setClickToDismissMode(bool value);
    bool clickToDismissMode() const;

public slots:
    void addMessage(const QString &message);
    void addInstantMessage(const QString &message);

protected slots:
    void dequeue();
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    const QScopedPointer<MaterialSnackbarPrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialSnackbar)
    Q_DECLARE_PRIVATE(MaterialSnackbar)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALSNACKBAR_H
