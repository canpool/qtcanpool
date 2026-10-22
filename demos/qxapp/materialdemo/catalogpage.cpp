/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "catalogpage.h"

#include "qxmaterial/materialbadge.h"
#include "qxmaterial/materialcard.h"
#include "qxmaterial/materialchip.h"
#include "qxmaterial/materialdialog.h"
#include "qxmaterial/materialprogressring.h"
#include "qxmaterial/materialripple.h"
#include "qxmaterial/materialsegmentedbutton.h"
#include "qxmaterial/materialskeleton.h"
#include "qxmaterial/materialslider.h"
#include "qxmaterial/materialswitch.h"
#include "qxmaterial/materialtextfield.h"
#include "qxmaterial/materialtokens.h"
#include "qxcontrols/qxcontrol.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtGui/QPainter>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QX_CONTROLS_USE_NAMESPACE
QX_MATERIAL_USE_NAMESPACE

namespace
{
constexpr int kGap = 16;
constexpr int kCardWidth = 220;
// Wide enough for a name and a few characters of what is being typed.
constexpr int kFieldWidth = 300;
constexpr int kCardHeight = 104;

/*! One card at one variant, carrying one line: the three answers to "where
 *  does this surface sit" side by side, which is the only way to read them. */
MaterialCard *sample(MaterialCard::Variant variant, const QString &text, QWidget *parent)
{
    auto *surface = new MaterialCard(variant, parent);
    surface->setFixedSize(kCardWidth, kCardHeight);
    auto *layout = new QVBoxLayout(surface);
    const int inset = MaterialCard::shadowMargin(variant);
    layout->setContentsMargins(inset, inset, inset, inset);
    auto *label = new QLabel(text, surface);
    label->setWordWrap(true);
    label->setFont(MaterialType::font(MaterialType::Scale::Body));
    layout->addWidget(label);
    return surface;
}

/*! A row of things beside each other, left aligned. */
QWidget *rowOf(const QList<QWidget *> &widgets, QWidget *parent, int spacing = 8)
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

/*! A surface for a ripple to sit on.
 *
 *  A ripple is an effect and not a widget: it listens to whatever it was
 *  attached to and is painted by that thing, so a screen made of controls has
 *  nowhere to put one - every control paints its own answer already. This is
 *  that place, and it is the smallest thing that can hold one. */
class RippleSurface : public QWidget
{
public:
    explicit RippleSurface(QWidget *parent)
        : QWidget(parent)
        , m_ripple(new MaterialRipple(this))
    {
        setFixedSize(240, 72);
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        // Half a pixel in and one and a half out: the stroke sits inside the
        // widget rather than straddling its edge, which keeps a rounded corner
        // from being clipped by it.
        const QRectF area = QRectF(rect()).adjusted(0.5, 0.5, -1.5, -1.5);
        const qreal radius = MaterialShape::radius(MaterialShape::Radius::Medium);
        painter.setPen(QPen(palette().color(QPalette::Mid), 1));
        painter.setBrush(palette().color(QPalette::Base));
        painter.drawRoundedRect(area, radius, radius);
        // The ripple is the press answer, so it goes exactly where the flat one
        // would have gone, and the flat one is not painted at all - a surface
        // that wears both gives two answers to one press.
        painter.setPen(Qt::NoPen);
        m_ripple->paint(&painter, area, radius);
    }
private:
    MaterialRipple *m_ripple = Q_NULLPTR;
};
}   // namespace

CatalogPage::CatalogPage(QWidget *parent)
    : DemoPage(tr("Catalog"),
               tr("Every control the library ships, once, on one screen: the surfaces, the fields, the "
                  "things the user chooses, and the things the application says while it works."),
               parent)
{
    addSection(tr("The window itself"));

    addNote(tr("Four of the controls are not on this page because the page is already showing them: the top "
               "app bar is this window's caption, the navigation rail is the column on the left, the floating "
               "action button is the round one above it, and the snackbar is what rises from the bottom "
               "edge when one of these rows has something to say."));

    auto *say = new MaterialChip(MaterialChip::Kind::Assist, this);
    say->setText(tr("Say something"));
    connect(say, &QxControl::clicked, this, [this]() {
        Q_EMIT messageRequested(tr("A snackbar is a sentence at the bottom of what is going on. It goes away "
                                   "on its own, because what it says is not something the user has to "
                                   "answer."));
    });
    auto *ask = new MaterialChip(MaterialChip::Kind::Assist, this);
    ask->setText(tr("Ask a question"));
    connect(ask, &QxControl::clicked, this, &CatalogPage::openDialog);
    addBlock(rowOf({say, ask}, this, kGap));

    addSection(tr("Surfaces"));

    auto *cards = new QWidget(this);
    auto *cardLayout = new QHBoxLayout(cards);
    cardLayout->setContentsMargins(QMargins());
    cardLayout->setSpacing(kGap);
    cardLayout->addWidget(sample(MaterialCard::Variant::Elevated,
                                 tr("Elevated: it casts a shadow, and is the page's own colour."), cards));
    cardLayout->addWidget(
        sample(MaterialCard::Variant::Filled, tr("Filled: one step from the page, and casts nothing."), cards));
    cardLayout->addWidget(sample(MaterialCard::Variant::Outlined,
                                 tr("Outlined: the line is the whole of what makes it a surface."), cards));
    cardLayout->addStretch(1);
    addBlock(cards);

    addNote(tr("A press in most toolkits is a flat tint that switches on, which says that this was pressed and "
               "nothing about where. A ripple starts where the pointer landed, spreads to the furthest corner "
               "and fades as it goes - so the surface answers from the place it was touched."));
    addBlock(new RippleSurface(this));

    addSection(tr("What can be typed"));

    addNote(tr("The label is the field's name and not its prompt: it stands while the field is filled, "
               "smaller and out of the way. The two variants are the two answers to \"where does the field "
               "end\" - a container tinted with the page's own ink, or a stroke with the label floating onto "
               "it."));

    auto *outlined = new MaterialTextField(this);
    outlined->setVariant(MaterialTextField::Variant::Outlined);
    outlined->setLabel(tr("Email"));
    outlined->setMinimumWidth(kFieldWidth);
    addRow(tr("Outlined"), outlined);

    auto *filled = new MaterialTextField(this);
    filled->setVariant(MaterialTextField::Variant::Filled);
    filled->setLabel(tr("Street"));
    filled->setMinimumWidth(kFieldWidth);
    addRow(tr("Filled"), filled);

    addSection(tr("What can be chosen"));

    struct Setting {
        const char *name;
        bool on;
    };
    const Setting settings[] = {
        {QT_TR_NOOP("Wi-Fi"), true}, {QT_TR_NOOP("Bluetooth"), false}, {QT_TR_NOOP("Battery saver"), false}};
    for (const Setting &setting : settings) {
        auto *control = new MaterialSwitch(this);
        control->setText(tr(setting.name));
        control->setOn(setting.on);
        addRow(tr(setting.name), control);
    }

    auto *segmented = new MaterialSegmentedButton(this);
    segmented->setItems(QStringList{tr("Day"), tr("Week"), tr("Month")});
    segmented->setCurrentIndex(1);
    addRow(tr("One of a fixed number"), segmented);

    auto *slider = new MaterialSlider(this);
    slider->setRange(0, 100);
    slider->setValue(40);
    QLabel *readout = new QLabel(tr("40 of 100"), this);
    readout->setFont(MaterialType::font(MaterialType::Scale::Body));
    connect(slider, &MaterialSlider::valueChanged, this, [readout](int value) {
        readout->setText(tr("%1 of 100").arg(value));
    });
    addRow(tr("A value in a range"), slider);
    addRow(QString(), readout);

    addSection(tr("What the user is carrying"));

    auto *assist = new MaterialChip(MaterialChip::Kind::Assist, this);
    assist->setText(tr("Suggest an edit"));
    auto *filter = new MaterialChip(MaterialChip::Kind::Filter, this);
    filter->setText(tr("Widgets"));
    // A filter chip that is in force is filled and carries a mark, so "which of
    // these are on" is readable without pressing each one to find out.
    filter->setChecked(true);
    auto *tools = new MaterialChip(MaterialChip::Kind::Filter, this);
    tools->setText(tr("Tools"));
    addBlock(rowOf({assist, filter, tools}, this));

    addSection(tr("While the application works"));

    auto *known = new MaterialProgressRing(this);
    // A ring is born indeterminate: a value it has been handed while it still
    // sweeps is stored but not shown, so being determinate is said out loud.
    known->setIndeterminate(false);
    known->setValue(65);
    auto *drive = new MaterialSlider(this);
    drive->setRange(0, 100);
    drive->setValue(65);
    connect(drive, &MaterialSlider::valueChanged, known, &MaterialProgressRing::setValue);
    addRow(tr("How far"), known);
    addRow(tr("Set it"), drive);

    auto *unknown = new MaterialProgressRing(this);
    unknown->setIndeterminate(true);
    addRow(tr("How far is not known"), unknown);

    auto *shapes = new QWidget(this);
    auto *shapeLayout = new QHBoxLayout(shapes);
    shapeLayout->setContentsMargins(QMargins());
    shapeLayout->setSpacing(8);
    auto *skeletonLine = new MaterialSkeleton(MaterialSkeleton::Shape::Rectangle, shapes);
    skeletonLine->setFixedSize(220, 16);
    auto *skeletonFace = new MaterialSkeleton(MaterialSkeleton::Shape::Circle, shapes);
    skeletonFace->setFixedSize(48, 48);
    shapeLayout->addWidget(skeletonLine);
    shapeLayout->addWidget(skeletonFace);
    shapeLayout->addStretch(1);
    addRow(tr("What has not arrived"), shapes);

    addSection(tr("A count"));

    auto *badge = new MaterialBadge(this);
    badge->setCount(12);
    badge->setMaximumCount(99);
    auto *add = new QPushButton(tr("Add one"));
    auto *clear = new QPushButton(tr("Clear"));
    connect(add, &QPushButton::clicked, this, [badge]() {
        // Past its maximum a badge stops counting and says so: "127" is not more
        // useful than "99+", and it is wider.
        badge->setCount(qMin(badge->count() + 1, badge->maximumCount() + 20));
    });
    connect(clear, &QPushButton::clicked, this, [badge]() {
        badge->setCount(0);
    });
    addBlock(rowOf({badge, add, clear}, this));

    addStretch();
}

void CatalogPage::openDialog()
{
    auto *dialog = new MaterialDialog(this);
    // Every question is a new dialog: one kept around would come back carrying
    // the last answer's state.
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    dialog->setTitle(tr("Discard this draft?"));
    dialog->setMessage(tr("A dialog is the one surface that interrupts: it is the only place a decision is "
                          "asked for and waited on, which is why there are two answers and no third."));
    dialog->setButtonText(MaterialDialog::Role::Primary, tr("Discard"));
    dialog->setButtonText(MaterialDialog::Role::Secondary, tr("Keep"));
    connect(dialog, &MaterialDialog::buttonClicked, this, [this](MaterialDialog::Role role) {
        Q_EMIT messageRequested(role == MaterialDialog::Role::Primary
                                    ? tr("Discarded, in a demo that had nothing to discard.")
                                    : tr("Kept: the draft is as it was."));
    });
    dialog->open();
}
