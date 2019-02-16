#include <QObject>
#include <QPainter>
#include <QAction>
#include <QLabel>
#include <QStyleOption>
#include <qdebug.h>

#include "QtnRibbonStyle.h"
#include "QtnStyleHelpers.h"
#include "galleryItems.h"

using namespace Qtitan;


/* GalleryItemTable */
GalleryItemTable::GalleryItemTable()
{
}

void GalleryItemTable::draw(QPainter* p, Qtitan::RibbonGallery* gallery, QRect rcItem, bool enabled, bool selected, bool pressed, bool checked)
{
    Q_UNUSED(enabled);
    Q_UNUSED(pressed);
    Q_UNUSED(checked);

    int indexItem = gallery->isItemSelected() ? gallery->getSelectedItem() : -1;
    QRect rcItems = gallery->getItemsRect();
    QSize szItems = gallery->galleryGroup()->getSize();
    int itemsPerRow = rcItems.width() / szItems.width();

    int columns = indexItem < 1 ? 0 : (indexItem - 1) % itemsPerRow + 1;
    int rows = indexItem < 1 ? 0 : (indexItem - 1) / itemsPerRow + 1;


    int columnItem = (getIndex() - 1) % itemsPerRow + 1;
    int rowItem = (getIndex() - 1) / itemsPerRow + 1;

    selected = rowItem <= rows && columnItem <= columns;

    rcItem.adjust(1, 1, -1, -1);

    if (selected)
    {
        Qtitan::DrawHelpers::draw3DRect(*p, QColor(239, 72, 16), QColor(239, 72, 16), rcItem.left(), rcItem.top(), rcItem.width(), rcItem.height(), true);
        rcItem.adjust(1, 1, -1, -1);
        Qtitan::DrawHelpers::draw3DRect(*p, QColor(255, 226, 148), QColor(255, 226, 148), rcItem.left(), rcItem.top(), rcItem.width(), rcItem.height(), true);
    }
    else
    {
        rcItem.adjust(1, 1, -1, -1);
        Qtitan::DrawHelpers::draw3DRect(*p, QColor(0, 0, 0), QColor(0, 0, 0), rcItem.left(), rcItem.top(), rcItem.width(), rcItem.height(), true);
    }
    rcItem.adjust(1, 1, -1, -1);
    p->fillRect(rcItem, QColor(255, 255, 255));
}

/* GalleryItemTableLabel */
GalleryItemTableLabel::GalleryItemTableLabel()
{
    setSeparator(true);
}

void GalleryItemTableLabel::draw(QPainter* p, Qtitan::RibbonGallery* gallery, QRect rcItem, bool enabled, bool selected, bool pressed, bool checked)
{
    int item = gallery->isItemSelected() ? gallery->getSelectedItem() : -1;

    QRect rcItems = gallery->getItemsRect();
    QSize szItems = gallery->galleryGroup()->getSize();
    int itemsPerRow = rcItems.width() / szItems.width();

    int columns = item < 1 ? 0 : (item - 1) % itemsPerRow + 1;
    int rows = item < 1 ? 0 : (item- 1) / itemsPerRow + 1;

    if (item < 1)
        setCaption(QObject::tr("Insert Table"));
    else
        setCaption(QObject::tr("%1 x %2 Table").arg(rows).arg(columns));

    Qtitan::RibbonGalleryItem::draw(p, gallery, rcItem, enabled, selected, pressed, checked);
}




enum NUM_COLOR
{
    clr_yellow,
    clr_bright_green,
    clr_turquoise,
    clr_pink,
    clr_blue,
    clr_red,
    clr_dark_blue,
    clr_teal,
    clr_green,
    clr_violet,
    clr_dark_red,
    clr_dark_yellow,
    clr_gray50,
    clr_gray25,
    clr_black,
};

static QString getNameColor(int idColor)
{
    switch(idColor)
    {
        case clr_yellow :
            return QObject::tr("Yellow");
        case clr_bright_green :
            return QObject::tr("Bright Green");
        case clr_turquoise :
            return QObject::tr("Turquoise");
        case clr_pink :
            return QObject::tr("Pink");
        case clr_blue :
            return QObject::tr("Blue");
        case clr_red :
            return QObject::tr("Red");
        case clr_dark_blue :
            return QObject::tr("Dark Blue");
        case clr_teal :
            return QObject::tr("Teal");
        case clr_green :
            return QObject::tr("Green");
        case clr_violet :
            return QObject::tr("Violet");
        case clr_dark_red :
            return QObject::tr("Dark Red");
        case clr_dark_yellow :
            return QObject::tr("Dark Yellow");
        case clr_gray50 :
            return QObject::tr("Gray-50%");
        case clr_gray25 :
            return QObject::tr("Gray-25%");
        case clr_black :
            return QObject::tr("Black");
        default:
            break;
    }
    return QString();
}


struct qtn_colorPreviewRect
{
    QColor clr;
    QString caption;
};

struct COLORPREVIEW
{
    qtn_colorPreviewRect rec[10];
}
themeColors[] =
{
    {//Office
       {{QColor(255, 255, 255), QObject::tr("White")},       {QColor(0, 0, 0),       QObject::tr("Black")},
        {QColor(238, 236, 225), QObject::tr("Tan")},         {QColor(31, 73, 125),   QObject::tr("Dark Blue")}, {QColor(79, 129, 189), QObject::tr("Blue")}, {QColor(192, 80, 77),  QObject::tr("Red")},
        {QColor(155, 187, 89),  QObject::tr("Olive Green")}, {QColor(128, 100, 162), QObject::tr("Purple")},    {QColor(75, 172, 198), QObject::tr("Aqua")}, {QColor(247, 150, 70), QObject::tr("Orange")}}
    },

    {//Grayscale
        {{QColor(255, 255, 255), QObject::tr("White")},      {QColor(0, 0, 0),       QObject::tr("Black")},
         {QColor(248, 248, 248), QObject::tr("")},           {QColor(0, 0, 0),       QObject::tr("")},          {QColor(221, 221, 221), QObject::tr("")}, {QColor(178, 178, 178), QObject::tr("")},
         {QColor(150, 150, 150), QObject::tr("")},           {QColor(128, 128, 128), QObject::tr("")},          {QColor(95, 95, 95),    QObject::tr("")}, {QColor(77, 77, 77),    QObject::tr("")}}
    },

    {//Apex
        {{QColor(255, 255, 255), QObject::tr("White")},      {QColor(0, 0, 0),       QObject::tr("Black")},
         {QColor(201, 194, 209), QObject::tr("")},           {QColor(105, 103, 109), QObject::tr("")},          {QColor(206, 185, 102), QObject::tr("")}, {QColor(156, 176, 132), QObject::tr("")},
         {QColor(107, 177, 201), QObject::tr("")},           {QColor(101, 133, 207), QObject::tr("")},          {QColor(126, 107, 201), QObject::tr("")}, {QColor(163, 121, 187), QObject::tr("")}}
    },

    {//Aspect
        {{QColor(255, 255, 255), QObject::tr("White")},       {QColor(0, 0, 0),       QObject::tr("Black")},
         {QColor(227, 222, 209), QObject::tr("")},            {QColor(50, 50, 50),    QObject::tr("")},          {QColor(240, 127, 9), QObject::tr("")}, {QColor(159, 41, 54),  QObject::tr("")},
         {QColor(27, 88, 124),   QObject::tr("")},            {QColor(78, 133, 66),   QObject::tr("")},          {QColor(96, 72, 120), QObject::tr("")}, {QColor(193, 152, 89), QObject::tr("")}}
    },

    {//Civic
        {{QColor(255, 255, 255), QObject::tr("White")},       {QColor(0, 0, 0),       QObject::tr("Black")},
         {QColor(197, 209, 215), QObject::tr("")},            {QColor(100, 107, 134), QObject::tr("")},          {QColor(209, 99, 73),   QObject::tr("")}, {QColor(204, 180, 0),  QObject::tr("")},
         {QColor(140, 173, 174), QObject::tr("")},            {QColor(140, 123, 112), QObject::tr("")},          {QColor(143, 176, 140), QObject::tr("")}, {QColor(209, 144, 73), QObject::tr("")}}
    },

    {//Concourse
        {{QColor(255, 255, 255), QObject::tr("White")},       {QColor(0, 0, 0),       QObject::tr("Black")},
         {QColor(222, 245, 250), QObject::tr("")},            {QColor(70, 70, 70),    QObject::tr("")},          {QColor(045, 162, 191), QObject::tr("")}, {QColor(218, 31, 40), QObject::tr("")},
         {QColor(235, 100, 27),  QObject::tr("")},            {QColor(57, 99, 157),   QObject::tr("")},          {QColor(71, 75, 120),   QObject::tr("")}, {QColor(125, 60, 74), QObject::tr("")}}
    },

    {//Currency
        {{QColor(255, 255, 255), QObject::tr("White")},       {QColor(0, 0, 0),       QObject::tr("Black")},
         {QColor(209, 225, 227), QObject::tr("")},            {QColor(74, 96, 110),   QObject::tr("")},          {QColor(121, 181, 176), QObject::tr("")}, {QColor(180, 188, 76),  QObject::tr("")},
         {QColor(183, 120, 81),  QObject::tr("")},            {QColor(119, 106, 91),  QObject::tr("")},          {QColor(182, 173, 118), QObject::tr("")}, {QColor(149, 174, 177), QObject::tr("")}}
    },

    {//Deluxe
        {{QColor(255, 255, 255), QObject::tr("White")},       {QColor(0, 0, 0),       QObject::tr("Black")},
         {QColor(255, 249, 229), QObject::tr("")},            {QColor(48, 53, 110),   QObject::tr("")},          {QColor(204, 71, 87),  QObject::tr("")}, {QColor(255, 111, 97), QObject::tr("")},
         {QColor(255, 149, 62),  QObject::tr("")},            {QColor(248, 189, 82),  QObject::tr("")},          {QColor(70, 166, 189), QObject::tr("")}, {QColor(84, 136, 188), QObject::tr("")}}
    }
};


/* GalleryItemFontColor */
GalleryItemFontColor::GalleryItemFontColor(QColor color, const QString& caption, bool topMargin /*= true*/, bool botttomMargin /*= true*/)
{
    m_color = color;
    setCaption(caption);
    setToolTip(caption);

    m_topMargin = topMargin;
    m_bottomMargin = botttomMargin;

    setSizeHint(QSize(17, 13 + (topMargin ? 2 : 0) + (botttomMargin ? 2 : 0)));
}

void GalleryItemFontColor::draw(QPainter* p, Qtitan::RibbonGallery* gallery, QRect rcItem, 
                                bool enabled, bool selected, bool pressed, bool checked)
{
    Q_UNUSED(gallery);
    Q_UNUSED(enabled);
    Q_UNUSED(pressed);

    rcItem.adjust(2, 0, -2, 0);
    if (m_topMargin)
        rcItem.adjust(0, 2, 0, 0);

    if (m_bottomMargin)
        rcItem.adjust(0, 0, 0, -2);

    p->fillRect(rcItem, m_color);

    if (selected || checked)
    {
        rcItem.adjust(0, 0, -1, -1);

        if (selected)
            Qtitan::DrawHelpers::draw3DRect(*p, QColor(242, 148, 54), QColor(242, 148, 54), rcItem.left(), rcItem.top(), rcItem.width(), rcItem.height(), true);
        else
            Qtitan::DrawHelpers::draw3DRect(*p, QColor(239, 72, 16), QColor(239, 72, 16), rcItem.left(), rcItem.top(), rcItem.width(), rcItem.height(), true);

        rcItem.adjust(1, 1, -1, -1);

        Qtitan::DrawHelpers::draw3DRect(*p, QColor(255, 226, 148), QColor(255, 226, 148), rcItem.left(), rcItem.top(), rcItem.width(), rcItem.height(), true);
    }
    else
    {
        QColor col(197, 197, 197);
        if (m_topMargin) p->fillRect(rcItem.left(), rcItem.top(), rcItem.width(), 1, col);
        p->fillRect(rcItem.left(), rcItem.top(), 1, rcItem.height(), col);
        p->fillRect(rcItem.right(), rcItem.top(), 1, rcItem.height(), col);
        if (m_bottomMargin) p->fillRect(rcItem.left(), rcItem.bottom(), rcItem.width(), 1, col);
    }
}

void GalleryItemFontColor::addStandardBackColors(Qtitan::RibbonGalleryGroup* items)
{
    items->appendItem(new GalleryItemFontColor(qRgb(0xFF, 0xFF, 0x00), getNameColor(clr_yellow)));
    items->appendItem(new GalleryItemFontColor(qRgb(0x00, 0xFF, 0x00), getNameColor(clr_bright_green)));
    items->appendItem(new GalleryItemFontColor(qRgb(0x00, 0xFF, 0xFF), getNameColor(clr_turquoise)));
    items->appendItem(new GalleryItemFontColor(qRgb(0xFF, 0x00, 0xFF), getNameColor(clr_pink)));
    items->appendItem(new GalleryItemFontColor(qRgb(0x00, 0x00, 0xFF), getNameColor(clr_blue)));
    items->appendItem(new GalleryItemFontColor(qRgb(0xFF, 0x00, 0x00), getNameColor(clr_red)));
    items->appendItem(new GalleryItemFontColor(qRgb(0x00, 0x00, 0x80), getNameColor(clr_dark_blue)));
    items->appendItem(new GalleryItemFontColor(qRgb(0x00, 0x80, 0x80), getNameColor(clr_teal)));
    items->appendItem(new GalleryItemFontColor(qRgb(0x00, 0x80, 0x00), getNameColor(clr_green)));
    items->appendItem(new GalleryItemFontColor(qRgb(0x80, 0x00, 0x80), getNameColor(clr_violet)));
    items->appendItem(new GalleryItemFontColor(qRgb(0x80, 0x00, 0x00), getNameColor(clr_dark_red)));
    items->appendItem(new GalleryItemFontColor(qRgb(0x80, 0x80, 0x00), getNameColor(clr_dark_yellow)));
    items->appendItem(new GalleryItemFontColor(qRgb(0x80, 0x80, 0x80), getNameColor(clr_gray50)));
    items->appendItem(new GalleryItemFontColor(qRgb(0xC0, 0xC0, 0xC0), getNameColor(clr_gray25)));
    items->appendItem(new GalleryItemFontColor(qRgb(0x00, 0x00, 0x00), getNameColor(clr_black)));
}

void GalleryItemFontColor::addStandardColors(Qtitan::RibbonGalleryGroup* items)
{
    items->appendItem(new GalleryItemFontColor(QColor(192, 0,   0),   QObject::tr("Dark Red")));
    items->appendItem(new GalleryItemFontColor(QColor(255, 0,   0),   QObject::tr("Red")));
    items->appendItem(new GalleryItemFontColor(QColor(255, 192, 0),   QObject::tr("Orange")));
    items->appendItem(new GalleryItemFontColor(QColor(255, 255, 0),   QObject::tr("Yellow")));
    items->appendItem(new GalleryItemFontColor(QColor(146, 208, 80),  QObject::tr("Light Green")));
    items->appendItem(new GalleryItemFontColor(QColor(0,   176, 80),  QObject::tr("Green")));
    items->appendItem(new GalleryItemFontColor(QColor(0,   176, 240), QObject::tr("Light Blue")));
    items->appendItem(new GalleryItemFontColor(QColor(0,   112, 192), QObject::tr("Blue")));
    items->appendItem(new GalleryItemFontColor(QColor(0,   32,  96),  QObject::tr("Dark Blue")));
    items->appendItem(new GalleryItemFontColor(QColor(112, 48,  160), QObject::tr("Purple")));
}

int nMatrix[6][10] = 
{
    {  0,   0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { -5, +50, -10, +80, +80, +80, +80, +80, +80, +80},
    { -15, +35, -25, +60, +60, +60, +60, +60, +60, +60},
    { -25, +25, -50, +40, +40, +40, +40, +40, +40, +40},
    { -35, +15, -75, -25, -25, -25, -25, -25, -25, -25},
    { -50, +5, -90, -50, -50, -50, -50, -50, -50, -50},
};

QString strMatrixTip[10] = 
{
    QObject::tr("Background 1"), QObject::tr("Text 1"), QObject::tr("Background 2"), QObject::tr("Text 2"), 
    QObject::tr("Accent 1"), QObject::tr("Accent 2"), QObject::tr("Accent 3"), QObject::tr("Accent 4"), QObject::tr("Accent 5"), QObject::tr("Accent 6")
};

QRgb translateColor(QRgb clr, int nTransalte)
{
    return nTransalte == 0 ? clr : nTransalte > 0 ? Qtitan::DrawHelpers::lightColor(clr, nTransalte) : Qtitan::DrawHelpers::darkColor(clr, nTransalte);
}

QString getColorTip(QString strToolTip, QString strAccent, int nTransalte)
{
    QString strTranslate = strToolTip;
    strTranslate += QObject::tr(", %1").arg(nTransalte > 0 ? QObject::tr("Lighter") : QObject::tr("Darker"));
    strTranslate += QObject::tr(" %1%").arg(abs(nTransalte));
    return (!strToolTip.isEmpty() ? strToolTip + QObject::tr(", ") : QString(QObject::tr(""))) + strAccent + (nTransalte != 0 ? strTranslate : QObject::tr(""));
}

void GalleryItemFontColor::addThemeColors(RibbonGalleryGroup* pItems, int nTheme)
{
    const COLORPREVIEW& clrPreview = themeColors[nTheme];

    for (int j = 0; j < 6; j++)
    {
        bool bTopMargin = j < 2;
        bool bBottomMargin = j == 0 || j == 5;

        for (int i = 0; i < 10; i++)
        {
            int nTranslate = nMatrix[j][i];
            QColor clr = translateColor(clrPreview.rec[i].clr.rgb(), nTranslate);
            QString strToolTip = getColorTip(clrPreview.rec[i].caption, strMatrixTip[i], nTranslate);

            pItems->appendItem(new GalleryItemFontColor(clr, strToolTip, bTopMargin, bBottomMargin));
        }
    }
}


/* GalleryItemUndo */
WidgetGalleryUndo::WidgetGalleryUndo()
{
}

void WidgetGalleryUndo::selectedItemChanged()
{
    if (QLabel* label = findInfoWidget())
        label->setText(isItemSelected() ? QObject::tr("Undo %1 Actions").arg(getSelectedItem() + 1) : QObject::tr("Cancel"));
}

QLabel* WidgetGalleryUndo::findInfoWidget()
{
    if (!parentWidget())
        return Q_NULL;

    const QObjectList& listChildren = parentWidget()->children();
    for (int i = 0; i < listChildren.size(); ++i) 
    {
        QWidget* w = qobject_cast<QWidget*>(listChildren.at(i));

        if (w && qobject_cast<QLabel*>(w) )
            return (QLabel*)w;
    }
    return NULL;
}

GalleryItemUndo::GalleryItemUndo(const QString& caption)
{
    setCaption(caption);
}

void GalleryItemUndo::draw(QPainter* p, Qtitan::RibbonGallery* gallery, QRect rcItem, 
                           bool enabled, bool selected, bool pressed, bool checked)
{
    int item = gallery->isItemSelected() ? gallery->getSelectedItem() : -1;
    selected = getIndex() <= item;
    Qtitan::RibbonGalleryItem::draw(p, gallery, rcItem, enabled, selected, pressed, checked);
}



