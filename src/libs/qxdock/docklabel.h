/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QLabel>

QX_DOCK_BEGIN_NAMESPACE

class DockLabelPrivate;

// A QLabel that supports eliding text.
class QX_DOCK_EXPORT DockLabel : public QLabel
{
    Q_OBJECT
public:
    explicit DockLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    explicit DockLabel(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~DockLabel();

    Qt::TextElideMode elideMode() const;
    void setElideMode(Qt::TextElideMode mode);

    bool isElided() const;

public:
    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;
    QString text() const;
    void setText(const QString &text);

Q_SIGNALS:
    void clicked();
    void doubleClicked();
    void elidedChanged(bool elided);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
    virtual void resizeEvent(QResizeEvent *e) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *e) override;

private:
    QX_DECLARE_PRIVATE(DockLabel)
};

QX_DOCK_END_NAMESPACE
