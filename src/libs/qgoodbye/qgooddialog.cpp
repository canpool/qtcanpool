/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2022-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#include "qgooddialog.h"
#include "qgood/qgoodwindow.h"

#ifdef Q_OS_MAC
#include "macosnative.h"
#endif

#include <QApplication>
#include <QDialog>
#include <QScreen>
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include <QWindow>
#include <QtMath>

namespace QGoodDialogUtils
{
static bool m_dialogIsVisible = false;
}

QGoodDialog::QGoodDialog(QDialog *dialog, QGoodWindow *childGw, QGoodWindow *parentGw) : QObject()
{
    m_dialog = dialog;
    m_childGw = childGw;
    m_parentGw = parentGw;
}

int QGoodDialog::exec()
{
    if (!m_dialog)
        return QDialog::Rejected;
    if (!m_childGw)
        return QDialog::Rejected;
    if (!m_parentGw)
        return QDialog::Rejected;
    if (QGoodDialogUtils::m_dialogIsVisible)
        return QDialog::Rejected;

    QGoodDialogUtils::m_dialogIsVisible = true;

    m_childGw->installEventFilter(this);
    m_dialog->installEventFilter(this);

#ifdef Q_OS_WIN
    m_dialog->setWindowModality(Qt::NonModal);
#endif
#ifdef Q_OS_LINUX
    m_childGw->setWindowModality(Qt::ApplicationModal);
#endif
#ifdef Q_OS_MAC
    m_childGw->setWindowModality(Qt::WindowModal);
#endif

#if defined Q_OS_WIN || defined Q_OS_LINUX
    bool is_message_box = qobject_cast<QMessageBox *>(m_dialog);
    bool is_input_dialog = qobject_cast<QInputDialog *>(m_dialog);

    auto func_center = [=] {
        QScreen *parent_screen = m_parentGw->windowHandle()->screen();

        qreal pixel_ratio = parent_screen->devicePixelRatio();
        QRect screen_geom = parent_screen->availableGeometry();
        screen_geom.moveTop(qFloor(screen_geom.top() / pixel_ratio));
        screen_geom.moveLeft(qFloor(screen_geom.left() / pixel_ratio));

        QRect child_geom;
        child_geom.setSize(m_childGw->size());
        child_geom.moveCenter(m_parentGw->geometry().center());

        if (child_geom.left() < screen_geom.left())
            child_geom.moveLeft(screen_geom.left());
        else if (child_geom.right() > screen_geom.right())
            child_geom.moveRight(screen_geom.right());

        if (child_geom.top() < screen_geom.top())
            child_geom.moveTop(screen_geom.top());
        else if (child_geom.bottom() > screen_geom.bottom())
            child_geom.moveBottom(screen_geom.bottom());

        m_childGw->setGeometry(child_geom);
    };

    auto func_fixed_size = [=] {
        if (is_message_box || is_input_dialog) {
            m_childGw->setFixedSize(m_childGw->sizeHint());
        } else {
            if (m_dialog->minimumSize() == m_dialog->maximumSize()) {
                m_childGw->setFixedSize(m_childGw->sizeHint());
            } else {
                m_childGw->setMinimumSize(m_dialog->minimumSize());
                m_childGw->setMaximumSize(m_dialog->maximumSize());
            }
        }
    };

    QTimer::singleShot(0, m_childGw, func_center);
    func_fixed_size();
#endif

    if (!m_dialog->windowTitle().isNull())
        m_childGw->setWindowTitle(m_dialog->windowTitle());
    else
        m_childGw->setWindowTitle(qApp->applicationName());

    if (!m_dialog->windowIcon().isNull())
        m_childGw->setWindowIcon(m_dialog->windowIcon());
    else
        m_childGw->setWindowIcon(m_parentGw->windowIcon());

    if (m_parentGw->isMinimized())
        m_parentGw->showNormal();

    QTimer::singleShot(0, m_childGw, &QGoodWindow::show);

    m_loop.exec();

    m_dialog->setParent(nullptr);
    m_childGw->setParent(nullptr);

    QGoodDialogUtils::m_dialogIsVisible = false;

    return m_dialog->result();
}

int QGoodDialog::exec(QDialog *dialog, QGoodWindow *childGw, QGoodWindow *parentGw)
{
#ifdef QGOOD_WINDOW_ENABLE
    QGoodDialog good_dialog(dialog, childGw, parentGw);
    return good_dialog.exec();
#else
    Q_UNUSED(childGw)
    Q_UNUSED(parentGw)
    return dialog->exec();
#endif
}

bool QGoodDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_childGw) {
        switch (event->type()) {
        case QEvent::Show: {
#ifdef Q_OS_WIN
            HWND hwnd_gw = HWND(m_childGw->winId());

            for (QWindow *w : qApp->topLevelWindows()) {
                // Prevent problems with fullscreen mode.
                if (w->type() == Qt::Window)
                    continue;

                HWND hwnd = HWND(w->winId());
                if (hwnd == hwnd_gw || IsChild(hwnd_gw, hwnd))
                    continue;

                if (m_windowList.contains(w))
                    continue;
                m_windowList.append(w);

                EnableWindow(hwnd, FALSE);
            }
#endif // Q_OS_WIN
#ifdef Q_OS_MAC
            for (QWindow *w : qApp->topLevelWindows()) {
                if (!w->isVisible())
                    continue;
                if (w == m_childGw->windowHandle())
                    continue;
                if (w == m_parentGw->windowHandle())
                    continue;
                if (w == m_dialog->windowHandle())
                    continue;
                m_windowList.append(w);
                w->setModality(Qt::WindowModal);
            }
            macOSNative::setStyle(long(m_parentGw->winId()), macOSNative::StyleType::Disabled);
#endif // Q_OS_MAC
            break;
        }
        case QEvent::Close: {
            QTimer::singleShot(0, m_parentGw, &QGoodWindow::activateWindow);
            m_loop.quit();
#ifdef Q_OS_WIN
            for (QWindow *w : m_windowList) {
                HWND hwnd = HWND(w->winId());
                EnableWindow(hwnd, TRUE);
            }
            m_windowList.clear();
#endif // Q_OS_WIN
#ifdef Q_OS_MAC
            for (QWindow *w : m_windowList) {
                w->setModality(Qt::NonModal);
            }
            m_windowList.clear();
            QTimer::singleShot(500, this, [=] {
                macOSNative::setStyle(long(m_parentGw->winId()), macOSNative::StyleType::NoState);
            });
#endif // Q_OS_MAC
            break;
        }
        default:
            break;
        }
    } else if (watched == m_dialog) {
        switch (event->type()) {
        case QEvent::Show: {
            if (m_dialog)
                m_dialog->activateWindow();
            break;
        }
        case QEvent::Hide: {
            if (m_childGw)
                m_childGw->close();
            break;
        }
        default:
            break;
        }
    }

    return QObject::eventFilter(watched, event);
}
