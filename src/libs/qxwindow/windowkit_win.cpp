/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
 **/
#include "windowkit_win.h"

QX_WINDOW_BEGIN_NAMESPACE

static RTL_OSVERSIONINFOW GetRealOSVersionImpl()
{
    HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
    using RtlGetVersionPtr = NTSTATUS(WINAPI *)(PRTL_OSVERSIONINFOW);
    // GetProcAddress returns FARPROC; converting it to the actual signature is the
    // documented usage, but GCC's -Wcast-function-type flags it anyway.
    QT_WARNING_PUSH
    QT_WARNING_DISABLE_GCC("-Wcast-function-type")
    auto pRtlGetVersion = reinterpret_cast<RtlGetVersionPtr>(::GetProcAddress(hMod, "RtlGetVersion"));
    QT_WARNING_POP
    RTL_OSVERSIONINFOW rovi{};
    rovi.dwOSVersionInfoSize = sizeof(rovi);
    pRtlGetVersion(&rovi);
    return rovi;
}

namespace Private
{

RTL_OSVERSIONINFOW GetRealOSVersion()
{
    static const auto result = GetRealOSVersionImpl();
    return result;
}

}

QX_WINDOW_END_NAMESPACE
