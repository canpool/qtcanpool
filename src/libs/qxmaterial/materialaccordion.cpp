/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialaccordion.h"

#include "materialtokens.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtCore/QVariantAnimation>
#include <QtCore/QVector>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QVBoxLayout>

#include <functional>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// A header is forty-eight tall, the height of a list row: a name sixteenth in
// from the left, the chevron sixteenth in from the right.
constexpr qreal kHeaderHeight = 48.0;
constexpr qreal kPadding = 16.0;
constexpr qreal kChevron = 24.0;
// The hairline that tells one header from the next, and the wash a header
// takes under the pointer.
constexpr qreal kOutlineOpacity = 0.12;
constexpr qreal kHoverOpacity = 0.08;
constexpr qreal kQuietInkOpacity = 0.6;
// The step a control takes down when nobody can use it - the same .38 the rest
// of the family keeps.
constexpr qreal kDisabledOpacity = 0.38;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

}   // namespace

/*!
 * One header and the body it keeps. It is the column's own part, not the
 * application's: what it answers to is the column, which is where the rule
 * about how many may be open lives.
 */
class MaterialAccordionSection : public QWidget
{
public:
    explicit MaterialAccordionSection(QWidget *parent = Q_NULLPTR)
        : QWidget(parent)
    {
        setFocusPolicy(Qt::StrongFocus);
        setMouseTracking(true);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        connect(&m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
            m_fraction = value.toReal();
            place();
            update();
        });
    }

    QString title() const
    {
        return m_title;
    }
    void setTitle(const QString &title)
    {
        m_title = title;
        update();
    }

    QWidget *content() const
    {
        return m_content;
    }
    void setContent(QWidget *content)
    {
        if (m_content == content) {
            return;
        }
        if (m_content) {
            m_content->hide();
        }
        m_content = content;
        if (m_content) {
            m_content->setParent(this);
            place();
        }
        updateGeometry();
        update();
    }

    bool isExpanded() const
    {
        return m_expanded;
    }
    void setExpanded(bool expanded, bool animated = true)
    {
        if (m_expanded == expanded) {
            return;
        }
        m_expanded = expanded;
        const qreal target = expanded ? 1.0 : 0.0;
        // The family's own answer to how long a movement takes: zero when the
        // application asked for less motion, and then the body is simply out.
        const QxTheme::QxMotion motion = MaterialMotion::forDuration(MaterialMotion::Duration::Medium);
        m_animation.stop();
        if (!animated || motion.duration <= 0) {
            m_fraction = target;
            place();
        } else {
            m_animation.setDuration(motion.duration);
            m_animation.setEasingCurve(motion.curve);
            m_animation.setStartValue(m_fraction);
            m_animation.setEndValue(target);
            m_animation.start();
        }
        update();
    }

    QSize sizeHint() const override
    {
        const int body = int(m_fraction * qreal(bodyHeight()));
        return QSize(320, int(kHeaderHeight) + body);
    }

    /*! What the column calls when this header is clicked or the keyboard asks
     *  it to open. */
    std::function<void()> headerActivated;
protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        // A header nobody can open is written a step down: the name and the
        // chevron together, while the column keeps its shape.
        painter.setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);

        const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
        const QColor page = palette().color(QPalette::Window);
        const QColor ink = palette().color(QPalette::WindowText);
        const QRectF header(0.0, 0.0, qreal(width()), kHeaderHeight);

        if (m_hovered) {
            painter.fillRect(header, QxControls::QxStateLayer::layered(page, ink, kHoverOpacity));
        }

        // The name, then the chevron at the other end.
        painter.setPen(ink);
        painter.setFont(MaterialType::font(MaterialType::Scale::Body));
        painter.drawText(header.adjusted(kPadding, 0.0, -kPadding - kChevron, 0.0),
                         Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, m_title);

        // The chevron turns over as the body comes out: a mark that points at
        // what is kept, and points away once it is out.
        painter.save();
        painter.translate(qreal(width()) - kPadding - kChevron / 2.0, kHeaderHeight / 2.0);
        painter.rotate(180.0 * m_fraction);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(m_expanded ? accent : QxControls::QxStateLayer::layered(page, ink, kQuietInkOpacity), 2.0,
                            Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        QPainterPath chevron;
        chevron.moveTo(-5.0, -2.5);
        chevron.lineTo(0.0, 3.0);
        chevron.lineTo(5.0, -2.5);
        painter.drawPath(chevron);
        painter.restore();

        // The hairline that tells this header from the next one down.
        painter.fillRect(QRectF(0.0, kHeaderHeight - 1.0, qreal(width()), 1.0),
                         QxControls::QxStateLayer::layered(page, ink, kOutlineOpacity));

        if (hasFocus()) {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(accent, 1.0));
            painter.drawRoundedRect(header.adjusted(2.0, 2.0, -2.0, -2.0), 4.0, 4.0);
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton && eventPos(event).y() < kHeaderHeight && headerActivated) {
            headerActivated();
        }
        QWidget::mouseReleaseEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        const bool hovered = eventPos(event).y() < kHeaderHeight;
        if (hovered != m_hovered) {
            m_hovered = hovered;
            update();
        }
        QWidget::mouseMoveEvent(event);
    }

    void leaveEvent(QEvent *event) override
    {
        if (m_hovered) {
            m_hovered = false;
            update();
        }
        QWidget::leaveEvent(event);
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            if (headerActivated) {
                headerActivated();
            }
            event->accept();
            return;
        }
        QWidget::keyPressEvent(event);
    }

    void resizeEvent(QResizeEvent *event) override
    {
        QWidget::resizeEvent(event);
        place();
    }
private:
    int bodyHeight() const
    {
        return m_content ? qMax(0, m_content->sizeHint().height()) : 0;
    }

    void place()
    {
        if (!m_content) {
            return;
        }
        // The body is clipped to what it has been given, so a half-open header
        // shows the top of it rather than a squashed whole.
        m_content->setGeometry(0, int(kHeaderHeight), width(), int(m_fraction * qreal(bodyHeight())));
        m_content->setVisible(m_fraction > 0.0);
        updateGeometry();
    }

    QString m_title;
    QWidget *m_content = Q_NULLPTR;
    bool m_expanded = false;
    bool m_hovered = false;
    /*! How far the body is out: 0 closed, 1 open. */
    qreal m_fraction = 0.0;
    QVariantAnimation m_animation;
};

class MaterialAccordionPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialAccordion)

    QVector<MaterialAccordionSection *> sections;
    QVBoxLayout *layout = Q_NULLPTR;
    MaterialAccordion::Mode mode = MaterialAccordion::Mode::Single;
};

MaterialAccordion::MaterialAccordion(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new MaterialAccordionPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->layout = new QVBoxLayout(this);
    d_ptr->layout->setContentsMargins(0, 0, 0, 0);
    d_ptr->layout->setSpacing(0);
}

MaterialAccordion::~MaterialAccordion()
{
    QX_FINI_PRIVATE()
}

int MaterialAccordion::addSection(const QString &title, QWidget *content)
{
    Q_D(MaterialAccordion);
    auto *section = new MaterialAccordionSection(this);
    section->setTitle(title);
    if (content) {
        section->setContent(content);
    }
    const int index = d->sections.count();
    section->headerActivated = [this, index]() {
        const bool expanded = isSectionExpanded(index);
        setSectionExpanded(index, !expanded);
    };
    d->sections.append(section);
    d->layout->addWidget(section);
    updateGeometry();
    return index;
}

int MaterialAccordion::sectionCount() const
{
    return d_ptr->sections.count();
}

QString MaterialAccordion::sectionTitle(int index) const
{
    if (index < 0 || index >= sectionCount()) {
        return QString();
    }
    return d_ptr->sections.at(index)->title();
}

QWidget *MaterialAccordion::sectionContent(int index) const
{
    if (index < 0 || index >= sectionCount()) {
        return Q_NULLPTR;
    }
    return d_ptr->sections.at(index)->content();
}

bool MaterialAccordion::isSectionExpanded(int index) const
{
    if (index < 0 || index >= sectionCount()) {
        return false;
    }
    return d_ptr->sections.at(index)->isExpanded();
}

MaterialAccordion::Mode MaterialAccordion::mode() const
{
    return d_ptr->mode;
}

QSize MaterialAccordion::sizeHint() const
{
    int height = 0;
    for (const MaterialAccordionSection *section : d_ptr->sections) {
        height += section->sizeHint().height();
    }
    return QSize(320, height > 0 ? height : int(kHeaderHeight));
}

QSize MaterialAccordion::minimumSizeHint() const
{
    return QSize(200, sizeHint().height());
}

void MaterialAccordion::setSectionTitle(int index, const QString &title)
{
    if (index < 0 || index >= sectionCount()) {
        return;
    }
    d_ptr->sections.at(index)->setTitle(title);
}

void MaterialAccordion::setSectionExpanded(int index, bool expanded)
{
    if (index < 0 || index >= sectionCount()) {
        return;
    }
    MaterialAccordionSection *section = d_ptr->sections.at(index);
    if (section->isExpanded() == expanded) {
        return;
    }
    if (expanded && d_ptr->mode == Mode::Single) {
        // One at a time: the header that was open closes first, and says so -
        // a page listening to the column hears both answers.
        for (int i = 0; i < d_ptr->sections.count(); ++i) {
            MaterialAccordionSection *other = d_ptr->sections.at(i);
            if (i != index && other->isExpanded()) {
                other->setExpanded(false);
                Q_EMIT sectionExpandedChanged(i, false);
            }
        }
    }
    section->setExpanded(expanded);
    updateGeometry();
    Q_EMIT sectionExpandedChanged(index, expanded);
}

void MaterialAccordion::setMode(Mode mode)
{
    if (d_ptr->mode == mode) {
        return;
    }
    d_ptr->mode = mode;
    Q_EMIT modeChanged(mode);
}

QX_MATERIAL_END_NAMESPACE
