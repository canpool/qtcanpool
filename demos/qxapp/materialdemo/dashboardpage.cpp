/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dashboardpage.h"

#include "qxmaterial/materialcard.h"
#include "qxmaterial/materialchip.h"
#include "qxmaterial/materialsegmentedbutton.h"
#include "qxmaterial/materialtokens.h"
#include "qxcontrols/qxcontrol.h"

#include <QtCore/QStringList>
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QX_CONTROLS_USE_NAMESPACE
QX_MATERIAL_USE_NAMESPACE

namespace
{
// The room between two cards of a row, and the room inside one: sixteen is what
// Material puts between two surfaces, and the padding inside a card is the
// card's own margin plus eight.
constexpr int kGap = 16;
constexpr int kCardPadding = 16;
constexpr int kFigureWidth = 216;
constexpr int kFigureHeight = 108;

/*! A card whose contents are laid out by the caller.
 *
 *  A card is a surface and not a container: what goes on it is the screen's
 *  business, so the layout is handed back rather than filled in here. The
 *  contents are inset by the margin the shadow is drawn in, because that band
 *  belongs to the shadow and a label standing on it would be cropped.
 */
QVBoxLayout *fill(MaterialCard *card, int spacing = 4)
{
    auto *layout = new QVBoxLayout(card);
    const int inset = MaterialCard::shadowMargin(card->variant()) + kCardPadding;
    layout->setContentsMargins(inset, inset, inset, inset);
    layout->setSpacing(spacing);
    return layout;
}

QLabel *label(const QString &text, MaterialType::Scale scale, bool strong, QWidget *parent)
{
    auto *line = new QLabel(text, parent);
    line->setFont(MaterialType::font(scale, strong));
    return line;
}

/*! One figure: what it is, what it is now, and which way it moved.
 *
 *  The three lines are three steps of the type scale for one reason: the number
 *  is what the screen is asked for, the caption says what the number is, and
 *  the change is the small print that makes the number mean something.
 */
MaterialCard *figure(const QString &caption, const QString &value, const QString &change, bool up)
{
    // Outlined, because the line is the one thing every theme draws for a
    // surface: a filled card is the palette's AlternateBase, and a theme in
    // which that is the page's own colour would leave the three figures
    // standing on nothing.
    auto *card = new MaterialCard(MaterialCard::Variant::Outlined);
    card->setFixedSize(kFigureWidth, kFigureHeight);
    QVBoxLayout *layout = fill(card);

    layout->addWidget(label(caption, MaterialType::Scale::Label, false, card));
    layout->addWidget(label(value, MaterialType::Scale::Headline, true, card));
    QLabel *delta = label(change, MaterialType::Scale::Label, false, card);
    // Up is the accent and down is the quieter ink: on a dashboard the colour
    // has to be read before the sign is, and a second red for "down" would fight
    // the error colour the theme already owns. The quieter ink is a palette role
    // and not a colour, so "quieter" means the same amount in either scheme.
    if (up) {
        delta->setStyleSheet(QStringLiteral("color: palette(highlight);"));
    } else {
        delta->setForegroundRole(QPalette::PlaceholderText);
    }
    layout->addWidget(delta);
    layout->addStretch(1);
    return card;
}

/*! A row of things beside each other, left aligned, with the room between them
 *  that Material puts between two surfaces. */
QWidget *rowOf(const QList<QWidget *> &widgets, QWidget *parent, int spacing = kGap)
{
    auto *row = new QWidget(parent);
    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(spacing);
    for (QWidget *widget : widgets) {
        layout->addWidget(widget);
    }
    layout->addStretch(1);
    return row;
}
}   // namespace

/*!
 * The bars of the chart, and nothing else: no axis, no legend, no ticks.
 *
 * A dashboard chart is read at a glance or not at all, so what it says is the
 * shape of the series and the names under it - the numbers behind it belong on
 * the figures above. It is a plain widget because it is not a control: it holds
 * no state the user can change and answers no pointer.
 */
class BarChart : public QWidget
{
public:
    explicit BarChart(QWidget *parent)
        : QWidget(parent)
    {
        setMinimumHeight(168);
    }

    void setSeries(const QStringList &labels, const QList<int> &values)
    {
        m_labels = labels;
        m_values = values;
        update();
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        if (m_values.isEmpty() || m_values.size() != m_labels.size()) {
            return;
        }

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const QFont small = MaterialType::font(MaterialType::Scale::Label);
        painter.setFont(small);
        const int labelBand = QFontMetrics(small).height() + 6;
        const QRectF plot = QRectF(rect()).adjusted(0.0, 4.0, 0.0, -labelBand);

        int highest = 0;
        for (int value : m_values) {
            highest = qMax(highest, value);
        }
        if (highest <= 0) {
            return;
        }

        // Three hairlines stand in for an axis: enough to see that a bar is a
        // third of the way up, and few enough that nothing is read off them.
        QPen grid(palette().color(QPalette::Mid));
        grid.setWidthF(1.0);
        painter.setPen(grid);
        for (int step = 1; step <= 3; ++step) {
            const qreal y = plot.bottom() - plot.height() * step / 3.0;
            painter.drawLine(QPointF(plot.left(), y), QPointF(plot.right(), y));
        }

        const int count = m_values.size();
        const qreal gap = 10.0;
        const qreal width = (plot.width() - gap * (count - 1)) / count;
        const qreal radius = qMin(qreal(MaterialShape::radius(MaterialShape::Radius::Small)), width / 2.0);
        const QColor accent = palette().color(QPalette::Highlight);
        for (int i = 0; i < count; ++i) {
            const qreal height = plot.height() * m_values.at(i) / qreal(highest);
            const QRectF bar(plot.left() + i * (width + gap), plot.bottom() - height, width, height);
            painter.setPen(Qt::NoPen);
            painter.setBrush(accent);
            painter.drawRoundedRect(bar, radius, radius);

            painter.setPen(palette().color(QPalette::WindowText));
            const QRectF name(bar.left() - gap / 2.0, plot.bottom() + 4.0, bar.width() + gap, labelBand);
            painter.drawText(name, Qt::AlignHCenter | Qt::AlignTop, m_labels.at(i));
        }
    }
private:
    QStringList m_labels;
    QList<int> m_values;
};

DashboardPage::DashboardPage(QWidget *parent)
    : DemoPage(tr("Dashboard"),
               tr("The screen an application opens on when what it is about is a number that moves: what "
                  "the figures are doing, what they are made of, and what came in while nobody was looking."),
               parent)
{
    addSection(tr("This month"));

    addNote(tr("Three figures, three cards: on Material a surface is how one thing is told apart from the "
               "next, and a row of them is the answer to \"how is it going\" before anything is read."));
    addBlock(rowOf({figure(tr("Revenue"), tr("$48,120"), tr("+12.4% on last month"), true),
                    figure(tr("Orders"), tr("1,204"), tr("+3.1% on last month"), true),
                    figure(tr("Refunds"), tr("18"), tr("-2.0% on last month"), false)},
                   this));

    addSection(tr("What the figures are made of"));

    // The chart and the range it is drawn at are one card: a range selector
    // that sat outside would be a control for a chart it is not beside.
    auto *chartCard = new MaterialCard(MaterialCard::Variant::Outlined);
    QVBoxLayout *chartLayout = fill(chartCard, 8);

    auto *header = new QWidget(chartCard);
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(QMargins());
    headerLayout->setSpacing(kGap);
    headerLayout->addWidget(label(tr("Visitors"), MaterialType::Scale::Title, true, header));
    headerLayout->addStretch(1);
    auto *range = new MaterialSegmentedButton(header);
    range->setItems(QStringList{tr("Day"), tr("Week"), tr("Month")});
    range->setCurrentIndex(1);
    headerLayout->addWidget(range);
    chartLayout->addWidget(header);

    m_chart = new BarChart(chartCard);
    chartLayout->addWidget(m_chart, 1);

    // Three series of different lengths, because the switch has to change the
    // shape of the chart and not only its numbers: a bar chart whose bars stay
    // put says nothing about the range it is drawn at.
    const auto showWeek = [this, range]() {
        switch (range->currentIndex()) {
        case 0:
            m_chart->setSeries(QStringList{tr("00"), tr("04"), tr("08"), tr("12"), tr("16"), tr("20")},
                               {12, 8, 46, 78, 64, 31});
            break;
        case 2:
            m_chart->setSeries(QStringList{tr("W1"), tr("W2"), tr("W3"), tr("W4")}, {240, 318, 275, 402});
            break;
        default:
            m_chart->setSeries(QStringList{tr("Mon"), tr("Tue"), tr("Wed"), tr("Thu"), tr("Fri"), tr("Sat"), tr("Sun")},
                               {58, 74, 61, 92, 88, 43, 22});
            break;
        }
    };
    connect(range, &MaterialSegmentedButton::currentIndexChanged, this, [showWeek](int) {
        showWeek();
    });
    showWeek();

    addBlock(chartCard);

    addSection(tr("What came in"));

    auto *recent = new MaterialCard(MaterialCard::Variant::Elevated);
    QVBoxLayout *recentLayout = fill(recent, 8);
    struct Entry {
        const char *text;
        const char *state;
    };
    const Entry entries[] = {{QT_TR_NOOP("#1042 - Album, twelve photos"), QT_TR_NOOP("paid")},
                             {QT_TR_NOOP("#1041 - Family portrait, one print"), QT_TR_NOOP("printed")},
                             {QT_TR_NOOP("#1040 - Pricing sheet, three pages"), QT_TR_NOOP("waiting")}};
    for (const Entry &entry : entries) {
        auto *line = new QWidget(recent);
        auto *lineLayout = new QHBoxLayout(line);
        lineLayout->setContentsMargins(QMargins());
        lineLayout->setSpacing(kGap);
        lineLayout->addWidget(label(tr(entry.text), MaterialType::Scale::Body, false, line));
        lineLayout->addStretch(1);
        lineLayout->addWidget(label(tr(entry.state), MaterialType::Scale::Label, false, line));
        recentLayout->addWidget(line);
    }
    // One action, and it says what it did: a template whose buttons do nothing
    // is a picture of a screen rather than a screen.
    auto *refresh = new MaterialChip(MaterialChip::Kind::Assist, recent);
    refresh->setText(tr("Refresh"));
    connect(refresh, &QxControl::clicked, this, [this]() {
        Q_EMIT messageRequested(tr("The figures are as fresh as this demo gets: they are drawn in the "
                                   "window, not fetched."));
    });
    recentLayout->addWidget(refresh, 0, Qt::AlignLeft);
    addBlock(recent);

    addStretch();
}
