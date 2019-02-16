#include <QStyleFactory>

#include "popupwindow.h"
#include "ui_popupwindow.h"
#include <QtnRibbonStyle.h>

using namespace Qtitan;


/* PopupDemoDialog */
PopupDemoDialog::PopupDemoDialog()
{
    m_defaultStyle = QStyleFactory::create(qApp->style()->objectName());
    m_officeStyle = new OfficeStyle();
    m_officeStyle->setDialogsIgnored(true);

    ui.setupUi(this);
    m_dragDropPopup = true;

    m_animationSpeed = 250;
    m_displayTime = 2500;
    m_transparency = 255;
    m_animType = Qtitan::PopupAnimationSlide;
    initDialog();
}

PopupDemoDialog::~PopupDemoDialog()
{
}

static int convertAnimationType(Qtitan::PopupAnimation type)
{   
    int index = 0;
    if (Qtitan::PopupAnimationUnfold == type)
        index = 1;
    else if (Qtitan::PopupAnimationSlide == type)
        index = 2;
    else if (Qtitan::PopupAnimationFade == type)
        index = 3;
    return index;
}

void PopupDemoDialog::initDialog()
{
    setOptions(QVariant(0));

    ui.lookWindow->addItem(tr("System Decoration"), OfficeStyle::PopupSystemDecoration);
    ui.lookWindow->addItem(tr("Compatible Decoration"), OfficeStyle::PopupCompatibleDecoration);
    ui.lookWindow->addItem(tr("Flat Decoration"), OfficeStyle::PopupOfficeFlatDecoration);
    ui.lookWindow->addItem(tr("MSN Messenger Decoration"), OfficeStyle::PopupMSNDecoration);
    ui.lookWindow->addItem(tr("Custom HTML Text Style"), PopupStyleCustom_HTML_Text);
    ui.lookWindow->setCurrentIndex(0);
    connect(ui.lookWindow, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedStyle(int)));

    ui.animationTypeBox->addItem(tr("No animation"), Qtitan::PopupAnimationNone);
    ui.animationTypeBox->addItem(tr("Unfold"),       Qtitan::PopupAnimationUnfold);
    ui.animationTypeBox->addItem(tr("Slide"),        Qtitan::PopupAnimationSlide);
    ui.animationTypeBox->addItem(tr("Fade"),         Qtitan::PopupAnimationFade);
    ui.animationTypeBox->setCurrentIndex(::convertAnimationType(m_animType));

    ui.animationSpeedBox->setValue(m_animationSpeed);
    connect(ui.animationSpeedBox, SIGNAL(valueChanged(int)), this, SLOT(animationSpeed(int)));

    ui.displayTimeBox->setValue(m_displayTime);
    connect(ui.displayTimeBox, SIGNAL(valueChanged(int)), this, SLOT(displayTime(int)));


    QString strValue(tr("Transparency:"));
    strValue += QObject::tr(" %1%").arg(abs((((100*m_transparency)/255))-100));
    ui.transparencyLabel->setText(strValue);
    ui.transparencylSlider->setSliderPosition(m_transparency);
    ui.transparencylSlider->setSingleStep(10);
    connect(ui.transparencylSlider, SIGNAL(valueChanged(int)), this, SLOT(transparencylSlider(int)));

    ui.allowMoveBox->setChecked(m_dragDropPopup);
    connect(ui.allowMoveBox, SIGNAL(clicked(bool)), this, SLOT(allowMove(bool)));

    connect(ui.showPopupButton, SIGNAL(pressed()), this, SLOT(showPopup()));
}

void PopupDemoDialog::transparencylSlider(int value)
{
    m_transparency = value;
    QString strValue(tr("Transparency:"));
    strValue += QObject::tr(" %1%").arg(abs(((100*m_transparency)/255)-100));
    ui.transparencyLabel->setText(strValue);
}

void PopupDemoDialog::currentIndexChangedStyle(int index)
{
    qApp->setStyle(index == OfficeStyle::PopupSystemDecoration ? m_defaultStyle : m_officeStyle);
    setOptions(ui.lookWindow->itemData(index));
}

void PopupDemoDialog::showPopup()
{
    QVariant lookValue = ui.lookWindow->itemData(ui.lookWindow->currentIndex());

    OfficePopupWindow* popup = new OfficePopupWindow(this);
    if (lookValue == OfficeStyle::PopupSystemDecoration)
        fillSystemDecoration(popup);
    else if (lookValue == OfficeStyle::PopupCompatibleDecoration)
        fillOfficeCompatibleDecoration(popup);
    else if (lookValue == OfficeStyle::PopupOfficeFlatDecoration)
        fillOfficeFlatDecoration(popup);
    else if (lookValue == OfficeStyle::PopupMSNDecoration)
        fillMSNDecoration(popup);
    else if (lookValue == PopupStyleCustom_HTML_Text)
        fillCustomHTMLTextDecoration(popup);

    connect(popup, SIGNAL(aboutToShow()), this, SLOT(aboutToShowPopup()));
    connect(popup, SIGNAL(aboutToHide()), this, SLOT(aboutToHidePopup()));

    QVariant value = ui.animationTypeBox->itemData(ui.animationTypeBox->currentIndex());
    popup->setAnimation(static_cast<PopupAnimation>(value.toUInt()));

    popup->setDragDropEnabled(m_dragDropPopup);
    popup->setTransparency(m_transparency/255.0);
    popup->setDisplayTime(m_displayTime);
    popup->setAnimationSpeed(m_animationSpeed);
    popup->setAttribute(Qt::WA_DeleteOnClose);
    popup->showPopup();
}

static inline bool isAncestor(const QObject* child)
{
    static QString className = QString("Qtitan::OfficePopupWindow");
    while (child) 
    {
        if (child->metaObject()->className() == className.toLocal8Bit())
            return true;
        child = child->parent();
    }
    return false;
}

void PopupDemoDialog::setEnabledWidgets(bool enabled)
{
    QList<Qtitan::OfficePopupWindow*> list = findChildren<Qtitan::OfficePopupWindow*>();
    if (list.count() == 1)
    {
        QList<QWidget*> list = findChildren<QWidget*>();
        for (int i = 0, count = list.count(); count > i; i++)
        {
            QWidget* child = list[i];
            if (!::isAncestor(child) && child != ui.showPopupButton)
                child->setEnabled(enabled);
        }
    }
}

void PopupDemoDialog::fillSystemDecoration(OfficePopupWindow* popup)
{
    QString showText;

    showText = QObject::tr(
        "<p><a href=\"http://www.devmachines.com/\">Click here to learn more about this<br />"
        "update, and how you can download our<br />"
        "latest product version.</a> "
        "<p><img src=\":/res/devmachines.png\" /></p>"
        );
    popup->setBodyText(showText);

    popup->setTitleText(tr("<p><font color=\"#ffffff\"><h4>Developer Machines Update Available</h4></font></p>"));
    popup->setTitleCloseButtonVisible();

    if (QLabel* label = qobject_cast<QLabel*>(popup->centralWidget()))
    {
        label->setOpenExternalLinks(true);
        label->setAlignment(Qt::AlignCenter);
    }
}

void PopupDemoDialog::fillOfficeCompatibleDecoration(Qtitan::OfficePopupWindow* popup)
{
    const int controlWidth = 190;

    QWidget* widget = new QWidget(popup);
    widget->resize(QSize(controlWidth, 100));

    QLabel* logoOffice = new QLabel(widget);
    logoOffice->setGeometry(QRect(14, 10, 25, 25));
    logoOffice->setPixmap(QPixmap(QString::fromUtf8(":/res/logooffice.bmp")));

    QPushButton* optionButton = new QPushButton(widget);
    optionButton->setText(tr("Option"));
    QSize sz = optionButton->sizeHint();
    optionButton->move(controlWidth - sz.width() - 10, 5);

    popup->setCentralWidget(widget);

    popup->setTitleIcon(QIcon(QString::fromUtf8(":/res/word2000.ico")));
    popup->setTitleText(tr("<p><font color=\"#ffffff\">Word 2000 Style</font></p>"));
    popup->setTitleCloseButtonVisible();

    if (Qtitan::OfficeStyle* ribbonStyle = qobject_cast<Qtitan::OfficeStyle*>(popup->style()))
        ribbonStyle->setPopupDecoration(OfficeStyle::PopupCompatibleDecoration);
}

void PopupDemoDialog::fillOfficeFlatDecoration(Qtitan::OfficePopupWindow* popup)
{
    QWidget* widget = new QWidget(popup);
    Ui::Form uiForm;
    uiForm.setupUi(widget);
    popup->setCentralWidget(widget);
    connect(uiForm.closeButton, SIGNAL(released()), popup, SLOT(closePopup()));
    if (Qtitan::OfficeStyle* ribbonStyle = qobject_cast<Qtitan::OfficeStyle*>(popup->style()))
        ribbonStyle->setPopupDecoration(OfficeStyle::PopupOfficeFlatDecoration);
}

/*
//void PopupDemoDialog::fillOffice2007Style(Qtitan::OfficePopupWindow* popup)
{
    popup->setTitleIcon(QIcon(":/res/letter.ico"));
    popup->setTitleText("<span style=\"font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:600; font-style:normal;\">John Smith</span><br>Re: Your request");
    popup->setBodyText("<a href='#'><img src=':/res/cross.ico'></a> \
                      <a href='#'><img src=':/res/flag.ico'></a> \
                      <span style=\"color:#0000ff; font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:400; font-style:normal;\"> \
                      I am sending you the requested inform&nbsp;&nbsp;&nbsp;</span>");
    popup->setTitleCloseButtonVisible();
    if (Qtitan::OfficeStyle* ribbonStyle = qobject_cast<Qtitan::OfficeStyle*>(popup->style()))
        ribbonStyle->setPopupDecoration(OfficeStyle::PopupSystemDecoration);
}

*/
void PopupDemoDialog::fillMSNDecoration(Qtitan::OfficePopupWindow* popup)
{
    const int controlMargin = 10;

    QWidget* widget = new QWidget(popup);
    popup->setCentralWidget(widget);

    QString showText;
    showText = QObject::tr(
        "<p><a href=\"http://www.devmachines.com/\">Please visit our website for<br />"
        "additional product<br />"
        "information including<br />"
        "registration details.</a> "
        );

    QLabel* textVisit = new QLabel(showText, widget);
    textVisit->setOpenExternalLinks(true);
    textVisit->setAlignment(Qt::AlignCenter);
    textVisit->move(controlMargin, 0);

    QSize sz = textVisit->sizeHint();

    QLabel* logoMSN = new QLabel(widget);
    logoMSN->setPixmap(QPixmap(QString::fromUtf8(":/res/logoMSN.png")));
    QSize szLogo = logoMSN->sizeHint();
    logoMSN->move(sz.width() - szLogo.width() + controlMargin, sz.height()+5);

    popup->setTitleIcon(QIcon(QString::fromUtf8(":/res/titleMSN.png")));
    popup->setTitleText(tr("Popup Window Demo"));
    popup->setTitleCloseButtonVisible();
    popup->setCloseButtonPixmap(QPixmap(QString::fromUtf8(":/res/closeMSN.png")));

    widget->resize(textVisit->sizeHint() + QSize(20, szLogo.height() + controlMargin));
    if (Qtitan::OfficeStyle* ribbonStyle = qobject_cast<Qtitan::OfficeStyle*>(popup->style()))
        ribbonStyle->setPopupDecoration(OfficeStyle::PopupMSNDecoration);
}

void PopupDemoDialog::fillCustomHTMLTextDecoration(Qtitan::OfficePopupWindow* popup)
{
    QString showText;

    showText = QObject::tr(
        "The Popup Window sample demonstrates how to create a popup window<br />"
        "similar to Outlook and MSN Messenger.<br />"
        "The popup control provides several built in themes that are ready to use.<br />"
        "You can choose from one of the predefined themes or create your own.<br />"
        "The animation, animation delay, show delay, and transparency or the popup<br />"
        "window can all be customized."
        );

    popup->setTitleText("<p><font color=\"#000064\"><h4>OfficePopupWindow Sample</h4></font></p>");
    popup->setBodyText(showText);
    popup->setTitleCloseButtonVisible();
    if (Qtitan::OfficeStyle* ribbonStyle = qobject_cast<Qtitan::OfficeStyle*>(popup->style()))
        ribbonStyle->setPopupDecoration(OfficeStyle::PopupOfficeFlatDecoration);
}

void PopupDemoDialog::displayTime(int value)
{
    m_displayTime = value;
}

void PopupDemoDialog::animationSpeed(int value)
{
    m_animationSpeed = value;
}

void PopupDemoDialog::allowMove(bool checked)
{
    m_dragDropPopup = checked;
}

void PopupDemoDialog::aboutToShowPopup()
{
    setEnabledWidgets(false);
}

void PopupDemoDialog::aboutToHidePopup()
{
    setEnabledWidgets(true);
}

void PopupDemoDialog::setOptions(QVariant lookValue)
{
    if (lookValue == OfficeStyle::PopupSystemDecoration)
    {
        m_animationSpeed = 250;
        m_displayTime = 2500;
        m_transparency = 255;
        m_animType = Qtitan::PopupAnimationSlide;
    }
    else if (lookValue == OfficeStyle::PopupCompatibleDecoration)
    {
        m_animationSpeed = 500;
        m_displayTime = 3000;
        m_transparency = 255;
        m_animType = PopupAnimationFade;
    }
    else if (lookValue == OfficeStyle::PopupOfficeFlatDecoration)
    {
        m_animationSpeed = 500;
        m_displayTime = 3000;
        m_transparency = 200;
        m_animType = Qtitan::PopupAnimationFade;
    }
    else if (lookValue == OfficeStyle::PopupMSNDecoration)
    {
        m_animationSpeed = 350;
        m_displayTime = 3000;
        m_transparency = 255;
        m_animType = PopupAnimationSlide;
    }
    else if (lookValue == PopupStyleCustom_HTML_Text)
    {
        m_animationSpeed = 250;
        m_displayTime = 7500;
        m_transparency = 255;
        m_animType = Qtitan::PopupAnimationSlide;
    }

    ui.animationTypeBox->setCurrentIndex(::convertAnimationType(m_animType));

    ui.animationSpeedBox->setValue(m_animationSpeed);
    ui.displayTimeBox->setValue(m_displayTime);

    QString strValue(tr("Transparency:"));
    strValue += QObject::tr(" %1%").arg(abs((((100*m_transparency)/255))-100));
    ui.transparencyLabel->setText(strValue);
    ui.transparencylSlider->setSliderPosition(m_transparency);
}

