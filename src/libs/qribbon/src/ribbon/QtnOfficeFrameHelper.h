/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2011 Developer Machines (http://www.devmachines.com)
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
#ifndef QTN_OFFICEFRAMEHELPER_H
#define QTN_OFFICEFRAMEHELPER_H

#include <QPixmap>
#include <QStyle>

#include "QtitanDef.h"

class QRect;
class QIcon;

namespace Qtitan
{
    class ContextHeader;
    class StyleOptionTitleBar;
    /* OfficeFrameHelper */

/*!
    \class Qtitan::OfficeFrameHelper
    \internal
*/
    class OfficeFrameHelper
    {
    public:
        virtual ~OfficeFrameHelper() 
        {
        }
    public:
        virtual void enableOfficeFrame(QWidget* parent) = 0;

        virtual bool isDwmEnabled() const = 0;
        virtual bool isActive() const = 0;
        virtual bool isMaximize() const = 0;

        virtual int getFrameBorder() const = 0;
        virtual int titleBarSize() const = 0;
        virtual int tabBarHeight() const = 0;

        virtual void setHeaderRect(const QRect& rcHeader) = 0;
        virtual void setContextHeader(QList<ContextHeader*>* listContextHeaders) = 0;

        virtual QPixmap getFrameSmallIcon() const = 0;
        virtual QString getSystemMenuString(uint item) const = 0;

        virtual void drawTitleBar(QPainter* painter, const StyleOptionTitleBar& opt) = 0;
        virtual void fillSolidRect(QPainter* painter, const QRect& rect, const QRegion& airRegion, QColor clr) = 0;
        virtual void drawDwmCaptionText(QPainter* painter, const QRect& rect, const QString& strWindowText, bool active) = 0;
        virtual QSize getSizeSystemIcon() const = 0;
#ifdef Q_OS_WIN
        virtual HICON getWindowIcon(const QIcon& icon) const = 0;
#endif // Q_OS_WIN
        virtual void resetWindowIcon() = 0;
        virtual void recalcFrameLayout() = 0;
        virtual void postRecalcFrameLayout() = 0;
        virtual void processClickedSubControl(QStyle::SubControl subControl) = 0;

    #ifdef Q_OS_WIN
        virtual bool winEvent(MSG* message, long* result) = 0;
    #endif // Q_OS_WIN

    };

}; //namespace Qtitan


#endif // QTN_OFFICEFRAMEHELPER_H
