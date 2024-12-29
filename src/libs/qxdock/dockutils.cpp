/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockutils.h"
#include "dockmanager.h"
#include "docksplitter.h"

#include <QPainter>
#include <QSplitter>
#include <QDebug>

#ifdef QX_DOCK_DEBUG
#include <QBoxLayout>
#include "dockcontainer.h"
#include "dockpanel.h"
#include "dockwidget.h"
#include "dockfloatingcontainer.h"
#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
#include <QSettings>
#include <QFile>
#include <QApplication>
#include <qpa/qplatformnativeinterface.h>
#endif

QX_DOCK_BEGIN_NAMESPACE

namespace internal
{
const int FloatingWidgetDragStartEvent = QEvent::registerEventType();
const int DockedWidgetDragStartEvent = QEvent::registerEventType();

/**
 * Creates a semi transparent pixmap from the given pixmap Source.
 * The Opacity parameter defines the opacity from completely transparent (0.0)
 * to completely opaque (1.0)
 */
QPixmap createTransparentPixmap(const QPixmap &source, qreal opacity)
{
    QPixmap transparentPixmap(source.size());
    transparentPixmap.fill(Qt::transparent);
    QPainter p(&transparentPixmap);
    p.setOpacity(opacity);
    p.drawPixmap(0, 0, source);
    return transparentPixmap;
}

/**
 * Helper function to set the icon of a certain button.
 * Use this function to set the icons for the dock area and dock widget buttons.
 * The function first uses the CustomIconId to get an icon from the
 * CIconProvider. You can register your custom icons with the icon provider, if
 * you do not want to use the default buttons and if you do not want to use
 * stylesheets.
 * If the IconProvider does not return a valid icon (icon is null), the function
 * fetches the given standard pixmap from the QStyle.
 * \param[in] b The button whose icons are to be set
 * \param[in] pixmap The standard pixmap to be used for the button
 * \param[in] iconId The identifier for the custom icon.
 */
void setButtonIcon(QAbstractButton *b, QStyle::StandardPixmap pixmap, Qx::DockIcon iconId)
{
    // First we try to use custom icons if available
    QIcon Icon = DockManager::iconProvider().customIcon(iconId);
    if (!Icon.isNull()) {
        b->setIcon(Icon);
        return;
    }

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    b->setIcon(b->style()->standardIcon(pixmap));
#else
    // The standard icons does not look good on high DPI screens so we create
    // our own "standard" icon here.
    QPixmap normalPixmap = b->style()->standardPixmap(pixmap, 0, b);
    Icon.addPixmap(createTransparentPixmap(normalPixmap, 0.25), QIcon::Disabled);
    Icon.addPixmap(normalPixmap, QIcon::Normal);
    b->setIcon(Icon);
#endif
}

DockInsertParam dockAreaInsertParameters(Qx::DockWidgetArea area)
{
    switch (area) {
    case Qx::TopDockWidgetArea:
        return DockInsertParam(Qt::Vertical, false);
    case Qx::RightDockWidgetArea:
        return DockInsertParam(Qt::Horizontal, true);
    case Qx::CenterDockWidgetArea:
    case Qx::BottomDockWidgetArea:
        return DockInsertParam(Qt::Vertical, true);
    case Qx::LeftDockWidgetArea:
        return DockInsertParam(Qt::Horizontal, false);
    default:
        DockInsertParam(Qt::Vertical, false);
    }   // switch (area)

    return DockInsertParam(Qt::Vertical, false);
}

/**
 * Calls unpolish() / polish for the style of the given widget to update
 * stylesheet if a property changes
 */
void repolishStyle(QWidget *w, RepolishChildOptions options)
{
    if (!w) {
        return;
    }
    w->style()->unpolish(w);
    w->style()->polish(w);

    if (RepolishIgnoreChildren == options) {
        return;
    }

    QList<QWidget *> childrens = w->findChildren<QWidget *>(
        QString(), (RepolishDirectChildren == options) ? Qt::FindDirectChildrenOnly : Qt::FindChildrenRecursively);

    for (auto c : childrens) {
        c->style()->unpolish(c);
        c->style()->polish(c);
    }
}

/**
 * This function walks the splitter tree upwards to hides all splitters
 * that do not have visible content
 */
void hideEmptyParentSplitters(DockSplitter *splitter)
{
    while (splitter && splitter->isVisible()) {
        if (!splitter->hasVisibleContent()) {
            splitter->hide();
        }
        splitter = internal::findParent<DockSplitter *>(splitter);
    }
}

/**
 * Replace the from widget in the given splitter with the To widget
 */
void replaceSplitterWidget(QSplitter *splitter, QWidget *from, QWidget *to)
{
    int index = splitter->indexOf(from);
    from->setParent(nullptr);
    splitter->insertWidget(index, to);
}

bool isHorizontalSideBarArea(Qx::DockSideBarArea area)
{
    switch (area) {
    case Qx::DockSideBarTop:
    case Qx::DockSideBarBottom:
        return true;
    case Qx::DockSideBarLeft:
    case Qx::DockSideBarRight:
        return false;
    default:
        return false;
    }

    return false;
}

bool isSideBarArea(Qx::DockWidgetArea area)
{
    return toSideBarArea(area) != Qx::DockSideBarNone;
}

Qx::DockSideBarArea toSideBarArea(Qx::DockWidgetArea area)
{
    switch (area) {
    case Qx::LeftAutoHideArea:
        return Qx::DockSideBarLeft;
    case Qx::RightAutoHideArea:
        return Qx::DockSideBarRight;
    case Qx::TopAutoHideArea:
        return Qx::DockSideBarTop;
    case Qx::BottomAutoHideArea:
        return Qx::DockSideBarBottom;
    default:
        return Qx::DockSideBarNone;
    }

    return Qx::DockSideBarNone;
}

#ifdef QX_DOCK_DEBUG
void setBackgroudColor(QWidget *w)
{
    QPalette pe = w->palette();
    if (qobject_cast<DockContainer *>(w)) {
        pe.setColor(QPalette::Window, Qt::red);
    } else if (qobject_cast<DockPanel *>(w)) {
        pe.setColor(QPalette::Window, Qt::green);
    } else if (qobject_cast<DockWidget *>(w)) {
        pe.setColor(QPalette::Window, Qt::blue);
    } else {
        pe.setColor(QPalette::Window, Qt::yellow);
    }
    w->setPalette(pe);
    w->setAutoFillBackground(true);
    w->layout()->setContentsMargins(2, 2, 2, 2);
}
#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
static QString _window_manager;
static QHash<QString, xcb_atom_t> _xcb_atom_cache;

bool is_platform_x11()
{
    return QGuiApplication::platformName() == QLatin1String("xcb");
}

xcb_connection_t *x11_connection()
{
    if (!qApp)
        return nullptr;
    QPlatformNativeInterface *native = qApp->platformNativeInterface();
    if (!native)
        return nullptr;

    void *connection = native->nativeResourceForIntegration(QByteArray("connection"));
    return reinterpret_cast<xcb_connection_t *>(connection);
}

xcb_atom_t xcb_get_atom(const char *name)
{
    if (!is_platform_x11()) {
        return XCB_ATOM_NONE;
    }
    auto key = QString(name);
    if (_xcb_atom_cache.contains(key)) {
        return _xcb_atom_cache[key];
    }
    xcb_connection_t *connection = x11_connection();
    xcb_intern_atom_cookie_t request = xcb_intern_atom(connection, 1, strlen(name), name);
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, request, nullptr);
    if (!reply) {
        return XCB_ATOM_NONE;
    }
    xcb_atom_t atom = reply->atom;
    if (atom == XCB_ATOM_NONE) {
        QX_DOCK_PRINT("Unknown Atom response from XServer: " << name);
    } else {
        _xcb_atom_cache.insert(key, atom);
    }
    free(reply);
    return atom;
}

void xcb_update_prop(bool set, WId window, const char *type, const char *prop, const char *prop2)
{
    auto connection = x11_connection();
    xcb_atom_t type_atom = xcb_get_atom(type);
    xcb_atom_t prop_atom = xcb_get_atom(prop);
    xcb_client_message_event_t event;
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.sequence = 0;
    event.window = window;
    event.type = type_atom;
    event.data.data32[0] = set ? 1 : 0;
    event.data.data32[1] = prop_atom;
    event.data.data32[2] = prop2 ? xcb_get_atom(prop2) : 0;
    event.data.data32[3] = 0;
    event.data.data32[4] = 0;

    xcb_send_event(connection, 0, window,
                   XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                       XCB_EVENT_MASK_PROPERTY_CHANGE,
                   (const char *)&event);
    xcb_flush(connection);
}

xcb_get_property_reply_t *_xcb_get_props(WId window, const char *type, unsigned int atom_type)
{
    if (!is_platform_x11()) {
        return nullptr;
    }
    xcb_connection_t *connection = x11_connection();
    xcb_atom_t type_atom = xcb_get_atom(type);
    if (type_atom == XCB_ATOM_NONE) {
        return nullptr;
    }
    xcb_get_property_cookie_t request =
        xcb_get_property_unchecked(connection, 0, window, type_atom, atom_type, 0, 1024);
    xcb_get_property_reply_t *reply = xcb_get_property_reply(connection, request, nullptr);
    if (reply && reply->type != atom_type) {
        QX_DOCK_PRINT("ATOM TYPE MISMATCH (" << type << "). Expected: " << atom_type << "  but got " << reply->type);
        free(reply);
        return nullptr;
    }
    return reply;
}

template <typename T> void xcb_get_prop_list(WId window, const char *type, QVector<T> &ret, unsigned int atom_type)
{
    xcb_get_property_reply_t *reply = _xcb_get_props(window, type, atom_type);
    if (reply && reply->format == 32 && reply->type == atom_type && reply->value_len > 0) {
        const xcb_atom_t *data = static_cast<const T *>(xcb_get_property_value(reply));
        ret.resize(reply->value_len);
        memcpy((void *)&ret.first(), (void *)data, reply->value_len * sizeof(T));
    }
    free(reply);
}

QString xcb_get_prop_string(WId window, const char *type)
{
    QString ret;
    // try utf8 first
    xcb_atom_t utf_atom = xcb_get_atom("UTF8_STRING");
    if (utf_atom != XCB_ATOM_NONE) {
        xcb_get_property_reply_t *reply = _xcb_get_props(window, type, utf_atom);
        if (reply && reply->format == 8 && reply->type == utf_atom) {
            const char *value = reinterpret_cast<const char *>(xcb_get_property_value(reply));
            ret = QString::fromUtf8(value, xcb_get_property_value_length(reply));
            free(reply);
            return ret;
        }
        free(reply);
    }
    // Fall back to XCB_ATOM_STRING
    xcb_get_property_reply_t *reply = _xcb_get_props(window, type, XCB_ATOM_STRING);
    if (reply && reply->format == 8 && reply->type == XCB_ATOM_STRING) {
        const char *value = reinterpret_cast<const char *>(xcb_get_property_value(reply));
        ret = QString::fromLatin1(value, xcb_get_property_value_length(reply));
    }
    free(reply);
    return ret;
}

bool xcb_dump_props(WId window, const char *type)
{
    QVector<xcb_atom_t> atoms;
    xcb_get_prop_list(window, type, atoms, XCB_ATOM_ATOM);
    qDebug() << "\n\n!!!" << type << "  -  " << atoms.length();
    xcb_connection_t *connection = x11_connection();
    for (auto atom : atoms) {
        auto foo = xcb_get_atom_name(connection, atom);
        auto bar = xcb_get_atom_name_reply(connection, foo, nullptr);
        qDebug() << "\t" << xcb_get_atom_name_name(bar);
        free(bar);
    }
    return true;
}

void xcb_add_prop(bool state, WId window, const char *type, const char *prop)
{
    if (!is_platform_x11()) {
        return;
    }
    xcb_atom_t prop_atom = xcb_get_atom(prop);
    xcb_atom_t type_atom = xcb_get_atom(type);
    if (prop_atom == XCB_ATOM_NONE || type_atom == XCB_ATOM_NONE) {
        return;
    }
    QVector<xcb_atom_t> atoms;
    xcb_get_prop_list(window, type, atoms, XCB_ATOM_ATOM);
    int index = atoms.indexOf(prop_atom);
    if (state && index == -1) {
        atoms.push_back(prop_atom);
    } else if (!state && index >= 0) {
        atoms.remove(index);
    }
    xcb_connection_t *connection = x11_connection();
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, type_atom, XCB_ATOM_ATOM, 32, atoms.count(),
                        atoms.constData());
    xcb_flush(connection);
}

QString detectWindowManagerX11()
{
    // Tries to detect the windowmanager via X11.
    // See: https://specifications.freedesktop.org/wm-spec/1.3/ar01s03.html#idm46018259946000
    if (!is_platform_x11()) {
        return "UNKNOWN";
    }
    xcb_connection_t *connection = x11_connection();
    xcb_screen_t *first_screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    if (!first_screen) {
        QX_DOCK_PRINT("No screen found via XCB.");
        return "UNKNOWN";
    }
    // Get supporting window ()
    xcb_window_t root = first_screen->root;
    xcb_window_t support_win = 0;
    QVector<xcb_window_t> sup_windows;
    xcb_get_prop_list(root, "_NET_SUPPORTING_WM_CHECK", sup_windows, XCB_ATOM_WINDOW);
    if (sup_windows.length() == 0) {
        // This doesn't seem to be in use anymore, but wmctrl does the same so lets play safe.
        // Both XCB_ATOM_CARDINAL and XCB_ATOM_WINDOW break down to a uint32_t, so reusing sup_windows should be fine.
        xcb_get_prop_list(root, "_WIN_SUPPORTING_WM_CHECK", sup_windows, XCB_ATOM_CARDINAL);
    }
    if (sup_windows.length() == 0) {
        QX_DOCK_PRINT("Failed to get the supporting window on non EWMH comform WM.");
        return "UNKNOWN";
    }
    support_win = sup_windows[0];
    QString ret = xcb_get_prop_string(support_win, "_NET_WM_NAME");
    if (ret.length() == 0) {
        QX_DOCK_PRINT("Empty WM name occurred.");
        return "UNKNOWN";
    }
    return ret;
}

QString windowManager()
{
    if (_window_manager.length() == 0) {
        _window_manager = detectWindowManagerX11();
    }
    return _window_manager;
}
#endif

}   // internal

QX_DOCK_END_NAMESPACE
