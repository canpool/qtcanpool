#include "ribbonpopularpage.h"
#include <QColorDialog>
#include <QPainter>

using namespace QRibbon;

/* RibbonPopularPage */
PreviewColor::PreviewColor(QWidget *parent) : QWidget(parent) { resize(parent->size()); }

PreviewColor::~PreviewColor() {}

const QColor &PreviewColor::color() const { return m_color; }

void PreviewColor::setColor(const QColor &color)
{
    if (color != m_color) {
        m_color = color;
        repaint();
    }
}

void PreviewColor::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOption opt;
    opt.init(this);
    p.fillRect(opt.rect.adjusted(2, 2, -2, -2), QBrush(m_color));
}

/* RibbonPopularPage */
RibbonPopularPage::RibbonPopularPage(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent), m_mainWindow(mainWindow), m_wasDisplayed(false)
{
    m_pageRes.setupUi(this);

    m_pageRes.labelTitle->setAutoFillBackground(true);
    QPalette pal = m_pageRes.labelTitle->palette();
    pal.setColor(QPalette::Background, QColor(238, 238, 238));
    m_pageRes.labelTitle->setPalette(pal);

    QRibbon::RibbonStyle *ribbonStyle = qobject_cast<QRibbon::RibbonStyle *>(qApp->style());
    m_currentThemeId = ribbonStyle->getTheme();

    m_pageRes.comboBoxTheme->addItem("Office 2013 White", OfficeStyle::Office2013White);
    m_pageRes.comboBoxTheme->addItem("Office 2013 Light Gray", OfficeStyle::Office2013Gray);
    m_pageRes.comboBoxTheme->addItem("Office 2013 Dark Gray", OfficeStyle::Office2013Dark);
    m_pageRes.comboBoxTheme->setCurrentIndex(m_currentThemeId);
    connect(m_pageRes.comboBoxTheme, SIGNAL(currentIndexChanged(int)), this, SLOT(currentThemeChanged(int)));

    m_accentColor = ribbonStyle->accentColor();
    m_previewColor = new PreviewColor(m_pageRes.frameAccentColor);
    m_previewColor->setColor(m_accentColor);

    m_pageRes.pushButtonAccentColor->setIcon(QIcon(":/res/arrowdown.png"));
    connect(m_pageRes.pushButtonAccentColor, SIGNAL(pressed()), this, SLOT(showColorDialog()));

    const QString strCalligraphy = QString(":/res/calligraphy.png");
    const QString strClouds = QString(":/res/clouds.png");
    const QString strTreeRings = QString(":/res/treerings.png");
    m_pageRes.comboBoxBackground->addItem("None", QString());
    m_pageRes.comboBoxBackground->addItem("Calligraphy", strCalligraphy);
    m_pageRes.comboBoxBackground->addItem("Clouds", strClouds);
    m_pageRes.comboBoxBackground->addItem("Tree Rings", strTreeRings);
    m_curRibbonBackground = m_mainWindow->ribbonBackground();
    int indexBackground = 0;
    if (m_curRibbonBackground == strCalligraphy)
        indexBackground = 1;
    else if (m_curRibbonBackground == strClouds)
        indexBackground = 2;
    else if (m_curRibbonBackground == strTreeRings)
        indexBackground = 3;
    m_pageRes.comboBoxBackground->setCurrentIndex(indexBackground);
    connect(m_pageRes.comboBoxBackground, SIGNAL(currentIndexChanged(int)), this, SLOT(currentBackgroundChanged(int)));

    updateEnabledWidgets();
}

RibbonPopularPage::~RibbonPopularPage() {}

void RibbonPopularPage::setupPage()
{
    QRibbon::RibbonStyle *ribbonStyle = qobject_cast<QRibbon::RibbonStyle *>(qApp->style());
    Q_ASSERT(ribbonStyle != Q_NULL);
    m_currentThemeId = ribbonStyle->getTheme();
    m_pageRes.comboBoxTheme->setCurrentIndex(m_currentThemeId);
}

void RibbonPopularPage::updateEnabledWidgets()
{
    bool isThemeModern = m_currentThemeId == OfficeStyle::Office2013White ||
                         m_currentThemeId == OfficeStyle::Office2013Gray ||
                         m_currentThemeId == OfficeStyle::Office2013Dark;

    m_pageRes.labelAccentColor->setEnabled(isThemeModern);
    m_pageRes.pushButtonAccentColor->setEnabled(isThemeModern);
    m_pageRes.labelBackgroundImage->setEnabled(isThemeModern);
    m_pageRes.comboBoxBackground->setEnabled(isThemeModern);
    m_pageRes.frameAccentColor->setEnabled(isThemeModern);
}

void RibbonPopularPage::currentThemeChanged(int index)
{
    QVariant var = m_pageRes.comboBoxTheme->itemData(index);
    if (var.isValid())
        m_currentThemeId = (QRibbon::RibbonStyle::Theme)var.toUInt();
    updateEnabledWidgets();
}

void RibbonPopularPage::currentBackgroundChanged(int index)
{
    QVariant var = m_pageRes.comboBoxBackground->itemData(index);
    if (var.isValid())
        m_curRibbonBackground = var.toString();
}

void RibbonPopularPage::showColorDialog()
{
    QColor col = QColorDialog::getColor(m_accentColor, this);
    if (!col.isValid())
        return;

    m_accentColor = col;
    m_previewColor->setColor(m_accentColor);
}

void RibbonPopularPage::accepted()
{
    if (!m_wasDisplayed)
        return;

    QRibbon::RibbonStyle *ribbonStyle = qobject_cast<QRibbon::RibbonStyle *>(qApp->style());
    Q_ASSERT(ribbonStyle != Q_NULL);
    ribbonStyle->setTheme(m_currentThemeId);
    ribbonStyle->setAccentColor(m_accentColor);
    m_mainWindow->setRibbonBackground(m_curRibbonBackground);
}

void RibbonPopularPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_wasDisplayed = true;
    setupPage();
}
