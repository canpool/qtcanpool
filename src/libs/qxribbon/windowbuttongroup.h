/**
 * Copyleft (C) 2023-2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include <QWidget>

QX_RIBBON_BEGIN_NAMESPACE

class WindowButtonGroupPrivate;

class QX_RIBBON_EXPORT WindowButtonGroup : public QWidget
{
    Q_OBJECT
public:
    explicit WindowButtonGroup(QWidget *parent, Qt::WindowFlags flags = Qt::WindowFlags());
    ~WindowButtonGroup();

    void updateWindowFlags(Qt::WindowFlags flags);

    /*! Multiplies the caption glyph size. It used to scale the button's own size,
        which the style sheet overrode, so 1.0 is the size the glyphs are drawn at
        by default. */
    void setIconScale(qreal scale = 1.0);
    void setWindowStates(Qt::WindowStates s);

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    QWidget *widgetForMaximizeButton() const;

    /** default is disabled */
    bool signalIsEnabled() const;

public Q_SLOTS:
    void setSignalEnabled(bool enable);

Q_SIGNALS:
    void buttonMinimizeClicked();
    void buttonMaximzieClicked();
    void buttonCloseClicked();
protected:
    void changeEvent(QEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
private:
    QX_DECLARE_PRIVATE(WindowButtonGroup)
};

QX_RIBBON_END_NAMESPACE
