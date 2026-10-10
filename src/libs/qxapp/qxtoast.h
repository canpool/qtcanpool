/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTOAST_H
#define QXTOAST_H

#include "qxapp_global.h"

#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

QX_APP_BEGIN_NAMESPACE

class QxToastPrivate;

/*!
 * One short message, shown over the window it belongs to and gone a few seconds
 * later.
 *
 * A toast carries a Level - information, success, warning or error - which
 * picks the icon and the accent colour, and it carries the text. Everything
 * else about it is transient: it fades in, waits for its timeout, fades out and
 * is destroyed. Hovering holds the countdown back, so a message that arrived
 * while the pointer happened to be in the way does not vanish unread.
 *
 * Applications normally talk to QxToastManager rather than to this class:
 *
 * @code
 * QxToastManager *toasts = new QxToastManager(mainWindow);
 * toasts->show(tr("The document was saved"), QxToast::Success);
 * toasts->show(tr("No printer is configured"), QxToast::Warning, 8000);
 * @endcode
 *
 * The class is a plain child widget of its host rather than a top level window
 * of its own. That is deliberate: it needs no window manager to appear, the
 * host's stacking order puts it above the ribbon and the docks for free, and it
 * cannot take the keyboard focus away from the window the user is working in. A
 * Qt::ToolTip window would need a platform window of its own and would behave
 * differently under the offscreen platform and under WebAssembly, which is
 * exactly where this class has to keep working.
 *
 * The colours are read from the widget palette - the plate from
 * QPalette::ToolTipBase, its border from QPalette::Mid, the accent from
 * QPalette::Highlight - so a theme change reaches the toast without a line of
 * code here.
 */
class QX_APP_EXPORT QxToast : public QWidget
{
    Q_OBJECT
public:
    /*! What the message is about; it selects the icon and the accent colour. */
    enum Level {
        Information,
        Success,
        Warning,
        Error,
    };
    Q_ENUM(Level)

    /*! Milliseconds a toast stays on screen when the caller does not say. */
    static constexpr int DefaultTimeout = 4000;
    /*! Milliseconds the fade in and the fade out take when no one says otherwise. */
    static constexpr int DefaultFadeDuration = 160;
    /*!
     * Pixels of room the widget keeps around its plate for the drop shadow. A
     * caller that positions a toast by hand has to allow for it; the manager
     * does that itself.
     */
    static constexpr int ShadowMargin = 4;
    /*!
     * The narrowest and the widest plate a toast settles for, whatever width it
     * is given. A host narrower than MinPlateWidth wins over the minimum, since
     * a plate that does not fit is worse than a cramped one.
     */
    static constexpr int MinPlateWidth = 160;
    static constexpr int MaxPlateWidth = 420;

    explicit QxToast(const QString &text, Level level = Information, QWidget *parent = Q_NULLPTR);
    ~QxToast() override;

    QString text() const;
    /*! Replaces the message; sizeHintChanged() follows when the size changed. */
    void setText(const QString &text);

    Level level() const;
    void setLevel(Level level);

    /*!
     * Milliseconds left before the toast dismisses itself, zero or less when it
     * is to stay until something dismisses it.
     */
    int timeout() const;
    void setTimeout(int timeoutMs);

    int fadeDuration() const;
    /*! Zero turns the animation off, which makes the life cycle immediate. */
    void setFadeDuration(int ms);

    /*! Whether the pointer is holding the countdown back right now. */
    bool isPaused() const;
    /*! True from the moment dismiss() is called until the widget is gone. */
    bool isDismissing() const;

    /*!
     * The colour the level is drawn with.
     *
     * Qt has no palette role for "success" or "warning", and writing four
     * colours down here would freeze the toast against every theme. The theme's
     * highlight colour therefore supplies saturation and lightness - so the
     * accent stays as loud as the theme it runs under - and only the hue moves
     * to the corner of the wheel the level is read in: the theme's own hue for
     * information, green for success, amber for warning, red for error.
     */
    QColor accentColor() const;

    QSize sizeHint() const override;
    /*! Height of the whole widget when it is \a width wide, text wrapping included. */
    int heightForWidth(int width) const override;

public Q_SLOTS:
    /*!
     * Puts the toast on screen, fades it in and starts the countdown. The
     * manager calls this; calling it twice restarts the countdown.
     */
    void start();
    /*! Stops the countdown and leaves the toast on screen. */
    void stop();
    /*!
     * Takes the toast off screen: the countdown stops, the fade out runs and
     * dismissed() follows once the widget is hidden. Calling it again on a toast
     * that is already leaving changes nothing.
     */
    void dismiss();

Q_SIGNALS:
    /*! The toast wants to leave; emitted before the fade out starts. */
    void dismissRequested();
    /*! The toast is off screen. Its owner deletes the widget. */
    void dismissed();
    /*! The user clicked the toast. */
    void clicked();
    /*! The preferred size changed and whoever placed the toast should look again. */
    void sizeHintChanged();
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool event(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    Q_DISABLE_COPY(QxToast)
    QX_DECLARE_PRIVATE(QxToast)
};

QX_APP_END_NAMESPACE

#endif   // QXTOAST_H
