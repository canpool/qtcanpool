/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include "ribboncontrols.h"
#include <QCheckBox>
#include <QMenu>
#include <QRadioButton>
#include <QToolButton>

QRIBBON_BEGIN_NAMESPACE

class RibbonButtonControlPrivate;
/* RibbonButtonControl */
class QRIBBON_EXPORT RibbonButtonControl : public RibbonWidgetControl
{
    Q_OBJECT
public:
    explicit RibbonButtonControl(RibbonGroup *parentGroup = Q_NULL);
    virtual ~RibbonButtonControl();
public:
    const QPixmap &largeIcon() const;
    void setLargeIcon(const QPixmap &icon);

    const QPixmap &smallIcon() const;
    void setSmallIcon(const QPixmap &icon);

    const QString &label() const;
    void setLabel(const QString &label);

    void setMenu(QMenu *menu);
    QMenu *menu() const;

    void setPopupMode(QToolButton::ToolButtonPopupMode mode);
    QToolButton::ToolButtonPopupMode popupMode() const;

    void setToolButtonStyle(Qt::ToolButtonStyle style);
    Qt::ToolButtonStyle toolButtonStyle() const;

    QToolButton *widget() const;
public:
    virtual QSize sizeHint() const;
    virtual void sizeChanged(RibbonControlSizeDefinition::GroupSize size);
    virtual void actionChanged();
private:
    QTC_DECLARE_PRIVATE(RibbonButtonControl)
    Q_DISABLE_COPY(RibbonButtonControl)
};

/* RibbonCheckBoxControl */
class QRIBBON_EXPORT RibbonCheckBoxControl : public RibbonWidgetControl
{
public:
    explicit RibbonCheckBoxControl(const QString &text = QString(), RibbonGroup *parentGroup = Q_NULL);
    virtual ~RibbonCheckBoxControl();
public:
    QCheckBox *widget() const;
private:
    Q_DISABLE_COPY(RibbonCheckBoxControl)
};

/* RibbonRadioButtonControl */
class QRIBBON_EXPORT RibbonRadioButtonControl : public RibbonWidgetControl
{
public:
    explicit RibbonRadioButtonControl(const QString &text = QString(), RibbonGroup *parentGroup = Q_NULL);
    virtual ~RibbonRadioButtonControl();
public:
    QRadioButton *widget() const;
private:
    Q_DISABLE_COPY(RibbonRadioButtonControl)
};

QRIBBON_END_NAMESPACE
