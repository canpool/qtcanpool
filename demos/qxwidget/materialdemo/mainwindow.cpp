#include "mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>

#include "qxmaterial/materialappbar.h"
#include "qxmaterial/materialcheckbox.h"
#include "qxmaterial/materialradiobutton.h"
#include "qxmaterial/materialautocomplete.h"
#include "qxmaterial/materialtextfield.h"
#include "qxmaterial/materialscrollbar.h"
#include "qxmaterial/materialavatar.h"
#include "qxmaterial/materialbadge.h"
#include "qxmaterial/materialslider.h"
#include "qxmaterial/materialprogress.h"
#include "qxmaterial/materialflatbutton.h"
#include "qxmaterial/materialraisedbutton.h"
#include "qxmaterial/materialfab.h"
#include "qxmaterial/materialcircularprogress.h"
#include "qxmaterial/materialtheme.h"
#include "qxmaterial/materialiconbutton.h"
#include "qxmaterial/materialtoggle.h"
#include "qxmaterial/materialtabbar.h"

QX_MATERIAL_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *cw = new QWidget(this);
    setCentralWidget(cw);

    QVBoxLayout *mainLayout = new QVBoxLayout(cw);

    // appbar
    MaterialAppBar *appBar = new MaterialAppBar();
    const QColor &foregroundColor = appBar->foregroundColor();

    QLabel *label = new QLabel("Inbox");
    label->setAttribute(Qt::WA_TranslucentBackground);
    label->setForegroundRole(QPalette::WindowText);
    label->setContentsMargins(6, 0, 0, 0);

    QPalette palette = label->palette();
    palette.setColor(label->foregroundRole(), foregroundColor);
    label->setPalette(palette);

    label->setFont(QFont("Roboto", 18, QFont::Normal));

    MaterialIconButton *button = new MaterialIconButton(MaterialTheme::icon("navigation", "menu"));
    button->setIconSize(QSize(24, 24));
    button->setColor(foregroundColor);
    button->setFixedWidth(42);

    QHBoxLayout *appBarLayout = appBar->appBarLayout();
    appBarLayout->addWidget(button);
    appBarLayout->addWidget(label);
    appBarLayout->addStretch(1);

    mainLayout->addWidget(appBar);

    // new mid
    QHBoxLayout *midHLayout = new QHBoxLayout();
    midHLayout->setSpacing(5);
    midHLayout->setContentsMargins(0, 0, 0, 0);

    // HBoxLayout, VBoxLayout
    QHBoxLayout *hLayout = nullptr;
    QVBoxLayout *vLayout = nullptr;

    // checkbox
    MaterialCheckBox *cb1 = new MaterialCheckBox();
    cb1->setText("Option 1");
    MaterialCheckBox *cb2 = new MaterialCheckBox();
    cb2->setText("Option 2");
    MaterialToggle *toggle = new MaterialToggle();
    vLayout = new QVBoxLayout();
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->addWidget(cb1);
    vLayout->addWidget(cb2);
    vLayout->addWidget(toggle);
    midHLayout->addLayout(vLayout);

    // radiobutton
    MaterialRadioButton *rb1 = new MaterialRadioButton();
    rb1->setText("Option 1");
    MaterialRadioButton *rb2 = new MaterialRadioButton();
    rb2->setText("Option 2");
    MaterialRadioButton *rb3 = new MaterialRadioButton();
    rb3->setText("Option 3");
    vLayout = new QVBoxLayout();
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->addWidget(rb1);
    vLayout->addWidget(rb2);
    vLayout->addWidget(rb3);
    midHLayout->addLayout(vLayout);

    vLayout = new QVBoxLayout();
    vLayout->setSpacing(10);
    vLayout->setContentsMargins(0, 0, 0, 0);

    hLayout = new QHBoxLayout();
    hLayout->setSpacing(10);
    hLayout->setContentsMargins(0, 10, 0, 10);
    hLayout->addStretch();
    MaterialIconButton *iconButton = nullptr;
    iconButton = new MaterialIconButton(MaterialTheme::icon("toggle", "star"));
    hLayout->addWidget(iconButton);
    iconButton = new MaterialIconButton(MaterialTheme::icon("communication", "call"));
    hLayout->addWidget(iconButton);
    iconButton = new MaterialIconButton(MaterialTheme::icon("action", "alarm"));
    hLayout->addWidget(iconButton);
    iconButton = new MaterialIconButton(MaterialTheme::icon("action", "tab"));
    hLayout->addWidget(iconButton);
    hLayout->addStretch();
    vLayout->addLayout(hLayout);

    MaterialFlatButton *fb = new MaterialFlatButton(tr("I'M FLAT"));
    vLayout->addWidget(fb);
    MaterialRaisedButton *raisedBtn = new MaterialRaisedButton(tr("RISE UP"));
    vLayout->addWidget(raisedBtn);

    midHLayout->addLayout(vLayout);

    // autocomplete
    QStringList states =
      { "Alabama"
      , "Alaska"
      , "American Samoa"
      , "Arizona"
      , "Arkansas"
      , "California"
      , "Colorado"
      , "Connecticut"
      , "Delaware"
      , "District of Columbia"
      , "Florida"
      , "Georgia"
      , "Guam"
      , "Hawaii"
      , "Idaho"
      , "Illinois"
      , "Indiana"
      , "Iowa"
      , "Kansas"
      , "Kentucky"
      , "Louisiana"
      , "Maine"
      , "Maryland"
      , "Massachusetts"
      , "Michigan"
      , "Minnesota"
      , "Mississippi"
      , "Missouri"
      , "Montana"
      , "Nebraska"
      , "Nevada"
      , "New Hampshire"
      , "New Jersey"
      , "New Mexico"
      , "New York"
      , "North Carolina"
      , "North Dakota"
      , "Northern Marianas Islands"
      , "Ohio"
      , "Oklahoma"
      , "Oregon"
      , "Pennsylvania"
      , "Puerto Rico"
      , "Rhode Island"
      , "South Carolina"
      , "South Dakota"
      , "Tennessee"
      , "Texas"
      , "Utah"
      , "Vermont"
      , "Virginia"
      , "Virgin Islands"
      , "Washington"
      , "West Virginia"
      , "Wisconsin"
      , "Wyoming"
    };
    MaterialAutoComplete *autocomplete = new MaterialAutoComplete();
    autocomplete->setDataSource(states);
    // textfield
    MaterialTextField *textField = new MaterialTextField();
    textField->setLabel("Wat is this");
    MaterialSlider *slider = new MaterialSlider();
    MaterialProgress *progress = new MaterialProgress();
    vLayout = new QVBoxLayout();
    vLayout->setSpacing(5);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->addWidget(slider);
    vLayout->addWidget(progress);
    vLayout->addWidget(textField);
    vLayout->addWidget(autocomplete);
    midHLayout->addLayout(vLayout);

    QTextEdit *edit = new QTextEdit;
    edit->setText("<p>The distinction between the subjects of syntax and semantics has its origin in the study of natural languages.</p><p>The distinction between the subjects of syntax and semantics has its origin in the study of natural languages.</p><p>The distinction between the subjects of syntax and semantics has its origin in the study of natural languages.</p><p>The distinction between the subjects of syntax and semantics has its origin in the study of natural languages.</p><p>The distinction between the subjects of syntax and semantics has its origin in the study of natural languages.</p><p>The distinction between the subjects of syntax and semantics has its origin in the study of natural languages.</p><p>The distinction between the subjects of syntax and semantics has its origin in the study of natural languages.</p><p>The distinction between the subjects of syntax and semantics has its origin in the study of natural languages.</p>");
    edit->setLineWrapMode(QTextEdit::NoWrap);
    edit->update();
    edit->setMaximumHeight(150);
    MaterialScrollBar *verticalScrollbar = new MaterialScrollBar();
    MaterialScrollBar *horizontalScrollbar = new MaterialScrollBar();
    edit->setVerticalScrollBar(verticalScrollbar);
    edit->setHorizontalScrollBar(horizontalScrollbar);
    horizontalScrollbar->setOrientation(Qt::Horizontal);
    vLayout = new QVBoxLayout();
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->addWidget(edit);
    midHLayout->addLayout(vLayout);

    midHLayout->addStretch();

    mainLayout->addLayout(midHLayout);

    // new mid
    midHLayout = new QHBoxLayout();
    midHLayout->setSpacing(5);
    midHLayout->setContentsMargins(0, 0, 0, 0);

    hLayout = new QHBoxLayout();
    hLayout->setSpacing(10);
    hLayout->setContentsMargins(0, 10, 0, 10);
    MaterialAvatar *avatar = new MaterialAvatar(QChar('X'));
    hLayout->addWidget(avatar);
    avatar = new MaterialAvatar(MaterialTheme::icon("communication", "message"));
    avatar->setSize(60);
    MaterialBadge *badge = new MaterialBadge();
    badge->setParent(avatar);
    badge->setRelativePosition(15, 15);
    badge->setText("3");
    hLayout->addWidget(avatar);
    MaterialCircularProgress *circularProgress = new MaterialCircularProgress();
    hLayout->addWidget(circularProgress);
    MaterialTabBar *tabBar = new MaterialTabBar();
    tabBar->addTab("Media");
    tabBar->addTab("Playback");
    tabBar->addTab("Audio");
    tabBar->addTab("Video");
    tabBar->addTab("Tools");
    tabBar->setMinimumWidth(400);
    hLayout->addWidget(tabBar);
    MaterialFloatingActionButton *fab = new MaterialFloatingActionButton(MaterialTheme::icon("content", "add"));
    hLayout->addWidget(fab);
    hLayout->addStretch();
    midHLayout->addLayout(hLayout);

    midHLayout->addStretch(1);

    mainLayout->addLayout(midHLayout);

    mainLayout->addStretch(1);

    setWindowTitle(tr("Material Demo"));
    resize(800, 400);
}

MainWindow::~MainWindow()
{
}

