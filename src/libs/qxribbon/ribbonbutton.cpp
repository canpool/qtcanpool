/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonbutton.h"
#include "ribbonbutton_p.h"
#include "ribbongroup.h"
#include "ribbonscreentip.h"

#include <QCursor>
#include <QAction>
#include <QDebug>
#include <QHelpEvent>
#include <QKeySequence>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStyleOption>
#include <QStyleOptionFocusRect>
#include <QStyleOptionToolButton>
#include <QStylePainter>

QX_RIBBON_BEGIN_NAMESPACE

#define QX_INDICATOR_ARROW_WIDTH 8
#define QX_WIDTH_HEIGHT_RATIO    1.2

/**
 * @def Enabling this macro prints some common information
 */
#define DebugRibbonButton_TextDrawPrint 0

// #define QX_RIBBON_DEBUG_HELP_DRAW

#ifdef QX_RIBBON_DEBUG_HELP_DRAW
#define HELP_DRAW_RECT(p, rect)                                                                                        \
    do {                                                                                                               \
        p.save();                                                                                                      \
        p.setPen(Qt::red);                                                                                             \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)

QDebug operator<<(QDebug debug, const QStyleOptionToolButton &opt)
{
    debug << "=============="
          << "\nQStyleOption(" << (QStyleOption)opt << ")"
          << "\n  QStyleOptionComplex:"
             "\n     subControls("
          << opt.subControls
          << " ) "
             "\n     activeSubControls("
          << opt.activeSubControls
          << "\n  QStyleOptionToolButton"
             "\n     features("
          << opt.features
          << ")"
             "\n     toolButtonStyle("
          << opt.toolButtonStyle << ")";

    return (debug);
}

#endif

bool RibbonButtonPrivate::s_isToolButtonTextShift = false;

RibbonButtonPrivate::RibbonButtonPrivate()
    : m_buttonType(RibbonButton::LargeButton)
    , m_largeButtonType(RibbonButton::Normal)
    , m_mouseOnSubControl(false)
    , m_menuButtonPressed(false)
    , m_isWordWrap(false)
    , m_spacing(2)
{
}

RibbonButtonPrivate::~RibbonButtonPrivate()
{
}

QStyle *RibbonButtonPrivate::style() const
{
    Q_Q(const RibbonButton);
    return q->style();
}

/**
 * @brief Recomputes the sizehint; sizeHint caches its result after the first computation and need not recompute when
 * the font or text is unchanged
 *
 * Normally the height need not be handled because RibbonGroupLayout controls it; this function mainly provides width
 * information to RibbonGroupLayout, which lays out the button and eventually triggers resize, where the concrete size
 * is computed again
 *
 * Therefore resize must compute the size the same way recalcSizeHint does, especially regarding font settings
 *
 * sizeHint is usually called before layout and resizeEvent after it, so font metrics are computed twice; this could be
 * optimized later
 */
void RibbonButtonPrivate::recalcSizeHint(QStyleOptionToolButton &opt, QSize s)
{
    Q_Q(RibbonButton);
    // QToolButton's sizeHint already accounts for the menu arrow position
    // Looking at the source, QToolButton's sizeHint does not account for word wrap
    if (RibbonButton::LargeButton == m_buttonType) {
        // Compute the optimal size
        if (RibbonGroup *group = qobject_cast<RibbonGroup *>(q->parent())) {
            // For a large button based on a RibbonGroup, use the large button height computed by RibbonGroup
            s.setHeight(group->largeHeight());
        }
        if (RibbonButton::Normal == m_largeButtonType) {
            // Word wrap only applies in Normal mode
            if (s.width() > s.height() * QX_WIDTH_HEIGHT_RATIO) {
                // This is a rectangular button; if the width exceeds the height, check whether word wrap fits
                QFontMetrics fm = q->fontMetrics();
                QRect textRange(0, 0, s.width(), s.height());

                //! 1. First compute the optimal text textRange
                {
                    const int maxTryCount = 3;
                    int tryCount = 0;
                    int alignment = Qt::TextShowMnemonic | Qt::TextWordWrap;   // text alignment
                    do {
                        // Compute the tight rect for two lines of text, starting from half and increasing
                        textRange.setWidth(s.width() / 2 + (s.width() / 2) * (float(tryCount) / maxTryCount));
                        textRange = fm.boundingRect(textRange, alignment, opt.text);
                        // On Linux GNOME, fm.lineSpacing() is 15 and textRange.height() is 35
                        // On Windows 10, fm.lineSpacing() is 16 and textRange.height() is 32
#ifdef Q_OS_WINDOWS
                        if (textRange.height() <= (fm.lineSpacing() * 2)) {
#else
                        if (textRange.height() <= (fm.lineSpacing() * 2.5)) {
#endif
                            // Keep it within two lines
                            m_isWordWrap = (textRange.height() > fm.lineSpacing());
                            break;
                        }
                        ++tryCount;
                    } while (tryCount < maxTryCount);
                }
                // Leave 2 pixels on each side
                s.setWidth(textRange.width() + m_spacing * 2);
                //! 2. Then consider the arrow to adjust textRange
                if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) ||
                    (opt.features & QStyleOptionToolButton::HasMenu)) {
                    if (m_isWordWrap) {
                        s.rwidth() += QX_INDICATOR_ARROW_WIDTH + m_spacing;
                    }
                }
            } else {
                m_isWordWrap = opt.text.contains('\n');
                if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) ||
                    (opt.features & QStyleOptionToolButton::HasMenu)) {
                    if (m_isWordWrap) {
                        s.rwidth() += QX_INDICATOR_ARROW_WIDTH + m_spacing;
                    }
                }
            }
        } else {
            // Otherwise it is lite mode: only one line is allowed and a menu causes an offset
            if (s.width() > s.height() * 2) {
                // For an overly wide button, elide the text with "..."
                s.rwidth() = s.height() * 2;
            }
            if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) ||
                (opt.features & QStyleOptionToolButton::HasMenu)) {
                s.rwidth() += QX_INDICATOR_ARROW_WIDTH + m_spacing;
            }
        }
        if (RibbonButton::Normal == m_largeButtonType) {
            if (s.width() < s.height()) {
                // A short text's icon may be smaller than a long text's, so widen the short-text button based on the
                // height
                int textHeight = q->fontMetrics().lineSpacing();
                if (m_isWordWrap && !opt.text.contains('\n')) {
                    textHeight <<= 1;
                }
                int iconHeight = s.height() - textHeight - m_spacing;
                if (iconHeight > s.width()) {
                    s.rwidth() = iconHeight;
                }
            }
        }
    } else {
        // InstantPopup does not add a widget for the arrow in qtoolbutton, so handle it like MenuButtonPopup
        // In icon-only small mode, reserve space for the drop-down arrow
#ifdef Q_OS_WINDOWS
        if (Qt::ToolButtonIconOnly == opt.toolButtonStyle) {
#else
        if (1) {   // Tested on ubuntu 22.04
#endif
            if (opt.features & QStyleOptionToolButton::MenuButtonPopup ||
                opt.features & QStyleOptionToolButton::HasMenu) {
                // If there is a menu
                // s.rwidth() += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);
                s.rwidth() += QX_INDICATOR_ARROW_WIDTH + m_spacing;
            }
        }
    }
    m_sizeHint = s;
}

/**
 * @brief RibbonButton::drawIconAndLabel
 *
 * With LargeButtonType == Normal the icon takes half of the large button; in wps mode the icon takes 60% and the text
 * 40%, with no word wrap
 * @param opt
 * @param p
 * @param w
 */
void RibbonButtonPrivate::drawIconAndLabel(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w)
{
    if (RibbonButton::LargeButton == m_buttonType) {
        // Draw the icon and text
        QRect textRect = adjustedTextRect(opt, w);
        bool isArrow = opt.features & QStyleOptionToolButton::Arrow;
        if (((!isArrow && opt.icon.isNull()) && !opt.text.isEmpty()) ||
            (opt.toolButtonStyle == Qt::ToolButtonTextOnly)) {
            // no arrow, no icon, has text || text-only mode
            int alignment =
                Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextWordWrap;   // for plain text, center and wrap
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, w)) {
                alignment |= Qt::TextHideMnemonic;
            }
            p.setFont(opt.font);
            style()->drawItemText(&p, textRect, alignment, opt.palette, opt.state & QStyle::State_Enabled, opt.text,
                                  QPalette::ButtonText);
        } else {
            // not plain text
            if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
                // text plus icon
                p.setFont(opt.font);
                int alignment = Qt::TextShowMnemonic | Qt::TextWordWrap;   // word wrap
                if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, w)) {
                    alignment |= Qt::TextHideMnemonic;
                }
                // text below the icon
                // draw the icon first
                if (!isArrow) {
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
                    HELP_DRAW_RECT(p, m_iconRect);
#endif
                    QPixmap pm = createIconPixmap(opt, m_iconRect.size());
                    style()->drawItemPixmap(&p, m_iconRect, Qt::AlignCenter, pm);
                } else {
                    drawArrow(style(), &opt, m_iconRect, &p, w);
                }
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
                HELP_DRAW_RECT(p, textRect);
#endif
                if (RibbonButton::Lite == m_largeButtonType) {
                    alignment |= Qt::AlignCenter;
                } else {
                    alignment |= Qt::AlignHCenter | Qt::AlignTop;   // text is top-aligned
                }

                QString text = opt.text;
                // Then draw the text; for a LargeButton in Normal mode with a menu and m_isWordWrap true, the arrow
                // goes beside the text
                if (RibbonButton::Lite == m_largeButtonType) {
                    // lite mode: no word wrap, so the displayed content must be elided
                    text.remove('\n');
                    text = w->fontMetrics().elidedText(text, Qt::ElideRight, textRect.width(), Qt::TextShowMnemonic);
                }
                style()->drawItemText(&p, QStyle::visualRect(opt.direction, opt.rect, textRect), alignment, opt.palette,
                                      opt.state & QStyle::State_Enabled, text, QPalette::ButtonText);
            } else {
                // icon only
                if (isArrow) {
                    drawArrow(style(), &opt, opt.rect, &p, w);
                } else {
                    QPixmap pm = createIconPixmap(opt, m_iconRect.size());
                    style()->drawItemPixmap(&p, m_iconRect, Qt::AlignCenter, pm);
                }
            }
            // Draw the sub control drop-down arrow
            if (opt.features & QStyleOptionToolButton::MenuButtonPopup ||
                opt.features & QStyleOptionToolButton::HasMenu) {
                opt.rect = calcIndicatorArrowDownRect(opt);
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
                HELP_DRAW_RECT(p, opt.rect);
#endif
                style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, &p, w);
            }
        }
    } else {
        // small icon
        if (!opt.icon.isNull() && opt.toolButtonStyle != Qt::ToolButtonTextOnly) {
            QPixmap pm = createIconPixmap(opt, m_iconRect.size());

            if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
                p.save();
                p.setFont(opt.font);

                QRect pr = m_iconRect;                                                // icon area
                QRect tr = opt.rect.adjusted(pr.width() + 2 * m_spacing, 0, -1, 0);   // text area

#ifdef QX_RIBBON_DEBUG_HELP_DRAW
                HELP_DRAW_RECT(p, pr);
                HELP_DRAW_RECT(p, tr);
#endif

                int alignment = Qt::TextShowMnemonic;
                // shortcut underline
                if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, w)) {
                    alignment |= Qt::TextHideMnemonic;
                }
                if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                    // ribbonbutton does not support ToolButtonTextUnderIcon in small-icon mode
                } else {
                    style()->drawItemPixmap(&p, QStyle::visualRect(opt.direction, opt.rect, pr), Qt::AlignCenter, pm);
                    alignment |= Qt::AlignLeft | Qt::AlignVCenter;
                }
                style()->drawItemText(&p, QStyle::visualRect(opt.direction, opt.rect, tr), alignment, opt.palette,
                                      opt.state & QStyle::State_Enabled, opt.text, QPalette::ButtonText);
                p.restore();
            } else {
                style()->drawItemPixmap(&p, m_iconRect, Qt::AlignCenter, pm);
            }
        } else {   // text only
            int alignment = Qt::TextShowMnemonic;
            // shortcut underline
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, w)) {
                alignment |= Qt::TextHideMnemonic;
            }
            style()->drawItemText(&p, QStyle::visualRect(opt.direction, opt.rect, opt.rect.adjusted(2, 1, -2, -1)),
                                  alignment, opt.palette, opt.state & QStyle::State_Enabled, opt.text,
                                  QPalette::ButtonText);
        }

        // Draw the sub control drop-down arrow
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
            QStyleOptionToolButton tool = opt;
            tool.rect = calcIndicatorArrowDownRect(tool);
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
            HELP_DRAW_RECT(p, tool.rect);
#endif
            style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, &p, w);
        }
    }
}

void RibbonButtonPrivate::drawArrow(const QStyle *style, const QStyleOptionToolButton *toolbutton, const QRect &rect,
                                    QPainter *painter, const QWidget *widget)
{
    QStyle::PrimitiveElement pe;

    switch (toolbutton->arrowType) {
    case Qt::LeftArrow:
        pe = QStyle::PE_IndicatorArrowLeft;
        break;
    case Qt::RightArrow:
        pe = QStyle::PE_IndicatorArrowRight;
        break;
    case Qt::UpArrow:
        pe = QStyle::PE_IndicatorArrowUp;
        break;
    case Qt::DownArrow:
        pe = QStyle::PE_IndicatorArrowDown;
        break;
    default:
        return;
    }

    QStyleOption arrowOpt = *toolbutton;
    arrowOpt.rect = rect;
    style->drawPrimitive(pe, &arrowOpt, painter, widget);
}

/**
 * @brief Computes the drawing areas for the icon and the text according to the settings
 * @param opt
 */
void RibbonButtonPrivate::calcIconAndTextRect(const QStyleOptionToolButton &opt)
{
    Q_Q(RibbonButton);
    if (RibbonButton::LargeButton == m_buttonType) {
        m_iconRect = opt.rect.adjusted(m_spacing, m_spacing, -m_spacing, -m_spacing);
        if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
            // sizehint already guarantees the width can show two lines
            // The icon area is the area outside the text; if m_isEnableWordWrap is true, leave room for two lines of
            // text
            QFontMetrics fm = q->fontMetrics();
            int maxheight = 30;
            if (RibbonButton::Normal == m_largeButtonType) {
                // Subtract two lines of text, the vertical margins, and the gap between icon and text
                maxheight = opt.rect.height() - (fm.lineSpacing() * 2) - 3 * m_spacing;
            } else {
                maxheight = opt.rect.height() - (fm.lineSpacing() * 1.2) - 3 * m_spacing;
            }
            m_iconRect.setHeight(maxheight);

            // Without a menu, the height is the rect bottom minus the icon bottom minus 2*spacing
            m_textRect.setRect(m_spacing, m_iconRect.bottom() + m_spacing, m_iconRect.width(),
                               opt.rect.bottom() - m_iconRect.bottom() - 2 * m_spacing);
            // Ensure two lines of text fit
            // Determine the text area below
            if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) ||
                (opt.features & QStyleOptionToolButton::HasMenu)) {
                // With a menu and word wrap, offset the width by ARROW_WIDTH
                if (m_isWordWrap) {
                    m_textRect.adjust(0, 0, -(QX_INDICATOR_ARROW_WIDTH + m_spacing), 0);
                } else if (RibbonButton::Lite == m_largeButtonType) {
                    // In lite mode, also offset for the down arrow when word wrap is disabled
                    m_textRect.adjust(0, 0, -(QX_INDICATOR_ARROW_WIDTH + m_spacing), 0);
                }
            }
        } else {
            m_textRect = QRect();
        }
    } else {
        // small button
        if (opt.toolButtonStyle == Qt::ToolButtonIconOnly) {
            // InstantPopup does not add a widget for the arrow in qtoolbutton, so handle it like MenuButtonPopup
            // In icon-only small mode, reserve space for the drop-down arrow
            m_iconRect = opt.rect.adjusted(m_spacing, m_spacing, -m_spacing, -m_spacing);
            if (opt.features & QStyleOptionToolButton::MenuButtonPopup ||
                opt.features & QStyleOptionToolButton::HasMenu) {
                m_iconRect.adjust(0, 0, -(QX_INDICATOR_ARROW_WIDTH + m_spacing), 0);
            }
            // Determine the text area
            m_textRect = QRect();
        } else {
            m_iconRect = QRect(m_spacing, m_spacing, qMax(opt.rect.height(), opt.iconSize.width()) - 2 * m_spacing,
                               opt.rect.height() - 2 * m_spacing);
            m_textRect.setRect(m_iconRect.right() + m_spacing, m_spacing,
                               opt.rect.right() - m_iconRect.right() - 2 * m_spacing,
                               opt.rect.height() - 2 * m_spacing);
            // Determine the text area
            if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) ||
                (opt.features & QStyleOptionToolButton::HasMenu)) {
                // With a menu, offset the width by ARROW_WIDTH
                m_textRect.adjust(0, 0, -(QX_INDICATOR_ARROW_WIDTH + m_spacing), 0);
            }
        }
    }

    // Text position for plain text
    bool isArrow = opt.features & QStyleOptionToolButton::Arrow;
    if ((Qt::ToolButtonTextOnly == opt.toolButtonStyle) || (opt.icon.isNull() && !isArrow)) {
        m_textRect = opt.rect.adjusted(m_spacing, m_spacing, -m_spacing, -m_spacing);
    }
}

/**
 * @brief Computes the text display area according to the settings
 * @param opt
 * @param w
 * @return the text display area
 */
QRect RibbonButtonPrivate::adjustedTextRect(const QStyleOptionToolButton &opt, const QWidget *w)
{
    QRect rect = m_textRect;

    // These two offsets give some buttons a jitter effect on the text when clicked; office2013's ribbon does not move,
    // so the effect is disabled
    if (s_isToolButtonTextShift) {
        int shiftX = 0;
        int shiftY = 0;

        if (opt.state & (QStyle::State_Sunken | QStyle::State_On)) {
            shiftX = style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal, &opt, w);
            shiftY = style()->pixelMetric(QStyle::PM_ButtonShiftVertical, &opt, w);
        }

        rect.translate(shiftX, shiftY);
    }
    return rect;
}

/**
 * @brief Position of the sub control drop-down arrow
 *
 * For large mode, if the text wraps the arrow is at the far right; otherwise it sits where the second text line would
 * be
 * @param opt
 * @return
 * @note The IndicatorArrow size calculation depends on the text, so calcIconAndTextRect must be called first
 */
QRect RibbonButtonPrivate::calcIndicatorArrowDownRect(const QStyleOptionToolButton &opt)
{
    // Reserve QX_INDICATOR_ARROW_WIDTH px for drawing the arrow
    QRect rect = opt.rect;
    if (RibbonButton::LargeButton == m_buttonType) {
        // In Lite the arrow size is controlled by the rect width (see m_textRect in calcIconAndTextRect),
        // otherwise it is controlled by the rect height (see QX_INDICATOR_ARROW_WIDTH below)
        if ((RibbonButton::Lite == m_largeButtonType)) {
            // First check whether it is lite with word wrap disabled
            // yh + QX_INDICATOR_ARROW_WIDTH + yh = m_textRect.height()
            // yh = (m_textRect.height() - QX_INDICATOR_ARROW_WIDTH) / 2
            // y = m_textRect.top() + yh
            int y = m_textRect.top() + (m_textRect.height() - QX_INDICATOR_ARROW_WIDTH) / 2;
            rect.setRect(m_textRect.right() + m_spacing, y, QX_INDICATOR_ARROW_WIDTH, QX_INDICATOR_ARROW_WIDTH);
        } else if (m_isWordWrap) {
            // If word wrap is allowed, the menu drop-down arrow sits at the end of the second text line's rect
            rect.setRect(m_textRect.right() + m_spacing, m_textRect.top() + m_textRect.height() / 2,
                         QX_INDICATOR_ARROW_WIDTH, m_textRect.height() / 2);
        } else {
            // Otherwise it uses the second text line position       | y coordinate
            // m_textRect.height() / 2 + yh + QX_INDICATOR_ARROW_WIDTH + yh = m_textRect.height()
            // yh = m_textRect.height() / 4 - QX_INDICATOR_ARROW_WIDTH / 2
            // y = m_textRect.bottom() - yh - QX_INDICATOR_ARROW_WIDTH
            int y = m_textRect.bottom() - m_textRect.height() / 4 - QX_INDICATOR_ARROW_WIDTH / 2;
            rect.setRect(m_textRect.left(), y, m_textRect.width(), QX_INDICATOR_ARROW_WIDTH);
        }
    } else {
        rect.setRect(rect.width() - QX_INDICATOR_ARROW_WIDTH - m_spacing, m_spacing, QX_INDICATOR_ARROW_WIDTH,
                     rect.height() - 2 * m_spacing);
    }
    m_indicatorArrowRect = rect;   // record
    return rect;
}

QPixmap RibbonButtonPrivate::createIconPixmap(const QStyleOptionToolButton &opt, const QSize &iconsize)
{
    Q_Q(RibbonButton);
    if (!opt.icon.isNull()) {   // has an icon
        QIcon::State state = opt.state & QStyle::State_On ? QIcon::On : QIcon::Off;
        QIcon::Mode mode;
        if (!(opt.state & QStyle::State_Enabled)) {
            mode = QIcon::Disabled;
        } else if ((opt.state & QStyle::State_MouseOver) && (opt.state & QStyle::State_AutoRaise)) {
            mode = QIcon::Active;
        } else {
            mode = QIcon::Normal;
        }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        // return (opt.icon.pixmap(q->window()->windowHandle(), opt.rect.size().boundedTo(iconsize), mode, state));
        return opt.icon.pixmap(q->window()->windowHandle(), iconsize, mode, state);
#else
        return opt.icon.pixmap(iconsize, q->window()->devicePixelRatio(), mode, state);
#endif
    }
    return QPixmap();
}

void RibbonButtonPrivate::updateButtonState(const QPoint &pos)
{
    Q_Q(RibbonButton);
    bool isMouseOnSubControl(false);
    if (RibbonButton::LargeButton == m_buttonType) {
        isMouseOnSubControl = m_textRect.united(m_indicatorArrowRect).contains(pos);
    } else {
        // Small button mode behaves like a normal toolbutton
        isMouseOnSubControl = m_indicatorArrowRect.contains(pos);
    }
    if (m_mouseOnSubControl != isMouseOnSubControl) {
        m_mouseOnSubControl = isMouseOnSubControl;
        // Refresh once when switching from icon to text
        q->update();
    }
}

void RibbonButtonPrivate::drawButton(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w)
{
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
        if (!w->rect().contains(w->mapFromGlobal(QCursor::pos()))) {
            opt.state &= ~QStyle::State_MouseOver;
        }
    }
    bool autoRaise = opt.state & QStyle::State_AutoRaise;
    QStyle::State bflags = opt.state & ~QStyle::State_Sunken;

    if (autoRaise) {
        if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled)) {
            bflags &= ~QStyle::State_Raised;
        }
    }
    if (opt.state & QStyle::State_Sunken) {
        if (opt.activeSubControls & QStyle::SC_ToolButton) {
            bflags |= QStyle::State_Sunken;
        } else if (opt.activeSubControls & QStyle::SC_ToolButtonMenu) {
            bflags |= QStyle::State_MouseOver;
        }
    }
    // Draw the background
    QStyleOption tool(0);
    tool.palette = opt.palette;
    QStyle::PrimitiveElement pe = autoRaise ? QStyle::PE_PanelButtonTool : QStyle::PE_PanelButtonBevel;
    if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::MenuButtonPopup)) {
        tool.rect = opt.rect;
        tool.state = bflags;
        if (opt.activeSubControls & QStyle::SC_ToolButtonMenu) {
            // The menu is active, draw the whole button as selected
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, w);
        } else {
            // The menu is not active, so draw the icon or text area in normal mode
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, w);
            if (tool.state & QStyle::State_MouseOver) {
                if (m_mouseOnSubControl) {
                    if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon) {   // LargeButton
                        tool.rect = m_iconRect;   // mouse is on the text area, show the icon as normal
                    } else {
                        tool.rect =
                            m_iconRect.united(m_textRect);   // mouse is on the arrow area, show icon and text as normal
                    }
                } else {
                    if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon) {   // LargeButton
                        tool.rect = m_textRect.united(
                            m_indicatorArrowRect);   // mouse is on the icon area, show the text as normal
                    } else {
                        tool.rect =
                            m_indicatorArrowRect;   // mouse is not on the arrow area, show the arrow area as normal
                    }
                }
                tool.state = QStyle::State_Raised;
                style()->drawPrimitive(pe, &tool, &p, w);
            }
        }
    } else if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::HasMenu)) {
        tool.rect = opt.rect;
        tool.state = bflags;
        style()->drawPrimitive(pe, &tool, &p, w);
    } else if (opt.subControls & QStyle::SC_ToolButton) {
        tool.rect = opt.rect;
        tool.state = bflags;
        if (opt.state & QStyle::State_Sunken) {
            tool.state &= ~QStyle::State_MouseOver;
        }
        style()->drawPrimitive(pe, &tool, &p, w);
    }
    drawIconAndLabel(opt, p, w);
}

RibbonButton::RibbonButton(QWidget *parent)
    : QToolButton(parent)
{
    QX_INIT_PRIVATE(RibbonButton)
    setAutoRaise(true);
    setButtonType(SmallButton);
    setMouseTracking(true);
}

RibbonButton::RibbonButton(QAction *defaultAction, QWidget *parent)
    : RibbonButton(parent)
{
    setDefaultAction(defaultAction);
}

RibbonButton::RibbonButton(RibbonButtonPrivate *d, QWidget *parent)
    : QToolButton(parent)
{
    QX_SET_PRIVATE(d)
    setAutoRaise(true);
    setButtonType(SmallButton);
    setMouseTracking(true);
}

RibbonButton::~RibbonButton()
{
    QX_FINI_PRIVATE()
}

void RibbonButton::paintEvent(QPaintEvent *event)
{
    Q_D(RibbonButton);
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d->drawButton(opt, p, this);
}

void RibbonButton::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(RibbonButton);
    d->updateButtonState(e->pos());
    QToolButton::mouseMoveEvent(e);
}

void RibbonButton::mousePressEvent(QMouseEvent *e)
{
    Q_D(RibbonButton);
    RibbonScreenTip::hideTip();
    if ((e->button() == Qt::LeftButton) && (popupMode() == MenuButtonPopup)) {
        d->updateButtonState(e->pos());
        if (d->m_mouseOnSubControl) {
            d->m_menuButtonPressed = true;
            showMenu();
            // After showMenu ends, check again whether the mouse is on a SubControl
            d->updateButtonState(mapFromGlobal(QCursor::pos()));
            return;
        }
    }
    d->m_menuButtonPressed = false;
    // Use QAbstractButton's mousePressEvent here, not QToolButton's
    // For a QToolButton with a menu, QToolButton's mousePressEvent pops up the menu when the right side is clicked,
    // and the menu pop-up click position is customized above, so QToolButton's mousePressEvent is not needed
    QAbstractButton::mousePressEvent(e);
}

void RibbonButton::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(RibbonButton);
    QToolButton::mouseReleaseEvent(e);
    d->m_menuButtonPressed = false;
}

void RibbonButton::focusOutEvent(QFocusEvent *e)
{
    Q_D(RibbonButton);
    QToolButton::focusOutEvent(e);
    d->m_mouseOnSubControl = false;
}

void RibbonButton::leaveEvent(QEvent *e)
{
    Q_D(RibbonButton);
    d->m_mouseOnSubControl = false;
    RibbonScreenTip::hideTip();
    QToolButton::leaveEvent(e);
}

bool RibbonButton::hitButton(const QPoint &pos) const
{
    Q_D(const RibbonButton);
    if (QAbstractButton::hitButton(pos)) {
        return !d->m_menuButtonPressed;
    }
    return false;
}

/**
 * @brief Computes the drawing sizes in resizeEvent, avoiding on-the-fly computation during painting
 * @param e
 */
void RibbonButton::resizeEvent(QResizeEvent *e)
{
    Q_D(RibbonButton);
#if DebugRibbonButton_TextDrawPrint
    qDebug() << "RibbonButton::resizeEvent, text=" << text() << " obj=" << objectName() << " size=" << e->size();
#endif
    QToolButton::resizeEvent(e);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d->calcIconAndTextRect(opt);
}

/**
 * @brief A toolbutton's size is determined bottom-up: the text size first, then the icon size, ensuring two lines of
 * text
 * @note Beware of when m_sizeHint is refreshed
 * @return
 */
QSize RibbonButton::sizeHint() const
{
    QSize sz = QToolButton::sizeHint();
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d_ptr->recalcSizeHint(opt, sz);
    return d_ptr->m_sizeHint;
}

void RibbonButton::changeEvent(QEvent *e)
{
    Q_D(RibbonButton);
    if (e) {
        if (e->type() == QEvent::FontChange) {
            // Recompute the font-dependent geometry. The cached size hint needs
            // no invalidation of its own: sizeHint() recomputes it on every
            // call, so it cannot go stale from here.
            QStyleOptionToolButton opt;
            initStyleOption(&opt);
            d->calcIconAndTextRect(opt);
        }
    }
    QToolButton::changeEvent(e);
}

RibbonButton::RibbonButtonType RibbonButton::buttonType() const
{
    Q_D(const RibbonButton);
    return d->m_buttonType;
}

/**
 * @brief Sets the button style
 * @note Setting the button style calls setToolButtonStyle; to change toolButtonStyle,
 * e.g. to Qt::ToolButtonIconOnly, set it after this function
 * @param type
 */
void RibbonButton::setButtonType(RibbonButton::RibbonButtonType type)
{
    Q_D(RibbonButton);
    d->m_buttonType = type;
    if (LargeButton == type) {
        setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    } else {
        setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    }
}

QSize RibbonButton::minimumSizeHint() const
{
    return sizeHint();
}

void RibbonButton::setLargeButtonType(RibbonButton::LargeButtonType type)
{
    Q_D(RibbonButton);
    d->m_largeButtonType = type;
}

RibbonButton::LargeButtonType RibbonButton::largeButtonType() const
{
    Q_D(const RibbonButton);
    return d->m_largeButtonType;
}

/**
 * @brief Makes the text flicker once when the button is clicked
 * @param on
 */
void RibbonButton::setToolButtonTextShift(bool on)
{
    RibbonButtonPrivate::s_isToolButtonTextShift = on;
}

bool RibbonButton::isToolButtonTextShift()
{
    return RibbonButtonPrivate::s_isToolButtonTextShift;
}

namespace
{

/*!
 * \brief Removes the '&' mnemonic markers from a button text.
 */
QString withoutMnemonic(const QString &text)
{
    QString plain;
    plain.reserve(text.size());
    for (int i = 0; i < text.size(); ++i) {
        if (text.at(i) != QLatin1Char('&')) {
            plain.append(text.at(i));
            continue;
        }
        if (i + 1 < text.size() && text.at(i + 1) == QLatin1Char('&')) {   // "&&" stands for a literal '&'
            plain.append(QLatin1Char('&'));
            ++i;
        }
    }
    return plain;
}

/*!
 * \brief Shows the rich screen tip for the button's default action.
 *
 * Returns false when the action carries nothing richer than its text, so the plain
 * tooltip keeps being used and the change stays invisible for plain buttons.
 */
bool showScreenTipFor(RibbonButton *button)
{
    QAction *action = button->defaultAction();
    if (!action) {
        return false;
    }
    const QString text = action->iconText().isEmpty() ? action->text() : action->iconText();
    const QString description = action->statusTip();
    const QString shortcut = action->shortcut().toString(QKeySequence::NativeText);
    if (description.isEmpty() && shortcut.isEmpty()) {
        return false;
    }
    RibbonScreenTip::showTip(button, withoutMnemonic(text), description, shortcut);
    return true;
}

}   // namespace

bool RibbonButton::event(QEvent *e)
{
    Q_D(RibbonButton);
    switch (e->type()) {
    case QEvent::ToolTip:
        if (showScreenTipFor(this)) {
            return true;
        }
        break;
    case QEvent::WindowDeactivate:
    case QEvent::ActionChanged:
    case QEvent::ActionRemoved:
    case QEvent::ActionAdded:
        d->m_mouseOnSubControl = false;
        break;
    default:
        break;
    }

    return QToolButton::event(e);
}

QX_RIBBON_END_NAMESPACE
