/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"
#include "titlebutton.h"

#include "qxwindow/windowagentwidget.h"

#include <QAction>
#include <QActionGroup>
#include <QtCore/QFile>
#include <QtCore/QSignalBlocker>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QCursor>
#include <QtGui/QHoverEvent>
#include <QtGui/QScreen>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStyle>

QX_WINDOW_USE_NAMESPACE

// The four buttons and the title label are all this tall, which is also what the
// style sheets assume.
static constexpr int kTitleBarHeight = 32;

// The status line is a clock, so the window has something to repaint. It is also
// what makes the window materials visible: a material is drawn behind the window,
// and the central widget has to leave the background alone for it to be seen.
class ClockWidget : public QLabel
{
public:
    explicit ClockWidget(QWidget *parent = nullptr)
        : QLabel(parent)
    {
        setAlignment(Qt::AlignCenter);
        startTimer(100);
    }
protected:
    void timerEvent(QTimerEvent *event) override
    {
        Q_UNUSED(event);
        setText(QTime::currentTime().toString(QStringLiteral("hh:mm:ss")));
    }
};

/*
 * Sends the leave event the button under the mouse cursor should have received.
 *
 * Qt only sends it on mouse movement, and maximising from the maximize button
 * replaces the window without moving the cursor, so the button keeps the hovered
 * state it had until the mouse is moved again. Nothing in the window is at fault:
 * the button really is still under the cursor, and only the window underneath it
 * changed. That is why the fix is to post the events by hand rather than to
 * repaint.
 */
static inline void emulateLeaveEvent(QWidget *widget)
{
    Q_ASSERT(widget);
    if (!widget) {
        return;
    }
    QTimer::singleShot(0, widget, [widget]() {
        const QScreen *screen = widget->screen();
        const QPoint globalPos = QCursor::pos(screen);
        if (QRect(widget->mapToGlobal(QPoint{0, 0}), widget->size()).contains(globalPos)) {
            return;
        }

        QCoreApplication::postEvent(widget, new QEvent(QEvent::Leave));
        if (!widget->testAttribute(Qt::WA_Hover)) {
            return;
        }

        const QPoint localPos = widget->mapFromGlobal(globalPos);
        const QPoint scenePos = widget->window()->mapFromGlobal(globalPos);
        static constexpr const auto oldPos = QPoint{};
        const Qt::KeyboardModifiers modifiers = QGuiApplication::keyboardModifiers();
        // QHoverEvent grew its global position in 6.3 and swapped the local
        // position for the scene position in 6.4, so the arguments are the same
        // three points in a different order on each side of those two versions.
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
        const auto event = new QHoverEvent(QEvent::HoverLeave, scenePos, globalPos, oldPos, modifiers);
        Q_UNUSED(localPos);
#elif QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
        const auto event = new QHoverEvent(QEvent::HoverLeave, localPos, globalPos, oldPos, modifiers);
        Q_UNUSED(scenePos);
#else
        const auto event = new QHoverEvent(QEvent::HoverLeave, localPos, oldPos, modifiers);
        Q_UNUSED(scenePos);
#endif
        QCoreApplication::postEvent(widget, event);
    });
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    installWindowAgent();

    auto *clockWidget = new ClockWidget();
    clockWidget->setObjectName(QStringLiteral("clock-widget"));
    clockWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCentralWidget(clockWidget);

    loadStyleSheet(Dark);

    setWindowTitle(tr("QxWindow Demo"));
    resize(800, 600);
}

bool MainWindow::event(QEvent *event)
{
    // The style sheets colour the bar by whether the window is active, and the
    // property is set here because a style sheet cannot select on the active
    // state of a window.
    switch (event->type()) {
    case QEvent::WindowActivate:
    case QEvent::WindowDeactivate: {
        m_titleBar->setProperty("bar-active", event->type() == QEvent::WindowActivate);
        style()->polish(m_titleBar);
        break;
    }

    default:
        break;
    }
    return QMainWindow::event(event);
}

void MainWindow::buildTitleBar()
{
    m_titleBar = new QFrame(this);
    m_titleBar->setObjectName(QStringLiteral("win-title-bar"));
    m_titleBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    auto *layout = new QHBoxLayout(m_titleBar);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    const auto systemButton = [this](const QString &name, const QString &icon) {
        auto *button = new TitleButton(m_titleBar);
        button->setObjectName(name);
        button->setIcon(QIcon(icon));
        button->setIconSize(QSize(12, 12));
        button->setFixedSize(46, kTitleBarHeight);
        return button;
    };

#ifndef Q_OS_MAC
    m_iconButton = systemButton(QStringLiteral("icon-button"), QStringLiteral(":/res/window-bar/app-icon.png"));
    m_iconButton->setIconSize(QSize(18, 18));
    m_iconButton->setFixedWidth(40);
    layout->addWidget(m_iconButton);
#endif

    // The menu bar goes into the bar rather than into the window, which is what
    // makes it part of the caption - and the reason the agent later has to be told
    // that it stays clickable.
    m_menuBar = buildMenuBar();
    layout->addWidget(m_menuBar);

    m_titleLabel = new QLabel(m_titleBar);
    m_titleLabel->setObjectName(QStringLiteral("win-title-label"));
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setFixedHeight(kTitleBarHeight);
    layout->addWidget(m_titleLabel, 1);

    connect(this, &QWidget::windowTitleChanged, m_titleLabel, &QLabel::setText);
    m_titleLabel->setText(windowTitle());

#ifndef Q_OS_MAC
    m_minButton = systemButton(QStringLiteral("min-button"), QStringLiteral(":/res/window-bar/minimize.svg"));
    m_maxButton = systemButton(QStringLiteral("max-button"), QStringLiteral(":/res/window-bar/maximize.svg"));
    m_closeButton = systemButton(QStringLiteral("close-button"), QStringLiteral(":/res/window-bar/close.svg"));

    // The maximize button shows the state it will produce rather than the state
    // the window is in: a checked button is an invitation to restore.
    m_maxButton->setCheckable(true);
    connect(m_maxButton, &QAbstractButton::toggled, this, [this](bool checked) {
        m_maxButton->setIcon(QIcon(checked ? QStringLiteral(":/res/window-bar/restore.svg")
                                           : QStringLiteral(":/res/window-bar/maximize.svg")));
    });

    layout->addWidget(m_minButton);
    layout->addWidget(m_maxButton);
    layout->addWidget(m_closeButton);
#endif
}

QMenuBar *MainWindow::buildMenuBar()
{
    auto *menuBar = new QMenuBar(m_titleBar);
    menuBar->setObjectName(QStringLiteral("win-menu-bar"));

    auto *file = new QMenu(tr("File(&F)"), menuBar);
    file->addAction(tr("New(&N)"));
    file->addAction(tr("Open(&O)"));
    file->addSeparator();
    file->addAction(tr("Exit(&X)"));

    auto *edit = new QMenu(tr("Edit(&E)"), menuBar);
    edit->addAction(tr("Undo(&U)"));
    edit->addAction(tr("Redo(&R)"));

    auto *settings = new QMenu(tr("Settings(&S)"), menuBar);

    // The window is drawn from one of two style sheets. Switching them is how a
    // material below can be told apart from the plain background.
    auto *darkAction = new QAction(tr("Dark theme"), settings);
    darkAction->setCheckable(true);
    darkAction->setChecked(true);
    connect(darkAction, &QAction::toggled, this, [this](bool checked) {
        loadStyleSheet(checked ? Dark : Light);
    });
    settings->addAction(darkAction);

#ifdef Q_OS_WIN
    // The materials are alternatives to the background, and applying one replaces
    // whatever backdrop was there before, so they are not mutually exclusive:
    // more than one can be checked, and the last one applied is the one on screen.
    //
    // A material the system does not have answers false - mica needs Windows 11,
    // mica alt and acrylic need 22H2 - and the checkmark then goes back rather
    // than claiming an effect that is not there.
    settings->addSeparator();
    const struct {
        const char *key;
        QString label;
    } materials[] = {
        {"dwm-blur", tr("DWM blur")},
        {"acrylic-material", tr("Acrylic material")},
        {"mica", tr("Mica")},
        {"mica-alt", tr("Mica alt")},
    };
    for (const auto &material : materials) {
        const QString key = QString::fromLatin1(material.key);
        auto *action = new QAction(material.label, settings);
        action->setCheckable(true);
        connect(action, &QAction::toggled, this, [this, action, key](bool checked) {
            if (!m_windowAgent->setWindowAttribute(key, checked)) {
                QSignalBlocker blocker(action);
                action->setChecked(false);
                return;
            }
            // A material is drawn behind the window, so the window and its
            // central widget have to stop painting an opaque background over it.
            setProperty("custom-style", checked);
            style()->polish(this);
        });
        settings->addAction(action);
    }
#elif defined(Q_OS_MAC)
    // macOS has one effect with three settings, so these are mutually exclusive.
    settings->addSeparator();
    const struct {
        const char *value;
        QString label;
        bool custom;
    } blurEffects[] = {
        {"dark", tr("Dark blur"), true},
        {"light", tr("Light blur"), true},
        {"none", tr("No blur"), false},
    };
    auto *blurGroup = new QActionGroup(settings);
    for (const auto &effect : blurEffects) {
        const QString value = QString::fromLatin1(effect.value);
        const bool custom = effect.custom;
        auto *action = new QAction(effect.label, settings);
        action->setCheckable(true);
        connect(action, &QAction::toggled, this, [this, action, value, custom](bool checked) {
            // Only the action that was turned on does anything: the one that was
            // turned off would undo the setting its successor just made.
            if (!checked) {
                return;
            }
            if (!m_windowAgent->setWindowAttribute(QStringLiteral("blur-effect"), value)) {
                QSignalBlocker blocker(action);
                action->setChecked(false);
                return;
            }
            setProperty("custom-style", custom);
            style()->polish(this);
        });
        blurGroup->addAction(action);
        settings->addAction(action);
    }
#endif

    menuBar->addMenu(file);
    menuBar->addMenu(edit);
    menuBar->addMenu(settings);
    return menuBar;
}

void MainWindow::installWindowAgent()
{
    // 1. Take the window over. From here on it draws no system frame: the agent
    //    answers the hit test that decides which part of the client area still
    //    behaves as border, as caption and as system button.
    m_windowAgent = new WindowAgentWidget(this);
    m_windowAgent->setup(this);

    // 2. Construct the title bar. It is an ordinary widget and the agent is told
    //    which one it is, nothing more.
    buildTitleBar();
    setMenuWidget(m_titleBar);

    m_windowAgent->setTitleBar(m_titleBar);

    // 3. Hand over the system buttons. Handing them over is what keeps them
    //    clickable: the hit test calls everything else in the bar the caption, and
    //    a press there starts a window move instead of reaching the button.
#ifndef Q_OS_MAC
    m_windowAgent->setSystemButton(WindowAgentBase::WindowIcon, m_iconButton);
    m_windowAgent->setSystemButton(WindowAgentBase::Minimize, m_minButton);
    m_windowAgent->setSystemButton(WindowAgentBase::Maximize, m_maxButton);
    m_windowAgent->setSystemButton(WindowAgentBase::Close, m_closeButton);
#endif

    // The menu bar is the one widget inside the caption that has to stay live.
    m_windowAgent->setHitTestVisible(m_menuBar, true);

#ifdef Q_OS_MAC
    // macOS reserves a corner of the title bar for the three buttons it draws
    // itself, and this is how wide that corner is. Without it the hit test and
    // the screen disagree about where the buttons are.
    m_windowAgent->setSystemButtonAreaCallback([](const QSize &size) {
        static constexpr const int width = 75;
        return QRect(QPoint(size.width() - width, 0), QSize(width, size.height()));
    });
#endif

#ifndef Q_OS_MAC
    connect(m_minButton, &QAbstractButton::clicked, this, &QWidget::showMinimized);
    connect(m_maxButton, &QAbstractButton::clicked, this, [this]() {
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
        emulateLeaveEvent(m_maxButton);
    });
    connect(m_closeButton, &QAbstractButton::clicked, this, &QWidget::close);
#endif

#ifdef Q_OS_WINDOWS
    // The window icon behaves the way the system's does: one click opens the
    // system menu, two close the window. A click cannot tell the two apart, so
    // the menu is put on a short delay that the double click cancels.
    connect(m_iconButton, &QAbstractButton::clicked, this, [this]() {
        m_iconButton->setProperty("double-click-close", false);
        QTimer::singleShot(75, this, [this]() {
            if (m_iconButton->property("double-click-close").toBool()) {
                return;
            }
            m_windowAgent->showSystemMenu(m_iconButton->mapToGlobal(QPoint{0, m_iconButton->height()}));
        });
    });
    connect(m_iconButton, &TitleButton::doubleClicked, this, [this]() {
        m_iconButton->setProperty("double-click-close", true);
        close();
    });
#endif
}

void MainWindow::loadStyleSheet(Theme theme)
{
    if (m_theme == theme && !styleSheet().isEmpty()) {
        return;
    }

    const QString path =
        theme == Dark ? QStringLiteral(":/res/dark-style.css") : QStringLiteral(":/res/light-style.css");
    QFile qss(path);
    if (!qss.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    m_theme = theme;
    setStyleSheet(QString::fromUtf8(qss.readAll()));
}
