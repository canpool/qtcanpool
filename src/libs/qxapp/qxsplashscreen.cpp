/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxsplashscreen.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QEventLoop>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QPainter>

QX_APP_BEGIN_NAMESPACE

namespace
{

/*! Height of the band that carries the name, the message and the progress. */
const int kBandHeight = 46;
/*! Horizontal margin used inside that band. */
const int kBandMargin = 14;
/*! Height of the title and the message lines. */
const int kLineHeight = 18;

/*!
 * A splash without artwork still has to show the progress, so a null logo is
 * replaced by a plain plate the size of a small logo.
 */
QPixmap prepareLogo(const QPixmap &logo)
{
    if (!logo.isNull() && !logo.size().isEmpty()) {
        return logo;
    }
    QPixmap fallback(480, 300);
    fallback.fill(QColor(0x30, 0x30, 0x30));
    return fallback;
}

}   // namespace

class QxSplashScreenPrivate
{
    QX_DECLARE_PUBLIC(QxSplashScreen)
public:
    QxSplashScreenPrivate();
public:
    QString m_applicationName;
    QString m_version;
    QString m_message;
    int m_progress;
};

QxSplashScreenPrivate::QxSplashScreenPrivate()
    : m_progress(-1)
{
}

QxSplashScreen::QxSplashScreen(const QPixmap &logo, const QString &applicationName, const QString &version,
                               Qt::WindowFlags flags)
    : QSplashScreen(prepareLogo(logo), flags)
    , d_ptr(new QxSplashScreenPrivate())
{
    d_ptr->setPublic(this);
    Q_D(QxSplashScreen);
    d->m_applicationName = applicationName;
    d->m_version = version;
}

QxSplashScreen::~QxSplashScreen()
{
    QX_FINI_PRIVATE();
}

QString QxSplashScreen::applicationName() const
{
    Q_D(const QxSplashScreen);
    return d->m_applicationName;
}

QString QxSplashScreen::version() const
{
    Q_D(const QxSplashScreen);
    return d->m_version;
}

QString QxSplashScreen::message() const
{
    Q_D(const QxSplashScreen);
    return d->m_message;
}

int QxSplashScreen::progress() const
{
    Q_D(const QxSplashScreen);
    return d->m_progress;
}

void QxSplashScreen::setApplicationName(const QString &applicationName)
{
    Q_D(QxSplashScreen);
    if (d->m_applicationName == applicationName) {
        return;
    }
    d->m_applicationName = applicationName;
    repaint();
}

void QxSplashScreen::setVersion(const QString &version)
{
    Q_D(QxSplashScreen);
    if (d->m_version == version) {
        return;
    }
    d->m_version = version;
    repaint();
}

void QxSplashScreen::setMessage(const QString &message)
{
    Q_D(QxSplashScreen);
    if (d->m_message == message) {
        return;
    }
    d->m_message = message;
    repaint();
}

void QxSplashScreen::setProgress(int progress)
{
    Q_D(QxSplashScreen);
    const int bounded = progress < 0 ? -1 : qBound(0, progress, 100);
    if (d->m_progress == bounded) {
        return;
    }
    d->m_progress = bounded;
    repaint();
}

void QxSplashScreen::step(int progress, const QString &message)
{
    if (!message.isEmpty()) {
        setMessage(message);
    }
    setProgress(progress);

    // The caller is blocking the event loop with start-up work, so the splash
    // has to be driven from here. User input stays queued: the window is not
    // ready to react yet.
    if (QCoreApplication::instance()) {
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
}

void QxSplashScreen::drawContents(QPainter *painter)
{
    Q_D(QxSplashScreen);

    const QRect area = rect();
    if (area.isEmpty()) {
        return;
    }

    const int bandHeight = qMin(kBandHeight, area.height());
    const QRect band(0, area.height() - bandHeight, area.width(), bandHeight);
    painter->fillRect(band, QColor(0, 0, 0, 150));

    const int margin = qMin(kBandMargin, area.width() / 4);
    const QRect line(band.left() + margin, band.top() + 4, qMax(0, band.width() - 2 * margin), kLineHeight);

    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(QColor(0xff, 0xff, 0xff));

    QString title = d->m_applicationName;
    if (!d->m_version.isEmpty()) {
        title += QStringLiteral(" ");
        title += d->m_version;
    }
    painter->drawText(line, Qt::AlignLeft | Qt::AlignVCenter, title);

    font.setBold(false);
    painter->setFont(font);
    painter->setPen(QColor(0xd0, 0xd0, 0xd0));

    const QRect messageLine(line.left(), line.top() + kLineHeight, line.width(), kLineHeight);
    painter->drawText(messageLine, Qt::AlignLeft | Qt::AlignVCenter, d->m_message);

    if (d->m_progress < 0) {
        return;
    }

    painter->setPen(QColor(0xff, 0xff, 0xff));
    painter->drawText(messageLine, Qt::AlignRight | Qt::AlignVCenter, QStringLiteral("%1%").arg(d->m_progress));

    const QRect track(band.left() + margin, band.bottom() - 3, qMax(0, band.width() - 2 * margin), 2);
    painter->fillRect(track, QColor(0xff, 0xff, 0xff, 60));
    QRect fill = track;
    fill.setWidth(track.width() * d->m_progress / 100);
    painter->fillRect(fill, QColor(0x2f, 0x7c, 0xd1));
}

QX_APP_END_NAMESPACE
