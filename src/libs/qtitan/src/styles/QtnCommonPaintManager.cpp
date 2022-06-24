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
#include "QtnCommonStylePrivate.h"
#ifdef DEBUG_MEMORY_ENABLED
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

/*!
\class Qtitan::CommonPaintManager
\internal
*/
CommonPaintManager::CommonPaintManager(CommonStyle* baseStyle)
    : m_base(baseStyle)
{
}

CommonPaintManager::~CommonPaintManager()
{
}

CommonStyle* CommonPaintManager::baseStyle() const
{
    return m_base;
}

StyleHelper& CommonPaintManager::helper() const
{
    return baseStyle()->helper();
}

QPixmap CommonPaintManager::cached(const QString& img) const
{
    return baseStyle()->cached(img);
}

QPixmap CommonPaintManager::cachedPath(const QString& img) const
{
    return baseStyle()->CommonStyle::cached(img);
}

QRect CommonPaintManager::sourceRectImage(QRect rcSrc, int state, int count) const
{
    return baseStyle()->sourceRectImage(rcSrc, state, count);
}

void CommonPaintManager::drawImage(const QPixmap& soSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, 
                QRect rcSizingMargins, QColor clrTransparent, bool alphaBlend) const
{
    baseStyle()->drawImage(soSrc, p, rcDest, rcSrc, rcSizingMargins, clrTransparent, alphaBlend);
}

void CommonPaintManager::drawImage(const QPixmap& dcSrc, QPainter& p, const QRect& rcDest, 
                                   const QRect& rcSrc, QRect rcSizingMargins, bool alphaBlend) const
{
    baseStyle()->drawImage(dcSrc, p, rcDest, rcSrc, rcSizingMargins, alphaBlend);
}

void CommonPaintManager::drawPixmap(const QPixmap& dcSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, 
                 bool alpha, QRect rcSizingMargins, bool alphaBlend) const
{
    baseStyle()->drawPixmap(dcSrc, p, rcDest, rcSrc, alpha, rcSizingMargins, alphaBlend);
}
