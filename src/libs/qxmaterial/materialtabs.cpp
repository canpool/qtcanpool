/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialtabs.h"

#include "materialtokens.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtCore/QVariantAnimation>
#include <QtCore/QVector>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// A tab is forty-eight tall whatever it says, keeps sixteen either side of its
// name, and is never narrower than ninety - the smallest a name can be and
// still read as a tab.
constexpr qreal kHeight = 48.0;
constexpr qreal kTextPadding = 16.0;
constexpr qreal kMinTabWidth = 90.0;
// The indicator is the accent, two tall, standing on the row's own edge.
constexpr qreal kIndicatorHeight = 2.0;
// A tab that is not the one in force keeps the page's ink at six tenths; the
// one under the pointer takes a wash first, so it answers before it is
// clicked.
constexpr qreal kQuietInkOpacity = 0.6;
constexpr qreal kHoverOpacity = 0.08;
// The least of the row that is worth showing when the tabs are wider than it.
constexpr qreal kMinVisible = 120.0;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

}   // namespace

class MaterialTabsPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialTabs)

    QStringList labels;
    int current = -1;
    int hovered = -1;
    /*! How far the row has slid to the left: the first tab stands at -offset. */
    qreal offset = 0.0;
    /*! Where the indicator stands now, which is not where it is going while
     *  it travels. */
    QRectF indicator;
    QVariantAnimation animation;

    qreal tabWidth(const QString &label) const
    {
        const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Body));
        return qMax(kMinTabWidth, qreal(metrics.horizontalAdvance(label)) + 2.0 * kTextPadding);
    }
    qreal startOf(int index) const
    {
        qreal x = 0.0;
        for (int i = 0; i < index && i < labels.count(); ++i) {
            x += tabWidth(labels.at(i));
        }
        return x;
    }
    qreal totalWidth() const
    {
        qreal total = 0.0;
        for (const QString &label : labels) {
            total += tabWidth(label);
        }
        return total;
    }
};

MaterialTabs::MaterialTabs(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new MaterialTabsPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->indicator = value.toRectF();
        update();
    });
}

MaterialTabs::~MaterialTabs()
{
    QX_FINI_PRIVATE()
}

void MaterialTabs::addTab(const QString &label)
{
    d_ptr->labels.append(label);
    if (d_ptr->current < 0) {
        d_ptr->current = 0;
        keepCurrentInView();
        placeIndicator(false);
    }
    updateGeometry();
    update();
}

int MaterialTabs::count() const
{
    return d_ptr->labels.count();
}

QString MaterialTabs::tabLabel(int index) const
{
    if (index < 0 || index >= count()) {
        return QString();
    }
    return d_ptr->labels.at(index);
}

QRect MaterialTabs::tabRect(int index) const
{
    if (index < 0 || index >= count()) {
        return QRect();
    }
    const qreal x = d_ptr->startOf(index) - d_ptr->offset;
    return QRect(qRound(x), 0, qRound(d_ptr->tabWidth(d_ptr->labels.at(index))), qRound(kHeight));
}

int MaterialTabs::currentIndex() const
{
    return d_ptr->current;
}

QSize MaterialTabs::sizeHint() const
{
    return QSize(int(d_ptr->totalWidth()), int(kHeight));
}

QSize MaterialTabs::minimumSizeHint() const
{
    return QSize(int(qMin(d_ptr->totalWidth(), kMinVisible)), int(kHeight));
}

void MaterialTabs::setCurrentIndex(int index)
{
    if (index < 0 || index >= count() || index == d_ptr->current) {
        return;
    }
    d_ptr->current = index;
    keepCurrentInView();
    placeIndicator(true);
    update();
    Q_EMIT currentChanged(index);
}

void MaterialTabs::keepCurrentInView()
{
    if (d_ptr->current < 0) {
        return;
    }
    const qreal tabStart = d_ptr->startOf(d_ptr->current);
    const qreal tabEnd = tabStart + d_ptr->tabWidth(d_ptr->labels.at(d_ptr->current));
    const qreal room = qreal(width());
    // A tab that is past the row's right edge pulls the row left just enough;
    // one that is past its left edge pushes it right.
    if (tabEnd - d_ptr->offset > room) {
        d_ptr->offset = tabEnd - room;
    }
    if (tabStart - d_ptr->offset < 0.0) {
        d_ptr->offset = tabStart;
    }
    const qreal limit = qMax(0.0, d_ptr->totalWidth() - room);
    d_ptr->offset = qBound(0.0, d_ptr->offset, limit);
}

void MaterialTabs::placeIndicator(bool travelling)
{
    if (d_ptr->current < 0) {
        d_ptr->indicator = QRectF();
        update();
        return;
    }
    const QRect tab = tabRect(d_ptr->current);
    const QRectF target(qreal(tab.left()), kHeight - kIndicatorHeight, qreal(tab.width()), kIndicatorHeight);

    // The theme's own answer to "how long does a movement take": it is zero
    // when the application asked for less motion, and then the indicator is
    // simply there.
    const QxTheme::QxMotion motion = MaterialMotion::forDuration(MaterialMotion::Duration::Short);
    d_ptr->animation.stop();
    if (!travelling || motion.duration <= 0 || d_ptr->indicator.isNull()) {
        d_ptr->indicator = target;
        update();
        return;
    }
    d_ptr->animation.setDuration(motion.duration);
    d_ptr->animation.setEasingCurve(motion.curve);
    d_ptr->animation.setStartValue(d_ptr->indicator);
    d_ptr->animation.setEndValue(target);
    d_ptr->animation.start();
}

int MaterialTabs::tabAt(const QPointF &pos) const
{
    for (int i = 0; i < count(); ++i) {
        if (tabRect(i).contains(pos.toPoint())) {
            return i;
        }
    }
    return -1;
}

void MaterialTabs::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    const QColor page = palette().color(QPalette::Window);
    const QColor ink = palette().color(QPalette::WindowText);
    const QColor quiet = QxControls::QxStateLayer::layered(page, ink, kQuietInkOpacity);

    for (int i = 0; i < count(); ++i) {
        const QRect tab = tabRect(i);
        // A tab that the row cut off on either side is not drawn at all: half
        // a name is not a name, and the row slides to show whole ones.
        if (tab.right() < 0 || tab.left() > width()) {
            continue;
        }
        if (i == d_ptr->hovered && i != d_ptr->current) {
            painter.fillRect(tab, QxControls::QxStateLayer::layered(page, ink, kHoverOpacity));
        }
        const bool inForce = i == d_ptr->current;
        painter.setPen(inForce ? accent : quiet);
        painter.setFont(MaterialType::font(MaterialType::Scale::Body, inForce));
        painter.drawText(tab, Qt::AlignCenter | Qt::TextSingleLine, d_ptr->labels.at(i));
    }

    // The indicator: the accent, under the tab in force, wherever it is on its
    // way to.
    if (!d_ptr->indicator.isNull()) {
        painter.fillRect(d_ptr->indicator, accent);
    }

    if (hasFocus() && d_ptr->current >= 0) {
        const QRect tab = tabRect(d_ptr->current);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(accent, 1.0));
        painter.drawRoundedRect(QRectF(tab).adjusted(3.0, 3.0, -3.0, -3.0), 4.0, 4.0);
    }
}

void MaterialTabs::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        const int tab = tabAt(eventPos(event));
        if (tab >= 0) {
            setCurrentIndex(tab);
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void MaterialTabs::mouseMoveEvent(QMouseEvent *event)
{
    const int tab = tabAt(eventPos(event));
    if (tab != d_ptr->hovered) {
        d_ptr->hovered = tab;
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void MaterialTabs::leaveEvent(QEvent *event)
{
    if (d_ptr->hovered != -1) {
        d_ptr->hovered = -1;
        update();
    }
    QWidget::leaveEvent(event);
}

void MaterialTabs::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left && d_ptr->current > 0) {
        setCurrentIndex(d_ptr->current - 1);
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Right && d_ptr->current + 1 < count()) {
        setCurrentIndex(d_ptr->current + 1);
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void MaterialTabs::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // A row that changed its room slides to keep the tab in force whole, and
    // the indicator follows it there without travelling - it was not asked to
    // go anywhere, the row moved under it.
    keepCurrentInView();
    placeIndicator(false);
}

QX_MATERIAL_END_NAMESPACE
