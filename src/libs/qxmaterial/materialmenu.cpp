/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialmenu.h"

#include "materialtokens.h"

#include "qxtheme/qxthememanager.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QWidget>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's menu measures: a row of forty-eight, the rows keeping sixteen of
// their own from the surface's edges. The leading zone of forty is where an
// icon stands or the primary's check is drawn - one zone, because a command
// wears one or the other.
constexpr qreal kItemHeight = 48.0;
constexpr qreal kSeparatorHeight = 9.0;
constexpr qreal kPaddingH = 16.0;
constexpr qreal kLeadingZone = 40.0;
constexpr qreal kHoverOpacity = 0.08;
constexpr qreal kDisabledOpacity = 0.4;
constexpr int kCheckPenWidth = 2;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}   // namespace

class MaterialMenuPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialMenu)

    QList<QAction *> actions;
    int current = -1;
};

MaterialMenu::MaterialMenu(QWidget *anchor)
    : QxControls::QxPopupSurface(anchor)
    , d_ptr(new MaterialMenuPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setMouseTracking(true);
    setSurfaceRadius(MaterialShape::radius(MaterialShape::Radius::Small));
}

MaterialMenu::~MaterialMenu(){QX_FINI_PRIVATE()}

QAction *MaterialMenu::addAction(const QString &text, const QIcon &icon)
{
    QAction *action = new QAction(text, this);
    action->setIcon(icon);
    d_ptr->actions.append(action);
    update();
    return action;
}

QAction *MaterialMenu::addCheckable(const QString &text, bool checked, const QIcon &icon)
{
    QAction *action = addAction(text, icon);
    action->setCheckable(true);
    action->setChecked(checked);
    return action;
}

QAction *MaterialMenu::addSeparator()
{
    QAction *action = new QAction(this);
    action->setSeparator(true);
    d_ptr->actions.append(action);
    update();
    return action;
}

QList<QAction *> MaterialMenu::actions() const
{
    return d_ptr->actions;
}

void MaterialMenu::clear()
{
    for (QAction *action : d_ptr->actions) {
        action->deleteLater();
    }
    d_ptr->actions.clear();
    d_ptr->current = -1;
    update();
}

QAction *MaterialMenu::currentAction() const
{
    return d_ptr->current >= 0 && d_ptr->current < d_ptr->actions.count() ? d_ptr->actions.at(d_ptr->current)
                                                                          : Q_NULLPTR;
}

void MaterialMenu::setCurrentAction(QAction *action)
{
    d_ptr->current = indexOf(action);
    update();
}

void MaterialMenu::open()
{
    // The keyboard lands on the first command that can be worked, so the
    // menu opens with an answer in hand.
    d_ptr->current = -1;
    for (int index = 0; index < d_ptr->actions.count(); ++index) {
        if (isWorkable(d_ptr->actions.at(index))) {
            d_ptr->current = index;
            break;
        }
    }
    QxControls::QxPopupSurface::open();
}

QSize MaterialMenu::contentSizeHint() const
{
    qreal height = 16.0;   // a paper breathes eight above and below its rows
    for (int index = 0; index < d_ptr->actions.count(); ++index) {
        height += rowHeight(index);
    }
    qreal width = 0.0;
    const QFontMetrics metrics(font());
    for (QAction *action : d_ptr->actions) {
        if (action->isSeparator()) {
            continue;
        }
        width = qMax(width, kPaddingH + kLeadingZone + metrics.horizontalAdvance(action->text()));
    }
    return QSize(int(width + kPaddingH + 56.0), int(height));
}

void MaterialMenu::paintContent(QPainter *painter, const QRectF &content)
{
    const QFontMetrics metrics(font());
    const QColor ink = palette().color(QPalette::WindowText);
    const QColor hairline = QColor(0, 0, 0, 18);

    qreal y = content.top() + 8.0;
    for (int index = 0; index < d_ptr->actions.count(); ++index) {
        QAction *action = d_ptr->actions.at(index);
        const QRectF row(content.left(), y, content.width(), rowHeight(index));

        if (action->isSeparator()) {
            painter->fillRect(QRectF(content.left() + 1.0, row.center().y(), content.width() - 2.0, 1.0), hairline);
        } else {
            // The row's own wash: the keyboard's or the pointer's answer.
            if (index == d_ptr->current && isEnabled()) {
                painter->fillRect(row, QColor(0, 0, 0, int(kHoverOpacity * 255)));
            }

            painter->save();
            painter->setOpacity(action->isEnabled() ? 1.0 : kDisabledOpacity);

            // The leading zone of forty: an icon, or the check in the
            // primary when the command keeps a state that is in force.
            const QRectF leading(content.left() + kPaddingH, row.top(), kLeadingZone, row.height());
            if (action->isCheckable()) {
                if (action->isChecked()) {
                    const QRectF mark(leading.center().x() - 6.0, leading.center().y() - 5.0, 12.0, 10.0);
                    QPen pen(QxTheme::QxThemeManager::instance()->accentColor(), kCheckPenWidth);
                    pen.setCapStyle(Qt::RoundCap);
                    pen.setJoinStyle(Qt::RoundJoin);
                    painter->setPen(pen);
                    painter->setBrush(Qt::NoBrush);
                    painter->setRenderHint(QPainter::Antialiasing, true);
                    QPainterPath tick;
                    tick.moveTo(mark.left(), mark.center().y());
                    tick.lineTo(mark.center().x(), mark.bottom());
                    tick.lineTo(mark.right(), mark.top());
                    painter->drawPath(tick);
                }
            } else if (!action->icon().isNull()) {
                action->icon().paint(painter, leading.toRect(), Qt::AlignCenter,
                                     action->isEnabled() ? QIcon::Normal : QIcon::Disabled);
            }

            painter->setFont(font());
            painter->setPen(ink);
            painter->drawText(QRectF(content.left() + kPaddingH + kLeadingZone, row.top(),
                                     content.right() - content.left() - kPaddingH - kLeadingZone, row.height()),
                              Qt::AlignLeft | Qt::AlignVCenter, action->text());
            painter->restore();
        }
        y += row.height();
    }
}

void MaterialMenu::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_Down: {
        const int count = d_ptr->actions.count();
        if (count == 0) {
            break;
        }
        const int step = event->key() == Qt::Key_Down ? 1 : -1;
        int index = d_ptr->current;
        for (int walked = 0; walked < count; ++walked) {
            index = (index + step + count) % count;
            if (isWorkable(d_ptr->actions.at(index))) {
                d_ptr->current = index;
                update();
                event->accept();
                return;
            }
        }
        break;
    }
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (currentAction() && currentAction()->isEnabled()) {
            QAction *action = currentAction();
            Q_EMIT triggered(action);
            action->trigger();
            close();
            event->accept();
            return;
        }
        break;
    default:
        break;
    }
    QxControls::QxPopupSurface::keyPressEvent(event);
}

void MaterialMenu::mouseMoveEvent(QMouseEvent *event)
{
    // The pointer's answer and the keyboard's are the same one: hovering a
    // command puts the keyboard on it, and hovering elsewhere leaves the
    // keyboard where it was.
    QAction *action = actionAt(eventPos(event));
    if (action && isWorkable(action)) {
        setCurrentAction(action);
    }
    QxControls::QxPopupSurface::mouseMoveEvent(event);
}

void MaterialMenu::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxPopupSurface::mousePressEvent(event);
    QAction *action = actionAt(eventPos(event));
    if (action && isWorkable(action)) {
        setCurrentAction(action);
    }
}

void MaterialMenu::mouseReleaseEvent(QMouseEvent *event)
{
    QxControls::QxPopupSurface::mouseReleaseEvent(event);
    QAction *action = actionAt(eventPos(event));
    if (action && isWorkable(action)) {
        Q_EMIT triggered(action);
        action->trigger();
        close();
    }
}

bool MaterialMenu::isWorkable(QAction *action) const
{
    return action && !action->isSeparator() && action->isEnabled();
}

int MaterialMenu::indexOf(QAction *action) const
{
    return action ? d_ptr->actions.indexOf(action) : -1;
}

QAction *MaterialMenu::actionAt(const QPointF &pos) const
{
    const QPointF local = pos - contentRect().topLeft();
    qreal y = 8.0;
    for (int index = 0; index < d_ptr->actions.count(); ++index) {
        const qreal height = rowHeight(index);
        if (local.y() >= y && local.y() < y + height) {
            return d_ptr->actions.at(index);
        }
        y += height;
    }
    return Q_NULLPTR;
}

int MaterialMenu::rowHeight(int index) const
{
    return d_ptr->actions.at(index)->isSeparator() ? int(kSeparatorHeight) : int(kItemHeight);
}

QX_MATERIAL_END_NAMESPACE
