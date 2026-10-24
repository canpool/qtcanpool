/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "controlpages.h"

#include "demopage.h"
#include "loadingstate.h"

#include "qxfluent/fluentbadge.h"
#include "qxfluent/fluentbutton.h"
#include "qxfluent/fluentcombobox.h"
#include "qxfluent/fluentbreadcrumbbar.h"
#include "qxfluent/fluentcard.h"
#include "qxfluent/fluentcommandbar.h"
#include "qxfluent/fluentcontentdialog.h"
#include "qxfluent/fluentdropdownbutton.h"
#include "qxfluent/fluentexpander.h"
#include "qxfluent/fluentinfobar.h"
#include "qxfluent/fluentnumberbox.h"
#include "qxfluent/fluentprogressring.h"
#include "qxfluent/fluentrating.h"
#include "qxfluent/fluentreveal.h"
#include "qxfluent/fluentslider.h"
#include "qxfluent/fluentswitch.h"
#include "qxfluent/fluenttabview.h"
#include "qxfluent/fluentteachingtip.h"
#include "qxfluent/fluenttokens.h"
#include "qxfluent/fluenttogglesplitbutton.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QAction>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>

QX_FLUENT_USE_NAMESPACE

namespace
{
/*! These pages are built by free functions, so they have no tr() of their own.
 *
 *  The context is named here rather than borrowed from QObject, so that the
 *  words on these pages are found under one name when the demo is translated -
 *  and so that the same sentence on two pages is one translation and not two.
 */
QString tr(const char *text)
{
    return QCoreApplication::translate("ControlPages", text);
}

/*! A line of body text under a control: what the control answered, said back. */
QLabel *readout(QWidget *parent, const QString &text)
{
    auto *label = new QLabel(text, parent);
    label->setFont(FluentType::font(FluentType::Ramp::Body));
    return label;
}

/*! A rounded surface for a reveal to be painted on.
 *
 *  A reveal is an effect and not a widget: it listens to whatever it was
 *  attached to and is painted by that thing. A screen made of controls has
 *  nowhere to put one - every control paints its own wash already - so this is
 *  the place, and it is the smallest thing that can hold one. */
class RevealSurface : public QWidget
{
public:
    explicit RevealSurface(QWidget *parent)
        : QWidget(parent)
        , m_reveal(new FluentReveal(this))
    {
        setFixedSize(240, 72);
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        // Half a pixel in and one and a half out: the stroke sits inside the
        // widget rather than straddling its edge, which is what keeps a rounded
        // corner from being clipped by it.
        const QRectF area = QRectF(rect()).adjusted(0.5, 0.5, -1.5, -1.5);
        const qreal radius = FluentShape::radius(FluentShape::Radius::Medium);
        painter.setPen(QPen(palette().color(QPalette::Mid), 1));
        painter.setBrush(palette().color(QPalette::Base));
        painter.drawRoundedRect(area, radius, radius);
        // The reveal is the hover wash, so it goes exactly where the flat one
        // would have gone, and the flat one is not painted at all - wearing both
        // would be two answers to the same question.
        painter.setPen(Qt::NoPen);
        m_reveal->paint(&painter, area, radius, palette().color(QPalette::Text));
    }
private:
    FluentReveal *m_reveal = Q_NULLPTR;
};

/*! One card at one elevation, carrying one line.
 *
 *  A card is a surface, and a surface with nothing on it is a shape rather than
 *  an answer - so each of these says what it is for, inset by the margin the
 *  shadow is drawn in, which is the only part of the rectangle the contents may
 *  use. */
FluentCard *card(FluentElevation::Level level, const QString &text, QWidget *parent)
{
    auto *surface = new FluentCard(parent);
    surface->setElevation(level);
    surface->setFixedSize(220, 96);
    auto *layout = new QVBoxLayout(surface);
    const int inset = FluentCard::shadowMargin(level);
    layout->setContentsMargins(inset, inset, inset, inset);
    auto *label = new QLabel(text, surface);
    label->setWordWrap(true);
    label->setFont(FluentType::font(FluentType::Ramp::Body));
    layout->addWidget(label);
    return surface;
}

/*! A row of things that belong beside each other. */
QWidget *row(QWidget *parent, const QList<QWidget *> &widgets)
{
    auto *holder = new QWidget(parent);
    auto *layout = new QHBoxLayout(holder);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(16);
    for (QWidget *widget : widgets) {
        layout->addWidget(widget);
    }
    layout->addStretch(1);
    return holder;
}

DemoPage *numberBoxPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Number box"),
                              tr("A number with a range: typed into the field, or stepped with the two "
                                 "buttons beside it."),
                              parent);
    page->addSection(tr("A value in a range"));

    page->addNote(tr("The buttons are part of the control and not of the application: a number box that "
                     "only accepted typing would be a line edit with an opinion about characters."));
    auto *box = new FluentNumberBox(page);
    box->setRange(1, 60);
    box->setValue(30);
    auto *answer = readout(page, tr("30 minutes"));
    QObject::connect(box, &FluentNumberBox::valueChanged, page, [answer](int value) {
        answer->setText(tr("%1 minutes").arg(value));
    });
    page->addRow(tr("Duration"), box);
    page->addRow(QString(), answer);

    page->addStretch();
    return page;
}

DemoPage *dropDownButtonPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Drop-down button"),
                              tr("A menu with a button's face: the whole of it opens the list, wherever "
                                 "the click lands."),
                              parent);
    page->addSection(tr("Choosing one of a few"));

    page->addNote(tr("A drop-down button answers \"pick one\" rather than \"do this\". Its chevron is drawn "
                     "into the same pressable rectangle as the words, so there is no corner of it that does "
                     "something else."));
    auto *button = new FluentDropDownButton(page);
    button->setText(tr("Zoom"));
    const QStringList levels = {tr("50%"), tr("75%"), tr("100%"), tr("150%"), tr("200%")};
    for (const QString &level : levels) {
        QAction *entry = button->menu()->addAction(level);
        QObject::connect(entry, &QAction::triggered, page, [button, level]() {
            button->setText(level);
        });
    }
    auto *answer = readout(page, tr("Nothing has been chosen."));
    QObject::connect(button->menu(), &QMenu::triggered, page, [answer](QAction *entry) {
        answer->setText(tr("Chosen: %1").arg(entry->text()));
    });
    page->addRow(tr("Zoom"), button);
    page->addRow(QString(), answer);

    page->addStretch();
    return page;
}

DemoPage *toggleSplitButtonPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Toggle split button"),
                              tr("Two commands in one rectangle: the face turns something on, the "
                                 "chevron opens the list of the other ways to do it."),
                              parent);
    page->addSection(tr("A command and its variants"));

    page->addNote(tr("The split is the point: the most likely choice stays one click away, and the rest are "
                     "one click further - which is what a menu beside a button is, drawn as one control."));
    auto *button = new FluentToggleSplitButton(page);
    button->setText(tr("Bold"));
    button->setChecked(true);
    for (const QString &style : {tr("Bold"), tr("Italic"), tr("Underline")}) {
        button->menu()->addAction(style);
    }
    auto *answer = readout(page, tr("Bold is on."));
    QObject::connect(button, &FluentToggleSplitButton::toggled, page, [button, answer](bool on) {
        answer->setText(tr("%1 is %2.").arg(button->text(), on ? tr("on") : tr("off")));
    });
    page->addRow(tr("Style"), button);
    page->addRow(QString(), answer);

    page->addStretch();
    return page;
}

DemoPage *alertsPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Alerts"),
                              tr("The three ways this application says something to the reader without "
                                 "taking the window away from them."),
                              parent);
    page->addSection(tr("Which one to reach for"));

    page->addNote(tr("A content dialog is the one that waits: it is for a question the application cannot "
                     "answer on its own, and nothing else happens until it is answered."));
    page->addNote(tr("An info bar is the one that stays: something has already happened and still matters "
                     "after it has been read - a connection lost, a file only partly saved - and it goes "
                     "when the reader says so."));
    page->addNote(tr("A teaching tip points at what it is about, and is for the one thing the reader has not "
                     "found yet."));
    page->addStretch();
    return page;
}

DemoPage *contentDialogPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Content dialog"),
                              tr("A question asked over the page it is about, on a surface that dims "
                                 "whatever is behind it."),
                              parent);
    page->addSection(tr("Asking before doing"));

    page->addNote(tr("The dialog asks and the page answers: a button says which answer it was and nothing "
                     "else, so what happens next is written here and not in the dialog."));
    auto *launcher = new FluentButton(page);
    launcher->setText(tr("Delete the draft"));
    auto *answer = readout(page, tr("Nothing has been asked yet."));
    QObject::connect(launcher, &FluentButton::clicked, page, [launcher, answer]() {
        // Made per ask and deleted when it closes: a dialog that outlived its
        // question would answer the next one with the last one's states.
        auto *dialog = new FluentContentDialog(launcher->window());
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->setTitle(tr("Delete the draft?"));
        dialog->setMessage(tr("The draft and everything written in it goes. This cannot be undone."));
        dialog->primaryButton()->setText(tr("Delete"));
        dialog->secondaryButton()->setText(tr("Keep it"));
        QObject::connect(dialog->primaryButton(), &QPushButton::clicked, dialog, &QDialog::accept);
        QObject::connect(dialog->secondaryButton(), &QPushButton::clicked, dialog, &QDialog::reject);
        QObject::connect(dialog, &QDialog::accepted, answer, [answer]() {
            answer->setText(tr("Answered: delete it."));
        });
        QObject::connect(dialog, &QDialog::rejected, answer, [answer]() {
            answer->setText(tr("Answered: keep it."));
        });
        dialog->open();
    });
    page->addRow(tr("Ask"), launcher);
    page->addRow(QString(), answer);

    page->addStretch();
    return page;
}

DemoPage *switchPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Switch"),
                              tr("One setting that is either on or off, and says which without a label "
                                 "beside it."),
                              parent);
    page->addSection(tr("Settings that are either on or off"));

    page->addNote(tr("A switch is a state, not an action: it is where a setting is read as well as set, which "
                     "is why it carries its own name and its own accessible state."));
    const QStringList names = {tr("Wi-Fi"), tr("Bluetooth"), tr("Battery saver")};
    for (const QString &name : names) {
        auto *control = new FluentSwitch(page);
        control->setText(name);
        control->setOn(name == names.first());
        page->addRow(name, control);
    }

    page->addStretch();
    return page;
}

DemoPage *sliderPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Slider"),
                              tr("A value in a range, set by pointing: the rail shows how far the range "
                                 "goes and where it is divided."),
                              parent);
    page->addSection(tr("A value in a range"));

    page->addNote(tr("The marks are the intervals a keyboard steps by, drawn so that the reader can see "
                     "the grid the value moves on."));
    auto *slider = new FluentSlider(page);
    slider->setRange(0, 100);
    slider->setTickInterval(25);
    slider->setValue(40);
    auto *answer = readout(page, tr("40 of 100"));
    QObject::connect(slider, &FluentSlider::valueChanged, page, [answer](int value) {
        answer->setText(tr("%1 of 100").arg(value));
    });
    page->addRow(tr("Volume"), slider);
    page->addRow(QString(), answer);

    page->addStretch();
    return page;
}

/*! How many answers the summary is about, and how long they take to arrive.
 *
 *  The duration is the page's own and not a token from the family's motion
 *  vocabulary: nothing is moving from one state to another, the answers are
 *  simply not in hand yet. */
constexpr int kAnswers = 24;
constexpr int kAnswersPerWeek = 3;
constexpr int kCollectMs = 1200;

/*! The run of answers, drawn.
 *
 *  Bars would say the same numbers and a column of numbers says them better. A
 *  line says the one thing neither does - whether the answers are rising - so
 *  the summary holds the average in words and the run of them as a line, and
 *  nothing else.
 *
 *  Two states have to be drawn and they are not the same drawing. With answers
 *  there is a line to read against the grid; with none there is nothing to
 *  average and the honest drawing is a broken line saying the shape is yet to
 *  come - a line drawn flat through zero would be read as "the answers are
 *  terrible" rather than "there are no answers", which is the one thing a
 *  summary must never say by accident.
 *
 *  It follows the palette because it is data and not a photograph: the same
 *  numbers in a dark window are the same numbers, and the accent is what the
 *  theme says it is.
 */
class AnswerTrend : public QWidget
{
public:
    explicit AnswerTrend(QWidget *parent)
        : QWidget(parent)
    {
        setMinimumHeight(96);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void setAnswerCount(int count)
    {
        m_count = qMax(count, 0);
        update();
    }

    /*! The average over the first @p weeks of the run, or zero before there is
     *  a week to average. */
    static qreal average(int weeks)
    {
        const int counted = qBound(0, weeks, int(sizeof(kWeekly) / sizeof(kWeekly[0])));
        if (counted == 0) {
            return 0.0;
        }
        qreal sum = 0.0;
        for (int week = 0; week < counted; ++week) {
            sum += kWeekly[week];
        }
        return sum / counted;
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const QRectF area = QRectF(rect()).adjusted(2.0, 8.0, -2.0, -8.0);
        if (area.width() <= 0.0 || area.height() <= 0.0) {
            return;
        }

        QColor grid = palette().color(QPalette::PlaceholderText);
        grid.setAlpha(48);
        painter.setPen(QPen(grid, 1.0));
        // Three lines: the worst answer, the middle and the best. The scale is
        // one to five, so the middle is a line the eye can judge against.
        for (int step = 0; step <= 2; ++step) {
            const qreal y = area.top() + area.height() * step / 2.0;
            painter.drawLine(QPointF(area.left(), y), QPointF(area.right(), y));
        }

        const int weeks = qMin(m_count / kAnswersPerWeek, int(sizeof(kWeekly) / sizeof(kWeekly[0])));
        if (weeks < 2) {
            // Nothing to join up yet, so the line says so rather than standing
            // for a run of answers that never happened.
            QColor empty = palette().color(QPalette::PlaceholderText);
            empty.setAlpha(120);
            QPen dashes(empty, 1.5, Qt::DashLine);
            painter.setPen(dashes);
            const qreal middle = area.center().y();
            painter.drawLine(QPointF(area.left(), middle), QPointF(area.right(), middle));
            return;
        }

        QPolygonF line;
        for (int week = 0; week < weeks; ++week) {
            const qreal x = area.left() + area.width() * week / (weeks - 1);
            const qreal y = area.bottom() - area.height() * kWeekly[week] / 5.0;
            line.append(QPointF(x, y));
        }

        // What is under the line is the run so far, said quietly: it is the
        // trend that is the message and not the area beneath it.
        QColor under = palette().color(QPalette::Highlight);
        under.setAlpha(38);
        QPainterPath fill;
        fill.moveTo(line.first().x(), area.bottom());
        for (const QPointF &point : line) {
            fill.lineTo(point);
        }
        fill.lineTo(line.last().x(), area.bottom());
        fill.closeSubpath();
        painter.fillPath(fill, under);

        const QColor accent = palette().color(QPalette::Highlight);
        painter.setPen(QPen(accent, 2.0));
        painter.setBrush(Qt::NoBrush);
        painter.drawPolyline(line);

        // A disc at every point, and a larger one at the last: where the run
        // ends is what the reader came to see.
        painter.setPen(Qt::NoPen);
        painter.setBrush(accent);
        for (int week = 0; week < line.size(); ++week) {
            painter.drawEllipse(line.at(week), week == line.size() - 1 ? 4.0 : 2.5,
                                week == line.size() - 1 ? 4.0 : 2.5);
        }
    }
private:
    /*! The average of each week, oldest first, on the scale the reader answers
     *  on. Fixed numbers: a page that drew a different run every time would make
     *  a screenshot of it useless as a record of what it looked like. */
    static constexpr qreal kWeekly[8] = {3.0, 3.5, 2.9, 3.7, 4.1, 3.8, 4.4, 4.1};

    int m_count = 0;
};

constexpr qreal AnswerTrend::kWeekly[8];

/*! What a set of answers adds up to, in one surface: the average, how many
 *  answers it is the average of, and the run they make.
 *
 *  The count is the whole of its interface because the count is the whole of
 *  its state: nothing has arrived, some has, or all of it has. */
class RatingSummary : public QWidget
{
public:
    explicit RatingSummary(QWidget *parent)
        : QWidget(parent)
    {
        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(QMargins());
        layout->setSpacing(2);

        auto *headline = new QWidget(this);
        auto *headlineLayout = new QHBoxLayout(headline);
        headlineLayout->setContentsMargins(QMargins());
        headlineLayout->setSpacing(12);
        m_average = new QLabel(headline);
        m_average->setFont(FluentType::font(FluentType::Ramp::Title, true));
        headlineLayout->addWidget(m_average, 0, Qt::AlignBottom);
        m_words = new QLabel(headline);
        m_words->setFont(FluentType::font(FluentType::Ramp::Caption));
        m_words->setForegroundRole(QPalette::PlaceholderText);
        headlineLayout->addWidget(m_words, 0, Qt::AlignBottom);
        headlineLayout->addStretch(1);
        layout->addWidget(headline);

        m_trend = new AnswerTrend(this);
        layout->addWidget(m_trend, 1);

        auto *caption = new QLabel(this);
        caption->setFont(FluentType::font(FluentType::Ramp::Caption));
        caption->setForegroundRole(QPalette::PlaceholderText);
        caption->setText(QCoreApplication::translate("ControlPages", "The last eight weeks"));
        layout->addWidget(caption);

        setAnswerCount(0);
    }

    void setAnswerCount(int count)
    {
        m_trend->setAnswerCount(qMax(count, 0));

        if (count <= 0) {
            m_average->setText(QStringLiteral("\u2014"));
            m_words->setText(QCoreApplication::translate("ControlPages", "No answers yet"));
            return;
        }

        // To one decimal: a second one would be a precision nobody rates at.
        const int weeks = qMax(count / kAnswersPerWeek, 1);
        m_average->setText(QString::number(AnswerTrend::average(weeks), 'f', 1));
        m_words->setText(QCoreApplication::translate("ControlPages", "out of 5, from %1 answers").arg(count));
    }
private:
    QLabel *m_average = Q_NULLPTR;
    QLabel *m_words = Q_NULLPTR;
    AnswerTrend *m_trend = Q_NULLPTR;
};

DemoPage *ratingPage(QWidget *parent)
{
    auto *page =
        new DemoPage(tr("Rating"), tr("What a set of answers adds up to, and the one the reader gives."), parent);
    page->addSection(tr("What the answers say"));

    page->addNote(tr("A question asked and not yet answered leaves a screen with nothing to show, and "
                     "nothing is the one thing it can show wrongly: an empty surface reads as an answer. "
                     "So the screen says what it is doing - a ring where the summary will be, an average "
                     "of a dash rather than of zero - and then shows what the answers came to."));

    auto *summary = new RatingSummary(page);
    auto *surface = new FluentCard(page);
    surface->setElevation(FluentElevation::Level::Depth4);
    auto *surfaceLayout = new QVBoxLayout(surface);
    const int inset = FluentCard::shadowMargin(FluentElevation::Level::Depth4) + 8;
    surfaceLayout->setContentsMargins(inset, inset, inset, inset);
    surfaceLayout->addWidget(summary);

    auto *arrival = new QStackedWidget(page);

    auto *waiting = new QWidget(arrival);
    auto *waitingLayout = new QVBoxLayout(waiting);
    waitingLayout->setContentsMargins(QMargins());
    waitingLayout->setSpacing(10);
    waitingLayout->addStretch(1);
    auto *ring = new FluentProgressRing(waiting);
    // Indeterminate: the screen does not know how long the answers will take,
    // and a ring that claimed to know would be a lie the user waits on.
    ring->setIndeterminate(true);
    ring->setFixedSize(36, 36);
    waitingLayout->addWidget(ring, 0, Qt::AlignHCenter);
    auto *words = readout(waiting, tr("Collecting the answers"));
    words->setForegroundRole(QPalette::PlaceholderText);
    waitingLayout->addWidget(words, 0, Qt::AlignHCenter);
    waitingLayout->addStretch(1);

    auto *arrived = new QWidget(arrival);
    auto *arrivedLayout = new QVBoxLayout(arrived);
    arrivedLayout->setContentsMargins(QMargins());
    arrivedLayout->addWidget(surface);

    arrival->addWidget(waiting);
    arrival->addWidget(arrived);
    page->addBlock(arrival);

    auto *loading = new LoadingState(arrival, waiting, arrived, page);
    // The answers are already here in the summary: what the wait switches is
    // whether the screen is showing them, which is what a screen that has just
    // asked for them does.
    loading->startOnFirstShow(page, kCollectMs);
    QObject::connect(loading, &LoadingState::finished, page, [summary]() {
        summary->setAnswerCount(kAnswers);
    });

    page->addSection(tr("Your answer"));

    page->addNote(tr("A rating is a slider with pictures: the same keyboard steps, the same contract, and the "
                     "same honest answer when a screen reader asks what it is worth."));
    auto *rating = new FluentRating(page);
    rating->setCount(5);
    rating->setValue(3);
    auto *answer = readout(page, tr("3 of 5"));
    QObject::connect(rating, &FluentRating::valueChanged, page, [answer](int value) {
        answer->setText(tr("%1 of 5").arg(value));
    });
    page->addRow(tr("Stars"), rating);
    page->addRow(QString(), answer);

    // The two states the screen can be in, on demand: fetching again, and
    // having nothing at all. A demo of a wait nobody can call for would only
    // ever be seen by whoever watched it at start-up.
    auto *collect = new FluentButton(page);
    collect->setText(tr("Collect them again"));
    auto *clear = new FluentButton(page);
    clear->setText(tr("Clear the answers"));
    QObject::connect(collect, &FluentButton::clicked, page, [loading]() {
        loading->start(kCollectMs);
    });
    QObject::connect(clear, &FluentButton::clicked, page, [summary]() {
        summary->setAnswerCount(0);
    });
    page->addBlock(row(page, {collect, clear}));

    page->addStretch();
    return page;
}

DemoPage *badgePage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Badge"),
                              tr("A count the user has not looked at: a number, an upper bound, and the "
                                 "refusal to grow wider than the bound."),
                              parent);
    page->addSection(tr("A count that is waiting"));

    page->addNote(tr("Past its maximum a badge stops counting and says so: \"99+\" is read at a glance, "
                     "where \"127\" is a number to work out, and a wider badge moves whatever it sits on."));
    auto *badge = new FluentBadge(page);
    badge->setCount(12);
    badge->setMaximumCount(99);
    auto *add = new FluentButton(page);
    add->setText(tr("Add one"));
    auto *clear = new FluentButton(page);
    clear->setText(tr("Clear"));
    QObject::connect(add, &FluentButton::clicked, badge, [badge]() {
        badge->setCount(qMin(badge->count() + 1, badge->maximumCount() + 20));
    });
    QObject::connect(clear, &FluentButton::clicked, badge, [badge]() {
        badge->setCount(0);
    });
    page->addBlock(row(page, {badge, add, clear}));

    page->addStretch();
    return page;
}

DemoPage *cardPage(QWidget *parent)
{
    auto *page =
        new DemoPage(tr("Card"), tr("A surface to put content on, and how far above the page it sits."), parent);
    page->addSection(tr("Surfaces"));

    page->addNote(tr("Elevation is not decoration: it is how far above the page a surface sits, and the "
                     "shadow is how a Fluent screen says it. The margin the shadow needs is part of the "
                     "card's own geometry, so content that ignored it would be drawn under the shadow."));
    page->addBlock(row(page, {card(FluentElevation::Level::Depth4, tr("At rest on the page."), page),
                              card(FluentElevation::Level::Depth8, tr("Lifted: a card that is hovered."), page),
                              card(FluentElevation::Level::Depth16, tr("A menu, or a flyout."), page)}));

    page->addStretch();
    return page;
}

DemoPage *statusPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Status"),
                              tr("What a screen says about itself while nothing is being asked of the "
                                 "reader: that something is running, that something has news, that there is "
                                 "more here than the window shows."),
                              parent);
    page->addSection(tr("The smallest form that carries it"));

    page->addNote(tr("A progress ring answers \"how long\" when the work cannot yet say: it says that "
                     "something is running, which is already more than an empty pane says."));
    page->addNote(tr("A badge is a count on the thing the count is about, and belongs on that control rather "
                     "than in a corner of the window the reader has to go and look at."));
    page->addNote(tr("A reveal is for the one line a control needs and the layout has no room for: it appears "
                     "on the pointer, and nothing around it moves to make room."));
    page->addStretch();
    return page;
}

DemoPage *progressRingPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Progress ring"),
                              tr("Waiting: an arc when the length of the wait is known, a sweep when it "
                                 "is not."),
                              parent);
    page->addSection(tr("Waiting"));

    page->addNote(tr("A progress indicator that stops moving is one the user takes for a hang, which is why "
                     "the indeterminate one sweeps and the determinate one does not."));
    auto *known = new FluentProgressRing(page);
    known->setValue(65);
    page->addRow(tr("How far"), known);

    auto *drive = new FluentSlider(page);
    drive->setRange(0, 100);
    drive->setValue(65);
    QObject::connect(drive, &FluentSlider::valueChanged, known, &FluentProgressRing::setValue);
    page->addRow(tr("Set it"), drive);

    auto *unknown = new FluentProgressRing(page);
    unknown->setIndeterminate(true);
    page->addRow(tr("How far is not known"), unknown);

    page->addStretch();
    return page;
}

DemoPage *infoBarPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Info bar"),
                              tr("Something the application has to say, said where it happened and stayed "
                                 "until it is read."),
                              parent);
    page->addSection(tr("Something happened"));

    page->addNote(tr("An info bar is dismissed by the user and not by a timer: a message that leaves on its "
                     "own is a message about something that has already been forgotten."));
    auto *info = new FluentInfoBar(page);
    info->setSeverity(FluentInfoBar::Severity::Success);
    info->setTitle(tr("Saved"));
    info->setMessage(tr("The four severities are four colours, and the colour is the answer to how much the "
                        "user has to care."));
    info->setOpen(true);
    page->addBlock(info);

    // The answers are the severities in their own order, so a row's index is
    // its severity: a list of data payloads beside a list of answers is a
    // second thing to keep in step, and it can only drift.
    auto *severity = new FluentComboBox(page);
    severity->addItem(tr("Informational"));
    severity->addItem(tr("Success"));
    severity->addItem(tr("Warning"));
    severity->addItem(tr("Error"));
    severity->setCurrentIndex(1);
    QObject::connect(severity, &FluentComboBox::currentIndexChanged, info, [info](int index) {
        info->setSeverity(static_cast<FluentInfoBar::Severity>(index));
    });
    page->addRow(tr("Severity"), severity);

    auto *open = new FluentSwitch(page);
    open->setText(tr("Show it"));
    open->setOn(true);
    QObject::connect(open, &FluentSwitch::toggled, info, &FluentInfoBar::setOpen);
    page->addRow(tr("Open"), open);

    page->addStretch();
    return page;
}

DemoPage *teachingTipPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Teaching tip"),
                              tr("A tip that knows what it is about: it points at the thing it was given "
                                 "and opens on the side there is room for."),
                              parent);
    page->addSection(tr("Pointing at things"));

    page->addNote(tr("The tip is told its target rather than its position, so it stays right when the "
                     "window is resized, scrolled or moved to another screen - a popup placed by "
                     "coordinates would be wrong on all three."));
    auto *target = new FluentButton(page);
    target->setText(tr("Point at me"));
    QObject::connect(target, &FluentButton::clicked, page, [page, target]() {
        auto *tip = new FluentTeachingTip(target->window());
        tip->setAttribute(Qt::WA_DeleteOnClose);
        tip->setTitle(tr("A teaching tip"));
        tip->setMessage(tr("It points at the thing it is about, and closes when the pointer goes "
                           "somewhere else."));
        tip->setTarget(target);
        tip->setPreferredSide(FluentTeachingTip::Side::Bottom);
        tip->setOpen(true);
    });
    page->addRow(tr("Tip"), target);

    page->addStretch();
    return page;
}

DemoPage *expanderPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Expander"), tr("A header that is the summary of a body it can fold away."), parent);
    page->addSection(tr("Folding a part of the page"));

    page->addNote(tr("What an expander hides has to be worth the click: a summary that says nothing is a "
                     "door with no room behind it."));
    auto *expander = new FluentExpander(page);
    expander->setTitle(tr("What an expander hides"));
    auto *body = new QLabel(tr("A body the header is the summary of: the header stands in for it while it "
                               "is folded, so it has to be able to."));
    body->setWordWrap(true);
    body->setMinimumHeight(64);
    expander->setContent(body);
    expander->setExpanded(true);
    page->addBlock(expander);

    page->addStretch();
    return page;
}

DemoPage *breadcrumbBarPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Breadcrumb bar"),
                              tr("Where the reader is, and the way back up: one path, each part of it a "
                                 "thing that can be gone to."),
                              parent);
    page->addSection(tr("Where this is"));

    page->addNote(tr("A breadcrumb is a path and not a history: it says how deep the reader is, not how they "
                     "arrived, which is why it does not grow with the clicks."));
    auto *crumbs = new FluentBreadcrumbBar(page);
    crumbs->setItems({tr("Library"), tr("Fluent"), tr("Controls")});
    auto *answer = readout(page, tr("Nothing has been clicked."));
    QObject::connect(crumbs, &FluentBreadcrumbBar::itemClicked, page, [answer](int index, const QString &text) {
        answer->setText(tr("Went to %1 (part %2).").arg(text).arg(index + 1));
    });
    page->addBlock(crumbs);
    page->addRow(QString(), answer);

    page->addStretch();
    return page;
}

DemoPage *commandBarPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Command bar"),
                              tr("The commands that matter, in one row, and the ones that do not fit kept "
                                 "behind a button rather than dropped."),
                              parent);
    page->addSection(tr("A row of commands"));

    page->addNote(tr("The overflow is the control's own decision and not the application's: a bar that "
                     "dropped what did not fit would be one where a narrow window silently loses "
                     "features."));
    auto *bar = new FluentCommandBar(page);
    QAction *cut = bar->addCommand(FluentCommandBar::ItemRole::Button, tr("Cut"));
    QAction *copy = bar->addCommand(FluentCommandBar::ItemRole::Button, tr("Copy"));
    QAction *paste = bar->addCommand(FluentCommandBar::ItemRole::Button, tr("Paste"));
    paste->setEnabled(false);
    QAction *rename = bar->addCommand(FluentCommandBar::ItemRole::Button, tr("Rename"));
    QAction *share = bar->addCommand(FluentCommandBar::ItemRole::DropDown, tr("Share"));
    for (const QString &target : {tr("By link"), tr("By mail")}) {
        share->menu()->addAction(target);
    }
    page->addBlock(bar);

    auto *answer = readout(page, tr("No command has been run."));
    const QList<QAction *> commands = {cut, copy, rename};
    for (QAction *command : commands) {
        QObject::connect(command, &QAction::triggered, answer, [answer, command]() {
            answer->setText(tr("Ran %1.").arg(command->text()));
        });
    }
    page->addRow(QString(), answer);

    page->addStretch();
    return page;
}

DemoPage *revealPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Reveal"),
                              tr("The hover wash a surface can wear without a control being there: a light "
                                 "that follows the pointer across a rounded edge."),
                              parent);
    page->addSection(tr("A surface that answers the pointer"));

    page->addNote(tr("A reveal is an effect and not a widget - it listens to whatever it was attached to and "
                     "is painted by that thing - so a screen made of controls has to make room for one. The "
                     "surface below is that room."));
    page->addBlock(row(page, {new RevealSurface(page)}));

    page->addStretch();
    return page;
}

DemoPage *tabViewPage(QWidget *parent)
{
    auto *page = new DemoPage(tr("Tab view"),
                              tr("Several screens in one place, one of them in front, each kept alive while "
                                 "another is shown."),
                              parent);
    page->addSection(tr("Rooms in one window"));

    page->addNote(tr("The tabs are QActions because that is what an application already has: one can be "
                     "disabled, renamed, or reached from elsewhere without the view knowing."));
    auto *view = new FluentTabView(page);
    view->setClosable(true);
    for (const QString &name : {tr("Overview"), tr("Members"), tr("Settings")}) {
        auto *screen = new QLabel(tr("The %1 screen.").arg(name.toLower()));
        screen->setAlignment(Qt::AlignCenter);
        screen->setMinimumHeight(120);
        view->addTab(screen, name);
    }
    page->addBlock(view);

    page->addStretch();
    return page;
}
}   // namespace

QList<ControlPages::ControlGroup> ControlPages::build(QWidget *parent)
{
    // The order inside a group follows what a reader came for rather than what
    // the library gained last: a group is already the answer to "where is the
    // new one", since nothing in it is more than a patch old. What the four
    // newest are is a question Settings - or the release notes - answers better
    // than a pane whose order changes with every patch.
    // Feedback is the one group that arrives in two parts, and it is the group
    // that shows a pane can fold: a destination over other destinations is the
    // only thing a fold has to act on, and a fold on the narrow window is only
    // reachable from one. The part is a page of its own rather than a heading,
    // because what is opened has to be somewhere to go.
    // The glyph beside each name says what the page is about rather than which
    // patch added it: a column of identical marks is a column a reader has to
    // read, and a pane whose marks say nothing is spending the width words
    // would have used. The two parts carry the mark of what is under them -
    // a warning sign over the alerts, a dial over the status pages - because a
    // part is a destination too and is listed like one.
    const QString alerts = DemoPage::tr("Alerts");
    const QString status = DemoPage::tr("Status");
    return {{DemoPage::tr("Commands"),
             {{dropDownButtonPage(parent), QString(), QStringLiteral("square-chevron-down")},
              {toggleSplitButtonPage(parent), QString(), QStringLiteral("arrows-split-2")},
              {commandBarPage(parent), QString(), QStringLiteral("grid-dots")},
              {breadcrumbBarPage(parent), QString(), QStringLiteral("route")}}},
            {DemoPage::tr("Input"),
             {{numberBoxPage(parent), QString(), QStringLiteral("arrows-up-down")},
              {switchPage(parent), QString(), QStringLiteral("toggle-left")},
              {sliderPage(parent), QString(), QStringLiteral("adjustments")},
              {ratingPage(parent), QString(), QStringLiteral("star")}}},
            {DemoPage::tr("Surfaces"),
             {{cardPage(parent), QString(), QStringLiteral("layout-cards")},
              {expanderPage(parent), QString(), QStringLiteral("chevrons-down")},
              {tabViewPage(parent), QString(), QStringLiteral("tabs")}}},
            {DemoPage::tr("Feedback"),
             {{alertsPage(parent), QString(), QStringLiteral("alert-triangle")},
              {contentDialogPage(parent), alerts, QStringLiteral("app-window")},
              {infoBarPage(parent), alerts, QStringLiteral("info-circle")},
              {teachingTipPage(parent), alerts, QStringLiteral("bulb")},
              {statusPage(parent), QString(), QStringLiteral("gauge")},
              {progressRingPage(parent), status, QStringLiteral("progress")},
              {badgePage(parent), status, QStringLiteral("bell")},
              {revealPage(parent), status, QStringLiteral("sparkles")}}}};
}
