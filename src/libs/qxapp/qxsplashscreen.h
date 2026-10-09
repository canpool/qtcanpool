/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXSPLASHSCREEN_H
#define QXSPLASHSCREEN_H

#include "qxapp_global.h"

#include <QtCore/QString>
#include <QtGui/QPixmap>
#include <QtWidgets/QSplashScreen>

QT_BEGIN_NAMESPACE
class QPainter;
QT_END_NAMESPACE

QX_APP_BEGIN_NAMESPACE

class QxSplashScreenPrivate;

/*!
 * A splash screen that reports how far the start-up has come.
 *
 * QSplashScreen can already show a message line, but an application that loads
 * settings, a theme and a layout wants a progress bar next to it, and wants the
 * splash to keep repainting while that work blocks the event loop.
 * QxSplashScreen adds both:
 *
 * @code
 * QxSplashScreen splash(logo, QStringLiteral("MyApp"), QStringLiteral("3.0"));
 * splash.show();
 * splash.step(20, tr("Loading settings..."));
 * // ... more start-up work ...
 * splash.step(100, tr("Ready"));
 * splash.finish(&mainWindow);
 * @endcode
 *
 * The application name, the version, the message and the progress are drawn over
 * the bottom of the artwork, so one logo image is enough. A null logo is
 * replaced by a plain plate, which keeps the splash usable in tests and in
 * applications that have no artwork yet.
 */
class QX_APP_EXPORT QxSplashScreen : public QSplashScreen
{
    Q_OBJECT
public:
    explicit QxSplashScreen(const QPixmap &logo = QPixmap(), const QString &applicationName = QString(),
                            const QString &version = QString(), Qt::WindowFlags flags = Qt::WindowFlags());
    ~QxSplashScreen() override;

    QString applicationName() const;
    QString version() const;
    QString message() const;
    /*! Progress in percent, or -1 while it is unknown. */
    int progress() const;

public Q_SLOTS:
    void setApplicationName(const QString &applicationName);
    void setVersion(const QString &version);
    void setMessage(const QString &message);
    /*! Sets the progress to 0..100 percent; a negative value means "unknown". */
    void setProgress(int progress);
    /*!
     * Sets the progress and the message in one call, then processes pending
     * events so that the splash really updates while the start-up work keeps
     * running on the main thread. An empty message keeps the current one.
     */
    void step(int progress, const QString &message = QString());
protected:
    void drawContents(QPainter *painter) override;
private:
    Q_DISABLE_COPY(QxSplashScreen)
    QX_DECLARE_PRIVATE(QxSplashScreen)
};

QX_APP_END_NAMESPACE

#endif   // QXSPLASHSCREEN_H
