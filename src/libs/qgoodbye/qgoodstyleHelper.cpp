/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2022-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifdef _WIN32

#ifdef QT_VERSION_QT6
#include <windows.h>
#include <shellapi.h>
#endif

#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define NTDDI_VERSION NTDDI_VISTA
#define _WIN32_WINNT  _WIN32_WINNT_VISTA

#ifdef QT_VERSION_QT5
#include <windows.h>
#include <shellapi.h>
#endif

#include "qgoodstyleHelper.h"
#include <QLibrary>

#ifdef QT_VERSION_QT5
#include <QtWin>
#endif

QPixmap QGoodStyleHelper::winStandardPixmap(QStyle::StandardPixmap standardPixmap)
{
    QPixmap pixmap;

    HICON hicon = nullptr;

    LPCWSTR icon_handle_xp = nullptr;
    SHSTOCKICONID icon_handle_vista = SIID_INVALID;

    switch (standardPixmap) {
    case QStyle::SP_MessageBoxInformation: {
        icon_handle_xp = IDI_INFORMATION;
        icon_handle_vista = SIID_INFO;
        break;
    }
    case QStyle::SP_MessageBoxWarning: {
        icon_handle_xp = IDI_WARNING;
        icon_handle_vista = SIID_WARNING;
        break;
    }
    case QStyle::SP_MessageBoxCritical: {
        icon_handle_xp = IDI_ERROR;
        icon_handle_vista = SIID_ERROR;
        break;
    }
    case QStyle::SP_MessageBoxQuestion: {
        icon_handle_xp = IDI_QUESTION;
        icon_handle_vista = SIID_HELP;
        break;
    }
    default:
        break;
    }

    typedef HRESULT(STDAPICALLTYPE * tSHGetStockIconInfo)(SHSTOCKICONID siid, UINT uFlags, SHSTOCKICONINFO * psii);
    tSHGetStockIconInfo pSHGetStockIconInfo = tSHGetStockIconInfo(QLibrary::resolve("shell32", "SHGetStockIconInfo"));

    if (pSHGetStockIconInfo) {
        if (icon_handle_vista != SIID_INVALID) {
            SHSTOCKICONINFO sii;
            memset(&sii, 0, sizeof(SHSTOCKICONINFO));
            sii.cbSize = sizeof(sii);

            if (SUCCEEDED(pSHGetStockIconInfo(icon_handle_vista, SHGSI_ICON | SHGSI_LARGEICON, &sii))) {
                hicon = sii.hIcon;
            }
        }
    } else {
        if (icon_handle_xp) {
            hicon = LoadIconW(nullptr, icon_handle_xp);
        }
    }

    if (hicon) {
#ifdef QT_VERSION_QT5
        pixmap = QtWin::fromHICON(hicon);
#endif
#ifdef QT_VERSION_QT6
        pixmap = QPixmap::fromImage(QImage::fromHICON(hicon));
#endif
        DestroyIcon(hicon);
    }

    return pixmap;
}

#endif
