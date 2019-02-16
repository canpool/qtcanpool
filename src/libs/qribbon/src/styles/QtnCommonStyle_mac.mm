/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2012 Developer Machines (http://www.devmachines.com)
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
#include <AppKit/NSScreen.h>

using namespace Qtitan;

float qtn_mac_get_scalefactor()
{
#if (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_7)
    return [[NSScreen mainScreen] backingScaleFactor];
#else
     return [[NSScreen mainScreen] userSpaceScaleFactor];
#endif
}

QPixmap StyleHelper::alphaBlend(const QPixmap& src)
{
    QImage imageSrc = src.toImage();
    QImage imageCopy = imageSrc.convertToFormat(QImage::Format_ARGB32);

    QRect rectSrc = imageCopy.rect(); 
    int w = rectSrc.width();
    int h = rectSrc.height();

    uchar* pixels = imageCopy.bits();

    uint mask = 0;
    QImage::Format imageFormat = QImage::Format_ARGB32_Premultiplied;

    // Make a QImage out of the QPixmap and add alpha
    QImage image(w, h, imageFormat);
    if (!image.isNull())
    {
        int bytes_per_line = w * sizeof(QRgb);
        for (int y = 0; y < h; ++y) 
        {
            QRgb* dest = (QRgb*) image.scanLine(y);
            const QRgb* src = (const QRgb *) (pixels + y * bytes_per_line);
            for (int x = 0; x < w; ++x) 
            {
                const uint pixel = src[x];
                if ((pixel & 0xff000000) == 0 && (pixel & 0x00ffffff) != 0)
                    dest[x] = pixel | 0xff000000;
                else
                    dest[x] = pixel | mask;
            }
        }
    }
    return QPixmap::fromImage(image, Qt::AutoColor);
}
