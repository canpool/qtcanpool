#include <QResizeEvent>
#include <QPainter>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include "QtnCommonStyleV5.h"
#include "demomainwindow.h"
#include "aboutdialog.h"

#define IMAGE_TITLE 80
#define CONTENT_BORDER 20

#define SIZE_COLOR_BUTTON 50

/* ColorButton */
ColorButton::ColorButton(QWidget* parent)
    : QToolButton(parent)
{
    m_color = Qt::white;
    m_sunken = false;
    setCursor(Qt::PointingHandCursor);
    QAction* act = new QAction(this);
    setDefaultAction(act);
}

QSize ColorButton::sizeHint() const
{
    return QSize(SIZE_COLOR_BUTTON, SIZE_COLOR_BUTTON);
}

const QColor& ColorButton::color() const
{
    return m_color;
}

void ColorButton::setColor(const QColor& color)
{
    m_color = color;
}

void ColorButton::setToolTip(const QString &tip)
{
    if (QAction* action = defaultAction())
        action->setToolTip(tip);
}

void ColorButton::paintEvent(QPaintEvent* p)
{
    Q_UNUSED(p);
    QPainter painter(this);
    QRect r = QRect(0, 0, width(), height());
    painter.fillRect(r, color());
    QPen pen = painter.pen();
    pen.setWidth(5);
    pen.setColor(m_sunken ? Qt::lightGray : Qt::white);
    painter.setPen(pen);
    painter.drawRect(r.adjusted(0, 0, -1, -1));
}

void ColorButton::enterEvent(QEvent* event)
{
    QToolButton::enterEvent(event);
    m_sunken = true;
    update();
}

void ColorButton::leaveEvent(QEvent* event)
{
    QToolButton::leaveEvent(event);
    m_sunken = false;
    update();
}

/* TitleBarCaption */
TitleBarCaption::TitleBarCaption(const QString& productName, const QString& version, const QString& caption)
    : QLabel()
{
    setAttribute(Qt::WA_NoSystemBackground, true);
    setOpenExternalLinks(true);
    setText(QString("<table border=\"0\"><tr style=\"vertical-align: middle;\"> \
         <td><img src=\":res/logo.png\"></td><td><strong>%1 %2</strong></td><td> %3 by \
         <a href=\"http://www.devmachines.com\">Developer Machines</a></td></tr></table>").arg(productName, version, caption));
    connect(this, SIGNAL(linkHovered(const QString &)), this, SLOT(setLink(const QString &)));
}

TitleBarCaption::~TitleBarCaption()
{
}

void TitleBarCaption::mousePressEvent(QMouseEvent* event)
{
    QLabel::mousePressEvent(event);
    if (!m_link.isEmpty())
        event->accept();
}

/* DemoMainWindow */
DemoMainWindow::DemoMainWindow(const QString& productName, const QString& version, const QString& caption)
    : QWidget(), m_infoButton(0), m_productName(productName), m_version(version), hl(Q_NULL)
{
    setWindowTitle(caption);
    CommonStyleV5::create();

    m_material = new AcrylicMaterialWidget(this);
    m_material->setAcrylicBlend(AcrylicMaterialWidget::BackgroundBlend);
    m_material->setTintColor(Qt::darkGray);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    titleBar()->setWidget(new TitleBarCaption(productName, version, caption));
    titleBar()->setBlurBehindWindowEnabled(true);
#if 0
    titleBar()->setExtendViewIntoTitleBar(true);
    QPalette p = palette();
    p.setColor(QPalette::WindowText, Qt::white);
    setPalette(p);
#endif
    titleBar()->show();
#endif
}

WindowTitleBar* DemoMainWindow::titleBar() const
{
    return WindowTitleBar::get(const_cast<DemoMainWindow *>(this));
}

void DemoMainWindow::aboutLinkActivated(const QString& link)
{
    Q_UNUSED(link);
    Qtitan::AboutDialog::show(this, m_productName + " " + tr("Demo"), m_productName, m_version);
}

void DemoMainWindow::colorButtonClicked()
{
    ColorButton* button = qobject_cast<ColorButton *>(sender());
    if (button == Q_NULL)
        return;
    setShadeColor(button->color());
    setThemeColor(convertToTheme(button->color()));
}

#define MARGINS 4
void DemoMainWindow::setDemoWidget(QWidget* widget, QWidget* settings, Qt::Alignment alignment, bool useThemeLayout)
{
    m_settings = settings;

    if (m_settings)
    {
        QList<QLabel*> labels = m_settings->findChildren<QLabel*>();
        foreach(QLabel* l, labels)
        {
            QPalette p = l->palette();
            p.setColor(l->foregroundRole(), Qt::white);
            l->setPalette(p);
        }

        QList<QCheckBox*> boxes = m_settings->findChildren<QCheckBox*>();
        foreach(QCheckBox* b, boxes)
        {
            QPalette p = b->palette();
            p.setColor(b->foregroundRole(), Qt::white);
            b->setPalette(p);
        }
    }

    vl = new QVBoxLayout(this);

    hl = new QHBoxLayout();
    m_infoButton = new QLabel(this);
    m_infoButton->setCursor(Qt::PointingHandCursor);
    m_infoButton->setText("<a href=\"#\"><img src=\":res/question.png\"></a>");
    connect(m_infoButton, SIGNAL(linkActivated(const QString&)), this, SLOT(aboutLinkActivated(const QString&)));
    vl->addWidget(m_infoButton, false, Qt::AlignRight);
    hl->addWidget(widget, m_settings != 0 ? 1 : 0);
    if (m_settings != 0)
        hl->addWidget(m_settings, 0, alignment);

    hl->setContentsMargins(30, MARGINS, 30, 30);
//    vl->addSpacing(MARGINS * 2);
    vl->setContentsMargins(MARGINS, MARGINS, MARGINS, MARGINS);

    QHBoxLayout* themeLayout = useThemeLayout ? createThemeLayout() : 0;
    if (themeLayout != 0)
        vl->addLayout(themeLayout);
    else
        hl->setContentsMargins(30, SIZE_COLOR_BUTTON, 30, 30);

    vl->addLayout(hl, true);
    widget->setFocus();
}

ColorButton* DemoMainWindow::createButton(MainTheme theme)
{
    ColorButton* button = new ColorButton(this);
    button->setColor(convertToColor(theme));
    connect(button, SIGNAL(clicked()), this, SLOT(colorButtonClicked()));
    m_buttons.insert(theme, button);
    return button;
}

QHBoxLayout* DemoMainWindow::createThemeLayout()
{
    QHBoxLayout* themeLayout = new QHBoxLayout();
    themeLayout->setSizeConstraint(QLayout::SetFixedSize);
    themeLayout->addSpacing(0);
    themeLayout->setContentsMargins(30, IMAGE_TITLE - 5, 30, 0);

    ColorButton* button = createButton(M_THEME_1);
    themeLayout->addWidget(button);
    themeLayout->addSpacing(SIZE_COLOR_BUTTON);

    button = createButton(M_THEME_2);
    themeLayout->addWidget(button);
    themeLayout->addSpacing(SIZE_COLOR_BUTTON);

    button = createButton(M_THEME_3);
    themeLayout->addWidget(button);
    themeLayout->addSpacing(SIZE_COLOR_BUTTON);

    button = createButton(M_THEME_4);
    themeLayout->addWidget(button);
    themeLayout->addSpacing(SIZE_COLOR_BUTTON);

    button = createButton(M_THEME_5);
    themeLayout->addWidget(button);
    themeLayout->addSpacing(SIZE_COLOR_BUTTON);

    QLabel* themeDescription = new QLabel(this);
    QPalette palette = themeDescription->palette();
    palette.setColor(QPalette::WindowText, Qt::white);
    themeDescription->setPalette(palette);
    themeDescription->setWordWrap(true);
    
    QString text = descriptionText();
    if (text.isEmpty())
        text = "Select color to change tone of the main grid's elements (icons, backgrouds, captions and etc.).\
  Please note, you can replace any icons of the grid to better fit for your application style. For more details, refer to GridThemeManager class description of the documentation.";

    themeDescription->setText(text);
    themeLayout->addWidget(themeDescription, 1);

    return themeLayout;
}

Qt::GlobalColor DemoMainWindow::convertToColor(MainTheme id) const
{
    switch (id)
    {
        case M_THEME_1 :
            return Qt::darkRed;
        case M_THEME_2:
            return Qt::darkMagenta;
        case M_THEME_3:
            return Qt::darkGray;
        case M_THEME_4:
            return Qt::darkGreen;
        case M_THEME_5:
            return Qt::darkCyan;
        default:
            Q_ASSERT(false);
            break;
    };
    return (Qt::GlobalColor)- 1;
}

DemoMainWindow::MainTheme DemoMainWindow::convertToTheme(Qt::GlobalColor color) const
{
    switch (color)
    {
        case Qt::darkRed:
            return M_THEME_1;
        case Qt::darkMagenta:
            return M_THEME_2;
        case Qt::darkGray:
            return M_THEME_3;
        case Qt::darkGreen:
            return M_THEME_4;
        case Qt::darkCyan:
            return M_THEME_5;
        default:
            Q_ASSERT(false);
            break;
    };
    return (MainTheme)-1;
}

DemoMainWindow::MainTheme DemoMainWindow::convertToTheme(const QColor& color) const
{
    if (color == Qt::darkRed)
        return M_THEME_1;
    else if (color == Qt::darkMagenta)
        return M_THEME_2;
    else if (color == Qt::darkGray)
        return M_THEME_3;
    else if (color == Qt::darkGreen)
        return M_THEME_4;
    else if (color == Qt::darkCyan)
        return M_THEME_5;
    return (MainTheme)-1;
}

ColorButton* DemoMainWindow::buttomByTheme(MainTheme id) const
{
    return m_buttons.value(id);
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
static QRect qtn_marginsRemoved(const QRect& rc, const QMargins& margins)
{
    return QRect(QPoint(rc.left() + margins.left(), rc.top() + margins.top()),
        QPoint(rc.right() - margins.right(), rc.bottom() - margins.bottom()));
}
#endif

void DemoMainWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setClipRegion(event->region());
    QRect bounded(QPoint(0, 0), size());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    bounded = QRect(QPoint(0, 0), bounded.marginsRemoved(contentsMargins()).size());
#else
    bounded = QRect(QPoint(0, 0), qtn_marginsRemoved(bounded, contentsMargins()).size());
#endif
    painter.translate(contentsMargins().left(), contentsMargins().top());
    QRect r = bounded;
    r.setHeight(IMAGE_TITLE);
    painter.fillRect(r, QBrush(QPixmap(":/res/topheader.png")));
    painter.drawPixmap(10, 0, QPixmap(":/res/qtitanlogo64x64.png"));
    

    r.setTop(r.height());
    r.setHeight(bounded.size().height() - r.height());
    painter.fillRect(r, QColor(0x191919));
#if 0
    painter.save();
    QRect borderRect = bounded.adjusted(CONTENT_BORDER, IMAGE_TITLE, -CONTENT_BORDER, -CONTENT_BORDER);
    painter.setOpacity(0.3);
    QLinearGradient linearGrad(QPointF(200, 100), QPointF(500, 300));
    linearGrad.setColorAt(0, Qt::darkGray);
    linearGrad.setColorAt(1, Qt::lightGray);
    painter.fillRect(borderRect, linearGrad);
    painter.setOpacity(1);
    painter.restore();
#endif
    QFont f = painter.font();
    f.setPixelSize(20);
    f.setBold(true);
    painter.setFont(f);
    painter.setPen(QColor(0xFFFFFFFF));
    painter.drawText(84, 40, m_productName + " " + m_version);
}

void DemoMainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    m_material->setGeometry(QRect(QPoint(CONTENT_BORDER, IMAGE_TITLE), 
        size() - QSize(CONTENT_BORDER * 2, IMAGE_TITLE + CONTENT_BORDER)));
}
