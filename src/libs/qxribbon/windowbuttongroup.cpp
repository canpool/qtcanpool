/**
 * Copyleft (C) 2023-2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "windowbuttongroup.h"
#include "windowbuttongroup_p.h"
#include "ribbonmanager.h"
#include "ribbonutils.h"

#include <QResizeEvent>
#include <QEvent>
#include <QScopedPointer>
#include <QStyle>
#include <QToolButton>
#include <QHBoxLayout>
#include <QTimer>
#include <QCoreApplication>

QX_RIBBON_BEGIN_NAMESPACE

#define MIN_BUTTON_WIDTH 30
#define MAX_BUTTON_WIDTH 30
#define CLS_BUTTON_WIDTH 40

//! Used before the ribbon has published its metrics, so a group built early still
//! gets a glyph. Once the bar is up, glyphSize() follows the caption height.
static const int s_defaultGlyphSide = 16;

WindowButton::WindowButton(QWidget *p)
    : QToolButton(p)
{
    setAutoRaise(true);
}

WindowButtonGroupPrivate::WindowButtonGroupPrivate()
    : m_closeButton(Q_NULLPTR)
    , m_minimizeButton(Q_NULLPTR)
    , m_maximizeButton(Q_NULLPTR)
    , m_iconScale(1.0)
    , m_signalEnabled(false)
    , m_glyphSize()
{
}

void WindowButtonGroupPrivate::init()
{
    Q_Q(WindowButtonGroup);

    m_minimizeButton = new WindowButton(q);
    m_minimizeButton->setObjectName(QStringLiteral("qx_MinimizeWindowButton"));
    m_minimizeButton->setFixedWidth(MIN_BUTTON_WIDTH);
    m_minimizeButton->setFocusPolicy(Qt::NoFocus);
    m_minimizeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    connect(m_minimizeButton, &QAbstractButton::clicked, this, &WindowButtonGroupPrivate::buttonClicked);

    m_maximizeButton = new WindowButton(q);
    m_maximizeButton->setObjectName(QStringLiteral("qx_MaximizeWindowButton"));
    m_maximizeButton->setFixedWidth(MAX_BUTTON_WIDTH);
    m_maximizeButton->setCheckable(true);
    m_maximizeButton->setFocusPolicy(Qt::NoFocus);
    m_maximizeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    connect(m_maximizeButton, &QAbstractButton::clicked, this, &WindowButtonGroupPrivate::buttonClicked);

    m_closeButton = new WindowButton(q);
    m_closeButton->setObjectName(QStringLiteral("qx_CloseWindowButton"));
    m_closeButton->setFixedWidth(CLS_BUTTON_WIDTH);
    m_closeButton->setFocusPolicy(Qt::NoFocus);
    m_closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    connect(m_closeButton, &QAbstractButton::clicked, this, &WindowButtonGroupPrivate::buttonClicked);

    QHBoxLayout *lay = new QHBoxLayout(q);
    lay->setSpacing(0);
    lay->setContentsMargins(1, 0, 0, 1);
    lay->addWidget(m_minimizeButton);
    lay->addWidget(m_maximizeButton);
    lay->addWidget(m_closeButton);

    updateButtonIconSize();
}

void WindowButtonGroupPrivate::updateButton(Qt::WindowFlags flags)
{
    m_minimizeButton->setVisible(flags & Qt::WindowMinimizeButtonHint);
    m_maximizeButton->setVisible(flags & Qt::WindowMaximizeButtonHint);
    m_closeButton->setVisible(flags & Qt::WindowCloseButtonHint);
}

/*!
 * The box a caption glyph is drawn in.
 *
 * The style sheet used to name the glyph files on the buttons themselves, and an
 * image named that way is drawn in the button's contents rectangle - so the glyph
 * measured the caption height, not a size of its own. Keeping the box at the
 * caption height preserves that: the glyph keeps its share of the bar as the bar
 * grows with the font, and stays sharp because the vector asset is rendered at the
 * size it is drawn at.
 */
QSize WindowButtonGroupPrivate::glyphSize() const
{
    int side = groupHeight();
    if (side <= 0) {
        side = s_defaultGlyphSide;
    }
    return QSize(side, side) * m_iconScale;
}

void WindowButtonGroupPrivate::updateButtonIconSize()
{
    const QSize size = glyphSize();
    if (size == m_glyphSize) {
        return;
    }
    m_glyphSize = size;
    m_minimizeButton->setIconSize(size);
    m_maximizeButton->setIconSize(size);
    m_closeButton->setIconSize(size);
    updateButtonIcons();
}

/*!
 * The caption glyphs are masks, painted in the palette's window-text colour so
 * they invert with the theme instead of a style sheet naming a frozen file. The
 * close button's hover glyph uses highlighted text, the role for a glyph that
 * sits on a filled background.
 */
void WindowButtonGroupPrivate::updateButtonIcons()
{
    Q_Q(WindowButtonGroup);
    const QSize size = glyphSize();
    const QColor glyph = q->palette().color(QPalette::WindowText);
    const QColor onFilled = q->palette().color(QPalette::HighlightedText);

    QIcon minimize;
    QIcon maximize;
    QIcon close;
    // One pixmap per device pixel ratio, so the vector glyph is rendered at the
    // size it is drawn at rather than upscaled from a single small bitmap.
    for (int factor : {1, 2, 3}) {
        const QSize s = size * factor;
        minimize.addPixmap(tintedPixmap(QStringLiteral(":/qxribbon/res/titlebar_min.svg"), glyph, s));
        maximize.addPixmap(tintedPixmap(QStringLiteral(":/qxribbon/res/titlebar_max.svg"), glyph, s), QIcon::Normal,
                           QIcon::Off);
        // A maximized window shows the restore glyph on the same button.
        maximize.addPixmap(tintedPixmap(QStringLiteral(":/qxribbon/res/titlebar_normal.svg"), glyph, s), QIcon::Normal,
                           QIcon::On);
        close.addPixmap(tintedPixmap(QStringLiteral(":/qxribbon/res/titlebar_close.svg"), glyph, s), QIcon::Normal,
                        QIcon::Off);
        const QPixmap hover = tintedPixmap(QStringLiteral(":/qxribbon/res/titlebar_close_hover.svg"), onFilled, s);
        close.addPixmap(hover, QIcon::Active, QIcon::Off);
        close.addPixmap(hover, QIcon::Selected, QIcon::Off);
    }

    m_minimizeButton->setIcon(minimize);
    m_maximizeButton->setIcon(maximize);
    m_closeButton->setIcon(close);
}

QSize WindowButtonGroupPrivate::sizeHint() const
{
    int width = 0;
    int height = groupHeight();

    // Which buttons count has to be read from the hidden flag, not from
    // isVisible(): the group is sized while its window is still hidden (the
    // ribbon lays itself out before the first show), and isVisible() is false
    // for every button then, which would size the whole group to zero.
    if (!m_closeButton->isHidden()) {
        width += CLS_BUTTON_WIDTH;
    }
    if (!m_maximizeButton->isHidden()) {
        width += MAX_BUTTON_WIDTH;
    }
    if (!m_minimizeButton->isHidden()) {
        width += MIN_BUTTON_WIDTH;
    }
    return QSize(width, height);
}

int WindowButtonGroupPrivate::groupHeight() const
{
    // In TabBarInlineStyle, tabBarHeight may be smaller than titleBarHeight,
    // which causes the window buttons to cover the ribbon area, so use the smallest one
    return qMin(RibbonElementStyleOpt.titleBarHeight(), RibbonElementStyleOpt.tabBarHeight());
}

void WindowButtonGroupPrivate::buttonClicked()
{
    Q_Q(WindowButtonGroup);
    WindowButton *button = qobject_cast<WindowButton *>(sender());

    if (m_signalEnabled) {
        if (button == m_minimizeButton) {
            Q_EMIT q->buttonMinimizeClicked();
        } else if (button == m_maximizeButton) {
            Q_EMIT q->buttonMaximzieClicked();
        } else if (button == m_closeButton) {
            Q_EMIT q->buttonCloseClicked();
        }
    } else {
        QWidget *pw = q->parentWidget();
        if (pw) {
            if (button == m_minimizeButton) {
                pw->showMinimized();
            } else if (button == m_maximizeButton) {
                if (pw->isMaximized()) {
                    pw->showNormal();
                } else {
                    pw->showMaximized();
                }
            } else if (button == m_closeButton) {
                pw->close();
            }
        }
    }
    // It's a Qt issue that if a QAbstractButton::clicked triggers a window's maximization,
    // the button remains to be hovered until the mouse move. As a result, we need to
    // manually send leave events to the button.
    if (button == m_maximizeButton) {
        QTimer::singleShot(50, this, [this]() {
            QCoreApplication::postEvent(m_maximizeButton, new QEvent(QEvent::Leave));
        });
    }
}

WindowButtonGroup::WindowButtonGroup(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(WindowButtonGroup)
    Q_D(WindowButtonGroup);
    d->init();
    updateWindowFlags(flags);
}

WindowButtonGroup::~WindowButtonGroup()
{
    if (this->parent()) {
        this->parent()->removeEventFilter(this);
    }
    QX_FINI_PRIVATE()
}

void WindowButtonGroup::updateWindowFlags(Qt::WindowFlags flags)
{
    Q_D(WindowButtonGroup);
    if (flags == Qt::WindowFlags()) {
        if (QWidget *pw = parentWidget()) {
            flags = pw->windowFlags();
        }
    }
    d->updateButton(flags);
}

void WindowButtonGroup::setIconScale(qreal scale)
{
    Q_D(WindowButtonGroup);
    if (d->m_iconScale == scale) {
        return;
    }
    d->m_iconScale = scale;
    d->updateButtonIconSize();
}

void WindowButtonGroup::resizeEvent(QResizeEvent *e)
{
    // The caption height decides the glyph size, so a ribbon that switched style
    // or font resizes this group and the glyphs follow it.
    Q_D(WindowButtonGroup);
    d->updateButtonIconSize();
    QWidget::resizeEvent(e);
}

void WindowButtonGroup::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::PaletteChange) {
        Q_D(WindowButtonGroup);
        d->updateButtonIcons();
    }
    QWidget::changeEvent(e);
}

void WindowButtonGroup::setWindowStates(Qt::WindowStates s)
{
    Q_D(WindowButtonGroup);
    bool on = s.testFlag(Qt::WindowMaximized);
    d->m_maximizeButton->setChecked(on);
    d->m_maximizeButton->setToolTip(on ? tr("Restore") : tr("Maximize"));
}

QSize WindowButtonGroup::sizeHint() const
{
    Q_D(const WindowButtonGroup);
    return d->sizeHint();
}

QWidget *WindowButtonGroup::widgetForMaximizeButton() const
{
    Q_D(const WindowButtonGroup);
    return d->m_maximizeButton;
}

/**
 * If the signal is disabled, the window button's signal
 * will be directly applied to the parent window; otherwise,
 * only the button signal will be sent
 */
bool WindowButtonGroup::signalIsEnabled() const
{
    Q_D(const WindowButtonGroup);
    return d->m_signalEnabled;
}

void WindowButtonGroup::setSignalEnabled(bool enable)
{
    Q_D(WindowButtonGroup);
    d->m_signalEnabled = enable;
}

QX_RIBBON_END_NAMESPACE
