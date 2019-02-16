/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#include <QApplication>
#include <QSettings>
#include <QVariant>
#include <QPainter>
#include <QBrush>
#include <QStyleOption>
#include <QPixmapCache>
#include <QBitmap>
#include <QPushButton>
#include <QDialog>
#include <QStyleFactory>
#include <QBasicTimer>
#include <qevent.h>

#include "QtnStyleHelpers.h"
#include "QtnCommonStyle.h"
#include "QtnCommonStylePrivate.h"

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wswitch"
#endif

using namespace Qtitan;

/*! \brief The cachedPixmap function provides an application-wide cache for pixmaps.
 */
static QPixmap cachedPixmap(const QString& img)
{
    if (QPixmap *p = QPixmapCache::find(img))
        return *p;

    QPixmap pm;
    pm = QPixmap::fromImage(QImage(img), Qt::OrderedDither | Qt::OrderedAlphaDither);

//    if (pm.isNull())
//        return QPixmap();

    QPixmapCache::insert(img, pm);
    return pm;
}

/* StyleHelper */
StyleHelper::StyleHelper(QObject* parent)
    : QObject(parent)
    , m_pSettings(Q_NULL)
{
}

StyleHelper::~StyleHelper()
{
    delete m_pSettings;
}

void StyleHelper::openIniFile(const QString& fileName)
{
    Q_ASSERT(!fileName.isEmpty());

    if (fileName.isEmpty()) 
        return;

    delete m_pSettings;
    m_pSettings = new QSettings(fileName, QSettings::IniFormat);

    Q_ASSERT(m_pSettings->status() == QSettings::NoError);

    m_pSettings->sync();
    m_pSettings->setFallbacksEnabled(true);
}

QString StyleHelper::getValue(const QString& prefix, const QString& key) const
{
    Q_ASSERT(m_pSettings != Q_NULL);

    QString strValue;
    m_pSettings->beginGroup(prefix);
    QVariant value = m_pSettings->value(key);

    if (value.isValid() && value.type() == QVariant::String)
        strValue = value.toString();

    m_pSettings->endGroup();

    return strValue;
}

QColor StyleHelper::getColor(const QString& prefix, const QString& key, QColor color) const
{
    Q_ASSERT(m_pSettings != Q_NULL);

    int r = 0, g = 0, b = 0;
    QString strValue = getValue(prefix, key);

    if (!strValue.isEmpty())
    {
        QStringList strList = strValue.split(' ');
        if (strList.size() == 3)
        {
            r = strList[0].toUInt(); 
            g = strList[1].toUInt();
            b = strList[2].toUInt();
        }
        return QColor(r, g, b);
    }
    return color;
}

int StyleHelper::getInteger(const QString& prefix, const QString& key, int val) const
{
    Q_ASSERT(m_pSettings != Q_NULL);

    QString strValue = getValue(prefix, key);
    if (!strValue.isEmpty())
        val = strValue.toInt();
    return val;
}


/* QtitanAnimation */
QtitanAnimation::~QtitanAnimation() 
{
    setWidget(Q_NULL);
}

void QtitanAnimation::setWidget(QWidget* widget)
{ 
    if (widget)
        widget->installEventFilter(this);
    else if (_widget)
        _widget->removeEventFilter(this);

    _widget = widget; 
}

void QtitanAnimation::paint(QPainter *painter, const QStyleOption *option)
{
    Q_UNUSED(option);
    Q_UNUSED(painter);
}

void QtitanAnimation::drawBlendedImage(QPainter* painter, QRect rect, float alpha) 
{
    if (_secondaryImage.isNull() || _primaryImage.isNull())
        return;

    if (_tempImage.isNull())
        _tempImage = _secondaryImage;

    const int a = qRound(alpha*256);
    const int ia = 256 - a;
    const int sw = _primaryImage.width();
    const int sh = _primaryImage.height();
    const int bpl = _primaryImage.bytesPerLine();
    switch(_primaryImage.depth()) 
    {
        case 32:
            {
                uchar *mixed_data = _tempImage.bits();
                const uchar *back_data = _primaryImage.bits();
                const uchar *front_data = _secondaryImage.bits();
                for (int sy = 0; sy < sh; sy++) 
                {
                    quint32* mixed = (quint32*)mixed_data;
                    const quint32* back = (const quint32*)back_data;
                    const quint32* front = (const quint32*)front_data;
                    for (int sx = 0; sx < sw; sx++) 
                    {
                        quint32 bp = back[sx];
                        quint32 fp = front[sx];
                        mixed[sx] =  qRgba ((qRed(bp)*ia + qRed(fp)*a)>>8,
                                            (qGreen(bp)*ia + qGreen(fp)*a)>>8,
                                            (qBlue(bp)*ia + qBlue(fp)*a)>>8,
                                            (qAlpha(bp)*ia + qAlpha(fp)*a)>>8);
                    }
                    mixed_data += bpl;
                    back_data += bpl;
                    front_data += bpl;
                }
            }
        default:
            break;
    }
    painter->drawPixmap(rect, QPixmap::fromImage(_tempImage, Qt::AutoColor));
}

bool QtitanAnimation::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == _widget && event->type() == QEvent::Hide)
        setWidget(Q_NULL);

    return QObject::eventFilter(obj, event);
}

void QtitanTransition::paint(QPainter* painter, const QStyleOption* option)
{
    float alpha = 1.0;
    if (_duration > 0) 
    {
        QTime current = QTime::currentTime();

        if (_startTime > current)
            _startTime = current;

        int timeDiff = _startTime.msecsTo(current);
        alpha = timeDiff/(float)_duration;
        if (timeDiff > _duration) 
        {
            _running = false;
            alpha = 1.0;
        }
    }
    else 
    {
        _running = false;
    }
    drawBlendedImage(painter, option->rect, alpha);
}



namespace Qtitan
{
    /* RibbonPagePrivate */
    class CommonStylePrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(CommonStyle)
    public:
        explicit CommonStylePrivate();
        virtual ~CommonStylePrivate();
        static QString m_styleName;
        
    public:
        void initialization();

        void timerEvent();
        void startAnimation(QtitanAnimation*);
        void stopAnimation(const QWidget*);
        QtitanAnimation* widgetAnimation(const QWidget* widget) const;

        void setPalette(QWidget* widget);
        void unsetPalette(QWidget* widget);

        void setAutoFillBackground(QWidget* widget);
        void unsetAutoFillBackground(QWidget* widget);

    public:
        QStyle* m_defaultStyle;
        StyleHelper* m_helper;
        QList<QtitanAnimation*> m_animations;
        QBasicTimer m_animationTimer;
        QHash<const QWidget*, QPalette> m_customPaletteWidgets; // widgets whose palette we tampered
        QHash<const QWidget*, bool> m_customAutoFillBackground; 
    };
}

QString CommonStylePrivate::m_styleName = QString();

CommonStylePrivate::CommonStylePrivate()
{
    if (m_styleName.isEmpty() && qApp->style())
        m_styleName = qApp->style()->objectName();

    m_defaultStyle = Q_NULL;
    m_helper = Q_NULL;
}

CommonStylePrivate::~CommonStylePrivate()
{
    delete m_defaultStyle;
    m_defaultStyle = Q_NULL;
}

void CommonStylePrivate::initialization()
{
    QTN_P(CommonStyle);
    m_helper = new StyleHelper(&p);

    if (!m_styleName.isEmpty())
        m_defaultStyle = QStyleFactory::create(m_styleName);
}

void CommonStylePrivate::timerEvent()
{
    for (int i = m_animations.size() - 1 ; i >= 0; --i) 
    {
        QtitanAnimation* pItem = m_animations[i];
        QWidget* pWidget = pItem->widget();
        if (pWidget)
            pWidget->update();

        if (!pWidget ||
            !pWidget->isEnabled() ||
            !pWidget->isVisible() ||
            pWidget->window()->isMinimized() ||
            !pItem->running())
        {
            QtitanAnimation* a = m_animations.takeAt(i);
            delete a;
        }
    }

    if (m_animations.size() == 0 && m_animationTimer.isActive()) 
        m_animationTimer.stop();
}

void CommonStylePrivate::stopAnimation(const QWidget *w)
{
    for (int i = m_animations.size() - 1 ; i >= 0 ; --i) 
    {
        if (m_animations[i]->widget() == w) 
        {
            QtitanAnimation* a = m_animations.takeAt(i);
            delete a;
            break;
        }
    }
}

void CommonStylePrivate::startAnimation(QtitanAnimation* t)
{
    QTN_P(CommonStyle);
    stopAnimation(t->widget());
    m_animations.append(t);
    if (m_animations.size() > 0 && !m_animationTimer.isActive())
        m_animationTimer.start(45, &p);
}

QtitanAnimation* CommonStylePrivate::widgetAnimation(const QWidget* widget) const
{
    if (!widget)
        return 0;
    foreach (QtitanAnimation* a, m_animations) 
    {
        if (a->widget() == widget)
            return a;
    }
    return 0;
}

void CommonStylePrivate::setPalette(QWidget* widget)
{
    m_customPaletteWidgets.insert(widget, widget->palette());
}

void CommonStylePrivate::unsetPalette(QWidget* widget)
{
    if (m_customPaletteWidgets.contains(widget)) 
    {
        QPalette p = m_customPaletteWidgets.value(widget);
        widget->setPalette(p);
        m_customPaletteWidgets.remove(widget);
    }
}

void CommonStylePrivate::setAutoFillBackground(QWidget* widget)
{
    m_customAutoFillBackground.insert(widget, widget->autoFillBackground());
}

void CommonStylePrivate::unsetAutoFillBackground(QWidget* widget)
{
    if (m_customAutoFillBackground.contains(widget)) 
    {
        bool flag = m_customAutoFillBackground.value(widget);
        widget->setAutoFillBackground(flag);
        m_customAutoFillBackground.remove(widget);
    }
}



/* CommonStyle */
CommonStyle::CommonStyle()
{
    QTN_INIT_PRIVATE(CommonStyle);
    QTN_D(CommonStyle);
    d.initialization();
}

CommonStyle::~CommonStyle()
{
    QTN_FINI_PRIVATE();
}

QColor CommonStyle::getThemeColor(const QString& sectionName, const QString& keyName, QColor color) const
{
    return helper().getColor(sectionName, keyName, color);
}

/*! \reimp */
void CommonStyle::polish(QApplication* pApp )
{
    QTN_D(CommonStyle);
    if (d.m_defaultStyle)
        d.m_defaultStyle->polish(pApp);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QCommonStyle::polish(pApp);
#else
    QWindowsStyle::polish(pApp);
#endif
}

/*! \reimp */
void CommonStyle::polish(QWidget* widget)
{
    QTN_D(CommonStyle);
    if (d.m_defaultStyle)
        d.m_defaultStyle->polish(widget);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QCommonStyle::polish(widget);
#else
    QWindowsStyle::polish(widget);
#endif

    d.unsetPalette(widget);
    d.setPalette(widget);
}

/*! \reimp */
void CommonStyle::unpolish(QApplication* app)
{
    QTN_D(CommonStyle);
    if (d.m_defaultStyle)
        d.m_defaultStyle->unpolish(app);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QCommonStyle::unpolish(app);
#else
    QWindowsStyle::unpolish(app);
#endif
}

/*! \reimp */
void CommonStyle::unpolish(QWidget* widget)
{
    QTN_D(CommonStyle);
    if (d.m_defaultStyle)
        d.m_defaultStyle->unpolish(widget);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QCommonStyle::unpolish(widget);
#else
    QWindowsStyle::unpolish(widget);
#endif

    d.unsetPalette(widget);
    d.stopAnimation(widget);
}

/*! \reimp */
void CommonStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    bool ret = false;
    bool qtitanStyle = false;
    switch(pe)
    {
        case PE_Workspace :
            ret = !isNativeDialog(widget) && drawWorkspace(option, p, widget);
            break;
        case PE_Frame :
            ret = !isNativeDialog(widget) && drawFrame(option, p, widget);
            break;
        case PE_FrameGroupBox :
            ret = false;
            qtitanStyle = !isNativeDialog(widget) && true;
            break;
        case PE_FrameDefaultButton :
                ret = !isNativeDialog(widget) && drawFrameDefaultButton(option, p, widget);
            break;
        case PE_IndicatorToolBarSeparator :
                ret = !isNativeDialog(widget) && drawIndicatorToolBarSeparator(option, p, widget);
            break;
        case PE_IndicatorRadioButton : 
        case PE_IndicatorCheckBox : 
                ret = !isNativeDialog(widget) && drawIndicatorCheckRadioButton(pe, option, p, widget);
            break;
        case PE_PanelLineEdit: 
                ret = !isNativeDialog(widget) && drawControlEdit(option, p, widget);
            break;
        case PE_FrameLineEdit :
                ret = !isNativeDialog(widget) && drawFrameLineEdit(option, p, widget);
                break;
        case PE_PanelStatusBar :
                ret = !isNativeDialog(widget) && drawPanelStatusBar(option, p, widget);
            break;
        case PE_PanelMenu :
                ret = !isNativeDialog(widget) && drawPanelMenu(option, p, widget);
            break;
        case PE_FrameMenu :
                ret = !isNativeDialog(widget) && drawFrameMenu(option, p, widget);
            break;
        case PE_IndicatorMenuCheckMark :
                ret = !isNativeDialog(widget) && drawIndicatorMenuCheckMark(option, p, widget);
            break;
        case PE_IndicatorToolBarHandle :
                ret = !isNativeDialog(widget) && drawIndicatorToolBarHandle(option, p, widget);
            break;
        case PE_PanelButtonCommand : 
                ret = !isNativeDialog(widget) && drawPanelButtonCommand(option, p, widget);
            break;
        case PE_PanelButtonTool :
            {
                if (paintAnimation(tp_PrimitiveElement, (int)pe, option, p, widget))
                    return;
                ret = drawGroupControlEntry(option, p, widget);
                if (!ret)
                    ret = drawPanelButtonTool(option, p, widget);
               break;
            }
        case PE_FrameStatusBarItem:
                ret = true;
            break;
        case PE_IndicatorArrowLeft:
        case PE_IndicatorArrowRight:
        case PE_IndicatorArrowDown:
        case PE_IndicatorArrowUp:
                ret = !isNativeDialog(widget) && drawIndicatorArrow(pe, option, p, widget);
            break;
        case PE_PanelTipLabel: 
                ret = !isNativeDialog(widget) && drawPanelTipLabel(option, p, widget);
            break;
        case PE_IndicatorDockWidgetResizeHandle :
                ret = !isNativeDialog(widget) && drawIndicatorDockWidgetResizeHandle(option, p, widget);
            break;
        case PE_PanelItemViewItem :
                ret = !isNativeDialog(widget) && drawPanelItemViewItem(option, p, widget);
            break;
        case PE_IndicatorHeaderArrow :
                ret = !isNativeDialog(widget) && drawIndicatorHeaderArrow(option, p, widget);
            break;
        case PE_FrameTabWidget :
                ret = !isNativeDialog(widget) && drawFrameTabWidget(option, p, widget);
            break;
        case PE_FrameTabBarBase : // draw a line along the tabs
                ret = true;
            break;
        default:
            break;
    }

    if(!ret)
    {
        QTN_D(const CommonStyle);
        if (d.m_defaultStyle && !qtitanStyle)
            d.m_defaultStyle->drawPrimitive(pe, option, p, widget);
        else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            QCommonStyle::drawPrimitive(pe, option, p, widget);
#else
            QWindowsStyle::drawPrimitive(pe, option, p, widget);
#endif
    }
}

/*! \reimp */
void CommonStyle::drawControl(ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    bool ret = false;
    bool qtitanStyle = false;
    switch (element)  
    {
        case CE_ShapedFrame :
                ret = !isNativeDialog(widget) && drawShapedFrame(opt, p, widget);
            break;
        case CE_PushButton :
        case CE_PushButtonBevel :
                ret = false;
                qtitanStyle = !isNativeDialog(widget) && true;
            break;
        case CE_RadioButton:
        case CE_CheckBox:
                ret = false;
                qtitanStyle = !isNativeDialog(widget) && true;
            break;
        case CE_MenuBarEmptyArea:
                ret = !isNativeDialog(widget) && drawMenuBarEmptyArea(opt, p, widget);
            break;
        case CE_MenuItem:
                ret = !isNativeDialog(widget) && drawMenuItem(opt, p, widget);
            break;
        case CE_MenuBarItem :
                ret = !isNativeDialog(widget) && drawMenuBarItem(opt, p, widget);
            break;
        case CE_SizeGrip :
                ret = !isNativeDialog(widget) && drawSizeGrip(opt, p, widget);
            break;
        case CE_ScrollBarSubLine:
        case CE_ScrollBarAddLine:
                ret = !isNativeDialog(widget) && drawScrollBarLine(element, opt, p, widget);
            break;
        case CE_ScrollBarAddPage:
        case CE_ScrollBarSubPage: 
                ret = !isNativeDialog(widget) && drawScrollBarPage(element, opt, p, widget);
            break;
        case CE_ScrollBarSlider :
            ret = !isNativeDialog(widget) && drawScrollBarSlider(opt, p, widget);
            break;
        case CE_ToolBar :
                ret = !isNativeDialog(widget) && drawToolBar(opt, p, widget);
            break;
        case CE_ToolButtonLabel:
                ret = !isNativeDialog(widget) && drawToolButtonLabel(opt, p, widget);
            break;
        case CE_DockWidgetTitle:
                ret = !isNativeDialog(widget) && drawDockWidgetTitle(opt, p, widget);
            break;
        case CE_TabBarTab:
                ret = !isNativeDialog(widget) && drawTabBarTab(opt, p, widget);
            break;
        case CE_TabBarTabShape :
                ret = !isNativeDialog(widget) && drawTabBarTabShape(opt, p, widget);
            break;
        case CE_ProgressBarGroove :
                ret = !isNativeDialog(widget) && drawProgressBarGroove(opt, p, widget);
            break;
        case CE_ToolBoxTab :
                ret = !isNativeDialog(widget) && drawToolBoxTab(opt, p, widget);
            break;
        case CE_ToolBoxTabShape :
                ret = !isNativeDialog(widget) && drawToolBoxTabShape(opt, p, widget);;
            break;
        case CE_ToolBoxTabLabel :
                ret = !isNativeDialog(widget) && drawToolBoxTabLabel(opt, p, widget);
            break;
        case CE_ItemViewItem : 
                ret = !isNativeDialog(widget) && drawItemViewItem(opt, p, widget);
            break;
        case CE_HeaderEmptyArea :
                ret = !isNativeDialog(widget) && drawHeaderEmptyArea(opt, p, widget);
            break;
        case CE_Header :
                ret = !isNativeDialog(widget) && drawHeader(opt, p, widget);
            break;
        case CE_HeaderSection :
                ret = !isNativeDialog(widget) && drawHeaderSection(opt, p, widget);
            break;
        case CE_TabBarTabLabel :
                ret = !isNativeDialog(widget) && drawTabBarTabLabel(opt, p, widget);
            break;
        default:
            break;
    }
    if(!ret)
    {
        QTN_D(const CommonStyle);
        if (d.m_defaultStyle && !qtitanStyle)
            d.m_defaultStyle->drawControl(element, opt, p, widget);
        else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            QCommonStyle::drawControl(element, opt, p, widget);
#else
            QWindowsStyle::drawControl(element, opt, p, widget);
#endif
    }
}

/*! \reimp */
void CommonStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex* opt, QPainter* p, const QWidget* widget) const
{
    bool ret = false;
    bool qtitanStyle = false;
    switch (cc) 
    {
        case CC_GroupBox:
            ret = false;
            qtitanStyle = !isNativeDialog(widget) && true;
        break;
        case CC_Slider : 
                ret = !isNativeDialog(widget) && drawSlider(opt, p, widget);
            break;
         case CC_SpinBox :
                ret = !isNativeDialog(widget) && drawSpinBox(opt, p, widget);
            break;
        case CC_ScrollBar :
                ret = !isNativeDialog(widget) && drawScrollBar(opt, p, widget);
            break;
        case CC_ToolButton : 
                ret = !isNativeDialog(widget) && drawToolButton(opt, p, widget);
            break;
        case CC_ComboBox:
            {
                if (paintAnimation(tp_ComplexControl, (int)cc, opt, p, widget))
                    return;
                ret = drawComboBox(opt, p, widget);
            }
            break;
        case CC_MdiControls:
                ret = !isNativeDialog(widget) && drawMdiControls(opt, p, widget);
            break;
        case CC_TitleBar :
                ret = !isNativeDialog(widget) && drawTitleBar(opt, p, widget);
            break;

        default:
            break;
    }
    if (!ret)
    {
        QTN_D(const CommonStyle);
        if (d.m_defaultStyle && !qtitanStyle)
            d.m_defaultStyle->drawComplexControl(cc, opt, p, widget);
        else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            QCommonStyle::drawComplexControl(cc, opt, p, widget);
#else
            QWindowsStyle::drawComplexControl(cc, opt, p, widget);
#endif
    }
}

/*! \reimp */
QSize CommonStyle::sizeFromContents(ContentsType ct, const QStyleOption* opt, const QSize& contentsSize, const QWidget* w) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QTN_D(const CommonStyle);
    if (d.m_defaultStyle)
        return d.m_defaultStyle->sizeFromContents(ct, opt, contentsSize, w);

    return QCommonStyle::sizeFromContents(ct, opt, contentsSize, w);
#else
    return QWindowsStyle::sizeFromContents(ct, opt, contentsSize, w);
#endif
}

/*! \reimp */
QRect CommonStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex* opt, SubControl sc, const QWidget* widget) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QTN_D(const CommonStyle);
    if (d.m_defaultStyle)
        return d.m_defaultStyle->subControlRect(cc, opt, sc, widget);

    return QCommonStyle::subControlRect(cc, opt, sc, widget);
#else
    return QWindowsStyle::subControlRect(cc, opt, sc, widget);
#endif
}

/*! \reimp */
int CommonStyle::pixelMetric(PixelMetric pm, const QStyleOption* option, const QWidget* widget) const
{
    QTN_D(const CommonStyle);
    if (d.m_defaultStyle && pm == PM_DockWidgetFrameWidth)
        return d.m_defaultStyle->pixelMetric(pm, option, widget);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return QCommonStyle::pixelMetric(pm, option, widget);
#else
    return QWindowsStyle::pixelMetric(pm, option, widget);
#endif
}

/*! \reimp */
int CommonStyle::styleHint(StyleHint hint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* returnData) const
{
    if (hint == QStyle::SH_CustomBase)
    {
        if (StyleHintReturnThemeColor* vret = qstyleoption_cast<StyleHintReturnThemeColor*>(returnData)) 
        {
            if (!vret->sectionName.isEmpty() && !vret->keyName.isEmpty())
                return getThemeColor(vret->sectionName, vret->keyName, vret->color).rgb();
        }
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return QCommonStyle::styleHint(hint, opt, widget, returnData);
#else
    return QWindowsStyle::styleHint(hint, opt, widget, returnData);
#endif
}

/*! \internal */
QPixmap CommonStyle::standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt, const QWidget *widget) const
{
    QTN_D(const CommonStyle);
    if (d.m_defaultStyle)
        return d.m_defaultStyle->standardPixmap(standardPixmap, opt, widget);
    else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        return QCommonStyle::standardPixmap(standardPixmap, opt, widget);
#else
        return QWindowsStyle::standardPixmap(standardPixmap, opt, widget);
#endif
}

/*! \internal */
void CommonStyle::splitString(const QString& str, QString& strFirstRow, QString& strSecondRow)
{
    strFirstRow = str;
    strSecondRow.clear();

    int nIndex = str.indexOf(QLatin1Char(' '));
    if (nIndex < 1)
        return;

    int nCenter = str.length() / 2;
    for (int i = 0; i < nCenter - 1; i++)
    {
        Q_ASSERT(nCenter - i >= 0 && nCenter + i + 1 < str.length());

        if (str[nCenter - i] == QLatin1Char(' '))
        {
            strFirstRow = str.left(nCenter - i);
            strSecondRow = str.mid(nCenter - i + 1);
            return;
        }

        if (str[nCenter + i + 1] == QLatin1Char(' '))
        {
            strFirstRow = str.left(nCenter + i + 1);
            strSecondRow = str.mid(nCenter + i + 2);
            return;
        }
    }
}

/*! \internal */
QRect CommonStyle::getSource(QRect rcSrc, int nState, int nCount) const
{
    QRect rcImage(0, 0, rcSrc.width(), rcSrc.height() / nCount);
    rcImage.translate(0, nState * rcImage.height());
    return rcImage;
}

/*! \internal */
QPixmap CommonStyle::cached(const QString& img) const
{
    return ::cachedPixmap(img);
}

/*! \internal */
void CommonStyle::clearCache()
{
    QPixmapCache::clear();
}

/*! \internal */
void CommonStyle::drawImagePart(const QPixmap& soSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, bool alpha) const
{
    Q_UNUSED(alpha);
    if (rcDest.width() <= 0 || rcDest.height() <= 0 || rcSrc.width() <= 0 || rcSrc.height() <= 0)
        return;
    p.drawPixmap(rcDest, soSrc, rcSrc);
}

/*! \internal */
void CommonStyle::drawImage(const QPixmap& soSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, 
    QRect rcSizingMargins, QColor clrTransparent, bool alphaBlend) const
{
//    if ((rcSizingMargins.top() + rcSizingMargins.bottom()) > /*rcSrc.height()*/(rcSrc.bottom() - rcSrc.top()))
//        return;

    if ((rcSizingMargins.top() + rcSizingMargins.bottom()) > rcSrc.height()/*(rcSrc.bottom() - rcSrc.top())*/)
        return;

    bool alpha = soSrc.hasAlphaChannel();

    QPixmap copyPix = soSrc.copy();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QImage img = copyPix.toImage();
    img.setAlphaChannel(img.createMaskFromColor(clrTransparent.rgb(), Qt::MaskOutColor));
    copyPix = QPixmap::fromImage(img);
#else
    copyPix.setAlphaChannel(copyPix.createMaskFromColor(clrTransparent, Qt::MaskOutColor));
#endif

    drawPixmap(copyPix, p, rcDest, rcSrc, alpha, rcSizingMargins, alphaBlend);
}

/*! \internal */
void CommonStyle::drawImage(const QPixmap& dcSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, 
    QRect rcSizingMargins, bool alphaBlend) const
{
    bool alpha = dcSrc.hasAlphaChannel();
    QPixmap copyPix = dcSrc.copy();
    drawPixmap(copyPix, p, rcDest, rcSrc, alpha, rcSizingMargins, alphaBlend);
}

static bool qtn_isRectNull(const QRect& rect) { return (rect.left() == 0 && rect.right() == 0 && rect.top() == 0 && rect.bottom() == 0); }

/*! \internal */
void CommonStyle::drawPixmap(const QPixmap& soSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, 
    bool alpha, QRect rcSizingMargins, bool alphaBlend) const
{
    Q_UNUSED(alphaBlend)
    QPixmap dcSrc = soSrc;
/*
    // Draw the pixmap with alpha
    if (alpha)
    {
        dcSrc = helper().alphaBlend(dcSrc);
        if (alphaBlend)
        {
            if (dcSrc.hasAlphaChannel())
                dcSrc.setAlphaChannel(dcSrc.alphaChannel());
        }
    }
*/
    if (rcSizingMargins.top() + rcSizingMargins.bottom() > rcSrc.height())
        return;

    if (dcSrc.isNull())
        return;

    if (rcDest.width() == rcSrc.width()-1)
    {
        rcSizingMargins.setLeft(0);
        rcSizingMargins.setRight(0);
    }

    if (rcDest.height() == rcSrc.height()-1)
    {
        rcSizingMargins.setTop(0);
        rcSizingMargins.setBottom(0);
    }

    QRect rcDestSizingMargins = rcSizingMargins;

    if (rcDest.left() >= rcDest.right() || rcDest.top() >= rcDest.bottom())
        return;


    if (::qtn_isRectNull(rcSizingMargins))
    {
        p.drawPixmap(rcDest, dcSrc, rcSrc);
    }
    else
    {
        class qtn_Rect : public QRect
        {
        public:
            qtn_Rect(int l, int t, int r, int b) { setLeft(l); setRight(r-1); setTop(t); setBottom(b-1); }
        };
        QRect rc = rcDest;
        rc.adjust(0, 0, 1, 1);
        QRect rcImage = rcSrc;
        rcImage.adjust(0, 0, 1, 1);

        // left-top
        drawImagePart(dcSrc, p, qtn_Rect(rc.left(), rc.top(), rc.left() + rcDestSizingMargins.left(), rc.top() + rcDestSizingMargins.top()), 
            qtn_Rect(rcImage.left(), rcImage.top(), rcImage.left() + rcSizingMargins.left(), rcImage.top() + rcSizingMargins.top()), alpha);
        // top-center 
        drawImagePart(dcSrc, p, qtn_Rect(rc.left() + rcDestSizingMargins.left(), rc.top(), rc.right() - rcDestSizingMargins.right(), rc.top() + rcDestSizingMargins.top()), 
            qtn_Rect(rcImage.left() + rcSizingMargins.left(), rcImage.top(), rcImage.right() - rcSizingMargins.right(), rcImage.top() + rcSizingMargins.top()), alpha );
        // right-top 
        drawImagePart(dcSrc, p, qtn_Rect(rc.right() - rcDestSizingMargins.right(), rc.top(), rc.right(), rc.top() + rcDestSizingMargins.top()), 
            qtn_Rect(rcImage.right() - rcSizingMargins.right(), rcImage.top(), rcImage.right(), rcImage.top() + rcSizingMargins.top()), alpha);
        // left-center
        drawImagePart( dcSrc, p, qtn_Rect(rc.left(), rc.top() + rcDestSizingMargins.top(), rc.left() + rcDestSizingMargins.left(), rc.bottom() - rcDestSizingMargins.bottom()), 
            qtn_Rect(rcImage.left(), rcImage.top() + rcSizingMargins.top(), rcImage.left() + rcSizingMargins.left(), rcImage.bottom() - rcSizingMargins.bottom()), alpha );
        // center
        drawImagePart( dcSrc, p, qtn_Rect(rc.left() + rcDestSizingMargins.left(), rc.top() + rcDestSizingMargins.top(), rc.right() - rcDestSizingMargins.right(), rc.bottom() - rcDestSizingMargins.bottom()), 
            qtn_Rect(rcImage.left() + rcSizingMargins.left(), rcImage.top() + rcSizingMargins.top(), rcImage.right() - rcSizingMargins.right(), rcImage.bottom() - rcSizingMargins.bottom()), alpha );
        // right-center
        drawImagePart( dcSrc, p, qtn_Rect(rc.right() - rcDestSizingMargins.right(), rc.top() + rcDestSizingMargins.top(), rc.right(), rc.bottom() - rcDestSizingMargins.bottom()), 
            qtn_Rect(rcImage.right() - rcSizingMargins.right(), rcImage.top() + rcSizingMargins.top(), rcImage.right(), rcImage.bottom() - rcSizingMargins.bottom()), alpha );
        // left-bottom
        drawImagePart( dcSrc, p, qtn_Rect(rc.left(), rc.bottom() - rcDestSizingMargins.bottom(), rc.left() + rcDestSizingMargins.left(), rc.bottom()), 
            qtn_Rect(rcImage.left(), rcImage.bottom() - rcSizingMargins.bottom(), rcImage.left() + rcSizingMargins.left(), rcImage.bottom()), alpha);
        // bottom-center
        drawImagePart( dcSrc, p, qtn_Rect(rc.left() + rcDestSizingMargins.left(), rc.bottom() - rcDestSizingMargins.bottom(), rc.right() - rcDestSizingMargins.right(), rc.bottom()), 
            qtn_Rect(rcImage.left() + rcSizingMargins.left(), rcImage.bottom() - rcSizingMargins.bottom(), rcImage.right() - rcSizingMargins.right(), rcImage.bottom()), alpha );
        // right-bottom
        drawImagePart( dcSrc, p, qtn_Rect(rc.right() - rcDestSizingMargins.right(), rc.bottom() - rcDestSizingMargins.bottom(), rc.right(), rc.bottom()), 
            qtn_Rect(rcImage.right() - rcSizingMargins.right(), rcImage.bottom() - rcSizingMargins.bottom(), rcImage.right(), rcImage.bottom()), alpha );
    }
}

/*! \internal */
void CommonStyle::setDefaultStyle(QStyle* pStyle)
{
    QTN_D(CommonStyle);
    d.m_defaultStyle = pStyle;
}

/*! \internal */
QStyle* CommonStyle::defaultStyle() const
{
    QTN_D(const CommonStyle);
    return d.m_defaultStyle;
}

/*! \internal */
StyleHelper& CommonStyle::helper() const 
{ 
    QTN_D(const CommonStyle);
    return *d.m_helper; 
}

bool CommonStyle::transitionsEnabled() const
{
    return false;
}

/*! \internal */
bool CommonStyle::paintAnimation(TypePaint type, int nPrim, const QStyleOption* option, 
                                 QPainter* p, const QWidget* widget, int iDurationOver) const
{
    QTN_D(const CommonStyle);

    if (!transitionsEnabled())
        return false;

    bool bAnimation = qstyleoption_cast<const QStyleOptionToolButton*>(option) || 
                      qstyleoption_cast<const QStyleOptionButton*>(option)     || 
                      qstyleoption_cast<const QStyleOptionGroupBox*>(option)   ||
                      qstyleoption_cast<const QStyleOptionHeader*>(option)     ||
                      qstyleoption_cast<const QStyleOptionComplex*>(option)    ||
                      qstyleoption_cast<const StyleOptionTitleBar*>(option);
                      qstyleoption_cast<const QStyleOptionFrame*>(option);

    int state = option->state;

    QRect oldRect;
    QRect newRect;

    bool wasDetermined = widget && widget->property("_qtn_isDetermined").toBool();
    if (widget && !wasDetermined && bAnimation) 
    {
        QWidget* w = const_cast<QWidget*> (widget);
        int oldState = w->property("_q_stylestate").toInt();
        oldRect = w->property("_q_stylerect").toRect();
        newRect = w->rect();

        w->setProperty("_q_stylestate", (int)option->state);
        w->setProperty("_q_stylerect", w->rect());
        w->setProperty("_qtn_isDetermined", true);

        // Determine the animated transition
        bool doTransition = ((state & State_Sunken)     != (oldState & State_Sunken) ||
                             (state & State_On)         != (oldState & State_On)     ||
                             (state & State_MouseOver)  != (oldState & State_MouseOver));

        CommonStyle* thisStyle = (CommonStyle*)this;

        if (oldRect != newRect ||
            (state & State_Enabled) != (oldState & State_Enabled) ||
            (state & State_Active)  != (oldState & State_Active))
            thisStyle->qtn_d().stopAnimation(widget);


        if (doTransition) 
        {
            QRect rect(option->rect);
            QImage startImage(rect.size(), QImage::Format_ARGB32_Premultiplied);
            QImage endImage(rect.size(), QImage::Format_ARGB32_Premultiplied);

            QtitanAnimation* anim = d.widgetAnimation(widget);

            startImage.fill(0);
            QtitanTransition* t = new QtitanTransition;
            t->setWidget(w);

            QStyleOption* opt = Q_NULL;
            if (const QStyleOptionComboBox* comboBox = qstyleoption_cast<const QStyleOptionComboBox*>(option)) 
                opt = new QStyleOptionComboBox(*comboBox);
            else if (const StyleSystemToolButton* btn = qstyleoption_cast<const StyleSystemToolButton*>(option)) 
                opt = new StyleSystemToolButton(*btn);
            else if (const QStyleOptionToolButton* button = qstyleoption_cast<const QStyleOptionToolButton*>(option)) 
                opt = new QStyleOptionToolButton(*button);
            else if (const StyleOptionTitleBar* button = qstyleoption_cast<const StyleOptionTitleBar*>(option))
                opt = new StyleOptionTitleBar(*button);
            else if (const QStyleOptionButton* btn = qstyleoption_cast<const QStyleOptionButton*>(option)) 
                opt = new QStyleOptionButton(*btn);
            else if (const QStyleOptionGroupBox* box = qstyleoption_cast<const QStyleOptionGroupBox*>(option))
                opt = new QStyleOptionGroupBox(*box);
            else if (const QStyleOptionHeader* pHeader = qstyleoption_cast<const QStyleOptionHeader*>(option))
                opt = new QStyleOptionHeader(*pHeader);
            else if (const QStyleOptionComplex* pOptionComplex = qstyleoption_cast<const QStyleOptionComplex*>(option))
                opt = new QStyleOptionComplex(*pOptionComplex);
            else if (const QStyleOptionFrame* pOptionComplex = qstyleoption_cast<const QStyleOptionFrame*>(option))
                opt = new QStyleOptionFrame(*pOptionComplex);
            
            opt->rect.setRect(0, 0, rect.width(), rect.height());

            QPainter startPainter(&startImage);
            if (!anim) 
            {
                opt->state = (QStyle::State)oldState;
                switch(type)
                {
                    case tp_PrimitiveElement:
                        drawPrimitive((PrimitiveElement)nPrim, opt, &startPainter, widget);
                        break;
                    case tp_ControlElement:
                        drawControl((ControlElement)nPrim, opt, &startPainter, widget);
                        break;
                    case tp_ComplexControl :
                        {
                            if (const QStyleOptionComplex* pOptionComplex = qstyleoption_cast<const QStyleOptionComplex*>(opt))
                                drawComplexControl((ComplexControl)nPrim, pOptionComplex, &startPainter, widget);
                            break;
                        }
                    default:
                        break;
                }
            }
            else 
            {
                anim->paint(&startPainter, opt);
                thisStyle->qtn_d().stopAnimation(widget);
            }

            t->setStartImage(startImage);
            endImage.fill(0);

            opt->state = (QStyle::State)option->state;

            QPainter endPainter(&endImage);
            switch(type)
            {
                case tp_PrimitiveElement:
                    drawPrimitive((PrimitiveElement)nPrim, opt, &endPainter, widget);
                    break;
                case tp_ControlElement:
                    drawControl((ControlElement)nPrim, opt, &endPainter, widget);
                    break;
                case tp_ComplexControl :
                    {
                        if (const QStyleOptionComplex* pOptionComplex = qstyleoption_cast<const QStyleOptionComplex*>(opt))
                            drawComplexControl((ComplexControl)nPrim, pOptionComplex, &endPainter, widget);
                        break;
                    }
                default:
                    break;
            }

            delete opt;

            t->setEndImage(endImage);

            if (option->state & State_MouseOver || option->state & State_Sunken)
                t->setDuration(iDurationOver);
            else
                t->setDuration(300);

            thisStyle->qtn_d().startAnimation(t);
            t->setStartTime(QTime::currentTime());
        }
        w->setProperty("_qtn_isDetermined", false);
    }

    if (QtitanAnimation* anim = d.widgetAnimation(widget)) 
    {
        anim->paint(p, option);
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStyle::isNativeDialog(const QWidget* wid) const
{
    Q_UNUSED(wid);
    return false;
}

/*! \internal */
bool CommonStyle::showToolTip(const QPoint& pos, QWidget* w)
{
    Q_UNUSED(pos);
    Q_UNUSED(w);
    return false;
}

/*! \reimp */
bool CommonStyle::event(QEvent* e)
{
    QTN_D(CommonStyle);
    switch (e->type()) 
    {
        case QEvent::Timer:
            {
                QTimerEvent* pTimerEvent = (QTimerEvent*)e;
                if (d.m_animationTimer.timerId() == pTimerEvent->timerId()) 
                {
                    d.timerEvent();
                    e->accept();
                    return true;
                }
            }
            break;
        default:
            break;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return QCommonStyle::event(e);
#else
    return QWindowsStyle::event(e);
#endif
}

/*! \reimp */
bool CommonStyle::eventFilter(QObject* watched, QEvent* event)
{
    QTN_D(CommonStyle);
    switch (event->type()) 
    {
        case QEvent::Destroy:
        case QEvent::Create :
            if (QWidget* widget = qobject_cast<QWidget*>(watched))
            {
                d.stopAnimation(widget);
            }
            break;
        case QEvent::ToolTip:
            if (QWidget* pWidget = qobject_cast<QWidget*>(watched))
            {
                if (showToolTip(static_cast<QHelpEvent*>(event)->globalPos(), pWidget))
                    return true;
            }
            break;
        default:
            break;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return QCommonStyle::eventFilter(watched, event);
#else
    return QWindowsStyle::eventFilter(watched, event);
#endif
}


