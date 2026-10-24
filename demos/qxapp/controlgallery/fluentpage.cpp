/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentpage.h"
#include "fluentmaterialwindow.h"

#include "qxfluent/fluentautosuggestbox.h"
#include "qxfluent/fluentavatar.h"
#include "qxfluent/fluentbadge.h"
#include "qxfluent/fluentbreadcrumbbar.h"
#include "qxfluent/fluentbutton.h"
#include "qxfluent/fluentcalendarview.h"
#include "qxfluent/fluentcard.h"
#include "qxfluent/fluentcheckbox.h"
#include "qxfluent/fluentcombobox.h"
#include "qxfluent/fluentcommandbar.h"
#include "qxfluent/fluentcontentdialog.h"
#include "qxfluent/fluentdatepicker.h"
#include "qxfluent/fluentdivider.h"
#include "qxfluent/fluentdropdownbutton.h"
#include "qxfluent/fluentexpander.h"
#include "qxfluent/fluentgridview.h"
#include "qxfluent/fluentinfobar.h"
#include "qxfluent/fluentlistview.h"
#include "qxfluent/fluentmenuflyout.h"
#include "qxfluent/fluentnumberbox.h"
#include "qxfluent/fluentpasswordbox.h"
#include "qxfluent/fluentprogressbar.h"
#include "qxfluent/fluentprogressring.h"
#include "qxfluent/fluentradiobutton.h"
#include "qxfluent/fluentrating.h"
#include "qxfluent/fluentreveal.h"
#include "qxfluent/fluentslider.h"
#include "qxfluent/fluentswitch.h"
#include "qxfluent/fluentteachingtip.h"
#include "qxfluent/fluenttextbox.h"
#include "qxfluent/fluenttimepicker.h"
#include "qxfluent/fluenttogglesplitbutton.h"
#include "qxfluent/fluenttokens.h"
#include "qxfluent/fluenttooltip.h"
#include "qxfluent/fluenttreeview.h"
#include "qxfluent/fluentwindowmaterial.h"

#include <QtGui/QPainter>
#include <QComboBox>
#include <QDate>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTime>
#include <QVBoxLayout>

QX_FLUENT_USE_NAMESPACE

namespace
{
/*!
 * A surface for a FluentReveal to sit on.
 *
 * A reveal is an effect and not a widget: it listens to whatever it was
 * attached to and is painted by that thing, so a gallery made of controls has
 * nowhere to put one - every control paints its own wash already. This is that
 * place, and it is the smallest thing that can hold one: a rounded surface and
 * nothing else.
 */
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
}   // namespace

FluentPage::FluentPage(QWidget *parent)
    : GalleryPage(QStringLiteral("fluent"), parent)
{
    addSection(tr("Input"));

    auto *button = addControl<FluentButton>(QStringLiteral("fluentButton"), QStringLiteral("FluentButton"),
                                            new FluentButton(this));
    button->setText(tr("Accent"));
    button->setKind(FluentButton::Kind::Accent);

    auto *check = addControl<FluentCheckBox>(QStringLiteral("fluentCheckBox"), QStringLiteral("FluentCheckBox"),
                                             new FluentCheckBox(this));
    check->setText(tr("Keep me posted"));
    check->setChecked(true);

    // Two radios side by side: a choice of one among others is the whole of what
    // a radio says, and one alone in a row says nothing.
    auto *radioHost = new QWidget(this);
    auto *radioLayout = new QHBoxLayout(radioHost);
    radioLayout->setContentsMargins(0, 0, 0, 0);
    radioLayout->setSpacing(24);
    auto *everyDay = new FluentRadioButton(radioHost);
    everyDay->setText(tr("Day"));
    everyDay->setChecked(true);
    auto *everyWeek = new FluentRadioButton(radioHost);
    everyWeek->setText(tr("Week"));
    radioLayout->addWidget(everyDay);
    radioLayout->addWidget(everyWeek);
    addControl<QWidget>(QStringLiteral("fluentRadioButton"), QStringLiteral("FluentRadioButton"), radioHost);

    auto *combo = addControl<FluentComboBox>(QStringLiteral("fluentComboBox"), QStringLiteral("FluentComboBox"),
                                             new FluentComboBox(this));
    combo->setItems(QStringList{tr("Day"), tr("Week"), tr("Month")});
    combo->setCurrentIndex(1);

    auto *box = addControl<FluentTextBox>(QStringLiteral("fluentTextBox"), QStringLiteral("FluentTextBox"),
                                          new FluentTextBox(this));
    box->setPlaceholderText(tr("Search the gallery"));

    auto *password = addControl<FluentPasswordBox>(QStringLiteral("fluentPasswordBox"),
                                                   QStringLiteral("FluentPasswordBox"), new FluentPasswordBox(this));
    password->setText(QStringLiteral("correct horse"));

    auto *suggest = addControl<FluentAutoSuggestBox>(
        QStringLiteral("fluentAutoSuggestBox"), QStringLiteral("FluentAutoSuggestBox"), new FluentAutoSuggestBox(this));
    suggest->setPlaceholderText(tr("Type a fruit"));
    suggest->setSuggestions(QStringList{tr("Apple"), tr("Apricot"), tr("Banana")});

    auto *dayField = addControl<FluentDatePicker>(QStringLiteral("fluentDatePicker"),
                                                  QStringLiteral("FluentDatePicker"), new FluentDatePicker(this));
    dayField->setDate(QDate(2026, 10, 24));

    auto *timeField = addControl<FluentTimePicker>(QStringLiteral("fluentTimePicker"),
                                                   QStringLiteral("FluentTimePicker"), new FluentTimePicker(this));
    timeField->setTime(QTime(9, 30));

    auto *onOff = addControl<FluentSwitch>(QStringLiteral("fluentSwitch"), QStringLiteral("FluentSwitch"),
                                           new FluentSwitch(this));
    onOff->setText(tr("Wi-Fi"));
    onOff->setOn(true);

    auto *slider = addControl<FluentSlider>(QStringLiteral("fluentSlider"), QStringLiteral("FluentSlider"),
                                            new FluentSlider(this));
    slider->setRange(0, 100);
    slider->setTickInterval(25);
    slider->setValue(40);

    auto *number = addControl<FluentNumberBox>(QStringLiteral("fluentNumberBox"), QStringLiteral("FluentNumberBox"),
                                               new FluentNumberBox(this));
    number->setRange(0, 999);
    number->setValue(42);

    auto *exportButton = addControl<FluentDropDownButton>(
        QStringLiteral("fluentDropDownButton"), QStringLiteral("FluentDropDownButton"), new FluentDropDownButton(this));
    exportButton->setText(tr("Export"));
    exportButton->menu()->addAction(tr("As a PDF"));
    exportButton->menu()->addAction(tr("As an image"));

    auto *pinButton = addControl<FluentToggleSplitButton>(QStringLiteral("fluentToggleSplitButton"),
                                                          QStringLiteral("FluentToggleSplitButton"),
                                                          new FluentToggleSplitButton(this));
    pinButton->setText(tr("Pin"));
    pinButton->setChecked(true);
    pinButton->menu()->addAction(tr("Pin to the left"));
    pinButton->menu()->addAction(tr("Unpin all"));

    auto *rating = addControl<FluentRating>(QStringLiteral("fluentRating"), QStringLiteral("FluentRating"),
                                            new FluentRating(this));
    rating->setValue(3);

    addSection(tr("Display"));

    auto *badge =
        addControl<FluentBadge>(QStringLiteral("fluentBadge"), QStringLiteral("FluentBadge"), new FluentBadge(this));
    badge->setCount(12);
    badge->setMaximumCount(99);

    auto *card =
        addControl<FluentCard>(QStringLiteral("fluentCard"), QStringLiteral("FluentCard"), new FluentCard(this));
    card->setElevation(FluentElevation::Level::Depth8);
    card->setFixedSize(240, 96);
    // A card is a surface, and a surface with nothing on it is a shape rather
    // than an answer - so it carries one line of text, inset by the margin the
    // shadow is drawn in, which is the only part of the rectangle the card's
    // contents may use.
    auto *cardLayout = new QVBoxLayout(card);
    const int cardInset = FluentCard::shadowMargin(FluentElevation::Level::Depth8);
    cardLayout->setContentsMargins(cardInset, cardInset, cardInset, cardInset);
    cardLayout->addWidget(new QLabel(tr("A card lifts what is on it."), card));

    auto *divider = addControl<FluentDivider>(QStringLiteral("fluentDivider"), QStringLiteral("FluentDivider"),
                                              new FluentDivider(this));
    divider->setFixedWidth(240);

    auto *avatar = addControl<FluentAvatar>(QStringLiteral("fluentAvatar"), QStringLiteral("FluentAvatar"),
                                            new FluentAvatar(this));
    avatar->setName(tr("Ada Lovelace"));

    // The three views of a collection, one row each and the same shape of room:
    // what tells them apart is the shape of what they show, not the data in it.
    auto *list = addControl<FluentListView>(QStringLiteral("fluentListView"), QStringLiteral("FluentListView"),
                                            new FluentListView(this));
    list->setItems(QStringList{tr("Album"), tr("Artist"), tr("Genre")});
    list->setFixedSize(240, 132);

    auto *grid = addControl<FluentGridView>(QStringLiteral("fluentGridView"), QStringLiteral("FluentGridView"),
                                            new FluentGridView(this));
    grid->setItems(QStringList{tr("One"), tr("Two"), tr("Three"), tr("Four")});
    grid->setFixedSize(240, 132);

    auto *tree = addControl<FluentTreeView>(QStringLiteral("fluentTreeView"), QStringLiteral("FluentTreeView"),
                                            new FluentTreeView(this));
    QStandardItem *library = tree->addNode(tr("Library"));
    tree->addChild(library, tr("Album"));
    tree->addChild(library, tr("Single"));
    tree->expandAll();
    tree->setFixedSize(240, 132);

    auto *calendar = addControl<FluentCalendarView>(QStringLiteral("fluentCalendarView"),
                                                    QStringLiteral("FluentCalendarView"), new FluentCalendarView(this));
    calendar->setSelectedDate(QDate(2026, 10, 24));
    calendar->setFixedSize(320, 280);

    addSection(tr("Feedback"));

    auto *ring = addControl<FluentProgressRing>(QStringLiteral("fluentProgressRing"),
                                                QStringLiteral("FluentProgressRing"), new FluentProgressRing(this));
    ring->setValue(65);

    auto *info = addControl<FluentInfoBar>(QStringLiteral("fluentInfoBar"), QStringLiteral("FluentInfoBar"),
                                           new FluentInfoBar(this));
    info->setSeverity(FluentInfoBar::Severity::Success);
    info->setTitle(tr("Saved"));
    info->setMessage(tr("The gallery wrote nothing to disk."));
    info->setOpen(true);

    auto *progress = addControl<FluentProgressBar>(QStringLiteral("fluentProgressBar"),
                                                   QStringLiteral("FluentProgressBar"), new FluentProgressBar(this));
    progress->setMinimum(0);
    progress->setMaximum(100);
    progress->setValue(60);

    // A tooltip is a window that arrives over the page, so the row is the thing
    // it waits beside: hovering the button shows it, and a capture of the page
    // cannot hold a second window - the same bargain the dialog row makes.
    auto *tipAnchor = addControl<QPushButton>(QStringLiteral("fluentToolTip"), QStringLiteral("FluentToolTip"),
                                              new QPushButton(tr("Hover me"), this));
    auto *tooltip = new FluentToolTip(this);
    tooltip->setAnchor(tipAnchor);
    tooltip->setText(tr("A tooltip says a word about the control it waits beside."));

    // A dialog is a window that covers the page it was opened over, so the row
    // is what opens it rather than the dialog - a window capture cannot hold
    // two, which is the same bargain the teaching tip row makes.
    auto *dialogButton =
        addControl<QPushButton>(QStringLiteral("fluentContentDialogLauncher"), QStringLiteral("FluentContentDialog"),
                                new QPushButton(tr("Ask me something"), this));
    connect(dialogButton, &QPushButton::clicked, this, [this]() {
        if (!m_dialog) {
            m_dialog = new FluentContentDialog(this);
            m_dialog->setTitle(tr("Save before closing?"));
            m_dialog->setMessage(tr("The document has changes that are not on disk yet."));
            m_dialog->primaryButton()->setText(tr("Save"));
            m_dialog->secondaryButton()->setText(tr("Do not save"));
            m_dialog->closeButton()->setText(tr("Cancel"));
            // The dialog asks and the page answers: a button only says which
            // answer it was, so these three connections are the whole of what
            // pressing one does here.
            connect(m_dialog->primaryButton(), &QPushButton::clicked, m_dialog, &QDialog::accept);
            connect(m_dialog->secondaryButton(), &QPushButton::clicked, m_dialog, &QDialog::reject);
            connect(m_dialog->closeButton(), &QPushButton::clicked, m_dialog, &QDialog::reject);
        }
        m_dialog->open();
    });

    addSection(tr("Navigation"));

    auto *expander = addControl<FluentExpander>(QStringLiteral("fluentExpander"), QStringLiteral("FluentExpander"),
                                                new FluentExpander(this));
    expander->setTitle(tr("What an expander hides"));
    auto *body = new QLabel(tr("A body the header is the summary of."));
    body->setMinimumHeight(48);
    expander->setContent(body);
    expander->setExpanded(true);

    auto *crumbs = addControl<FluentBreadcrumbBar>(
        QStringLiteral("fluentBreadcrumbBar"), QStringLiteral("FluentBreadcrumbBar"), new FluentBreadcrumbBar(this));
    crumbs->setItems(QStringList{tr("Home"), tr("Library"), tr("Album")});

    auto *commands = addControl<FluentCommandBar>(QStringLiteral("fluentCommandBar"),
                                                  QStringLiteral("FluentCommandBar"), new FluentCommandBar(this));
    commands->addCommand(FluentCommandBar::ItemRole::Button, tr("Save"));
    commands->addCommand(FluentCommandBar::ItemRole::Toggle, tr("Pin"));
    QAction *sort = commands->addCommand(FluentCommandBar::ItemRole::DropDown, tr("Sort"));
    auto *sortMenu = new QMenu(commands);
    sortMenu->addAction(tr("By name"));
    sortMenu->addAction(tr("By date"));
    sort->setMenu(sortMenu);

    // A flyout is a surface of its own, like the dialog and the teaching tip: the
    // row is the button it opens from.
    auto *menuButton = addControl<QPushButton>(QStringLiteral("fluentMenuFlyout"), QStringLiteral("FluentMenuFlyout"),
                                               new QPushButton(tr("Open the menu"), this));
    auto *flyout = new FluentMenuFlyout(menuButton);
    flyout->addAction(tr("Rename"));
    flyout->addCheckable(tr("Show hidden"), true);
    flyout->addSeparator();
    flyout->addAction(tr("Delete"));
    connect(menuButton, &QPushButton::clicked, flyout, &FluentMenuFlyout::open);

    addSection(tr("Effects and transients"));

    addControl<RevealSurface>(QStringLiteral("fluentReveal"), QStringLiteral("FluentReveal"), new RevealSurface(this));

    // A teaching tip is a popup of its own, so the row is what opens it rather
    // than the tip: a tip that was open would be a second top-level window, and
    // a window capture cannot hold two.
    auto *tipButton =
        addControl<QPushButton>(QStringLiteral("fluentTeachingTipLauncher"), QStringLiteral("FluentTeachingTip"),
                                new QPushButton(tr("Point at me"), this));
    connect(tipButton, &QPushButton::clicked, this, [this, tipButton]() {
        if (!m_tip) {
            m_tip = new FluentTeachingTip(this);
            m_tip->setTitle(tr("A teaching tip"));
            m_tip->setMessage(tr("It points at the thing it is about, and says what that thing is."));
        }
        m_tip->setTarget(tipButton);
        m_tip->setPreferredSide(FluentTeachingTip::Side::Bottom);
        m_tip->setOpen(true);
    });

    // A window material is not a control and has no rectangle of its own: it is
    // a choice about the window everything else sits in, asked of qxwindow. And
    // the window it is asked of has to draw a caption of its own, because the
    // asking takes the system frame away - so this row opens a window for the
    // material to be shown on instead of dressing the gallery, whose title bar
    // is the system's and would simply be gone. The row is the switch that asks,
    // and the label says what the platform answered - which on anything other
    // than Windows is "no", and that is worth showing rather than hiding.
    auto *materialRow = new QWidget(this);
    auto *materialLayout = new QHBoxLayout(materialRow);
    materialLayout->setContentsMargins(0, 0, 0, 0);
    auto *materialBox = new QComboBox(materialRow);
    materialBox->addItem(tr("None"), int(FluentWindowMaterial::Material::None));
    materialBox->addItem(tr("Mica"), int(FluentWindowMaterial::Material::Mica));
    materialBox->addItem(tr("Mica Alt"), int(FluentWindowMaterial::Material::MicaAlt));
    materialBox->addItem(tr("Acrylic"), int(FluentWindowMaterial::Material::Acrylic));
    auto *materialState = new QLabel(tr("no window yet"), materialRow);
    materialLayout->addWidget(materialBox);
    materialLayout->addWidget(materialState);
    addControl<QWidget>(QStringLiteral("fluentWindowMaterial"), QStringLiteral("FluentWindowMaterial"), materialRow);

    connect(
        materialBox, QOverload<int>::of(&QComboBox::activated), this, [this, materialBox, materialState](int index) {
            if (!m_materialWindow) {
                // Made on the first ask and not in the constructor, for the
                // same reason the tip and the dialog are: a material is
                // asked of a window, and a window that exists before it is
                // wanted is one more top-level window the capture has to
                // explain.
                m_materialWindow = new FluentMaterialWindow(this);
                m_windowMaterial = new FluentWindowMaterial(m_materialWindow);
            }
            const auto wanted = static_cast<FluentWindowMaterial::Material>(materialBox->itemData(index).toInt());
            const bool took = m_windowMaterial->apply(wanted);
            if (wanted == FluentWindowMaterial::Material::None) {
                m_materialWindow->setMaterialText(tr("No material is asked for: the window is the application's own."));
            } else if (took) {
                m_materialWindow->setMaterialText(
                    tr("%1 is what this window is made of.").arg(materialBox->itemText(index)));
            } else {
                m_materialWindow->setMaterialText(
                    tr("%1 is not something this platform gives.").arg(materialBox->itemText(index)));
            }
            materialState->setText(took ? tr("applied") : tr("not applied"));
            m_materialWindow->show();
            m_materialWindow->raise();
            m_materialWindow->activateWindow();
        });
}
