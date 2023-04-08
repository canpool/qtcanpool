/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include "ribboncontrols.h"
#include "ribbongallery.h"

QRIBBON_BEGIN_NAMESPACE

/* RibbonGalleryControl */
class QRIBBON_EXPORT RibbonGalleryControl : public RibbonWidgetControl
{
    Q_OBJECT
public:
    explicit RibbonGalleryControl(RibbonGroup *parentGroup = Q_NULL);
    virtual ~RibbonGalleryControl();
public:
    RibbonGallery *widget() const;
    void setContentsMargins(int top, int bottom);
public:
    virtual QSize sizeHint() const;
    virtual void adjustCurrentSize(bool expand);
    virtual void sizeChanged(RibbonControlSizeDefinition::GroupSize size);
protected:
    virtual void resizeEvent(QResizeEvent *);
private:
    int m_marginTop;
    int m_marginBottom;
private:
    Q_DISABLE_COPY(RibbonGalleryControl)
};

QRIBBON_END_NAMESPACE
