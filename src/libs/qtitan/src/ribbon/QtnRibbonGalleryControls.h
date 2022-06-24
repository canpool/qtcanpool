/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#ifndef QTN_RIBBONGALLERYCONTROLS_H
#define QTN_RIBBONGALLERYCONTROLS_H

#include "QtnRibbonGallery.h"
#include "QtnRibbonControls.h"
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE


/* RibbonGalleryControl */
class QTITAN_EXPORT RibbonGalleryControl : public RibbonWidgetControl
{
    Q_OBJECT
public:
    explicit RibbonGalleryControl(RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonGalleryControl();

public:
    RibbonGallery* widget() const;
    void setContentsMargins(int top, int bottom);

public:
    virtual QSize sizeHint() const;
    virtual void adjustCurrentSize(bool expand);
    virtual void sizeChanged(RibbonControlSizeDefinition::GroupSize size);

protected:
    virtual void resizeEvent(QResizeEvent*);

private:
    int m_marginTop; 
    int m_marginBottom;

private:
    Q_DISABLE_COPY(RibbonGalleryControl)
};


QTITAN_END_NAMESPACE


#endif // QTN_RIBBONGALLERYCONTROLS_H


