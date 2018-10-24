/****************************************************************************
**
** Qtitan Library by Developer Machines (Charts and Diagrams for Qt.C++)
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
#include <QDomDocument>
#include <QFile>

#include "QtnChartThemePrivate.h"
#include "QtnChart.h"
#include "QtnChartPalette.h"
#include "QtnChartPainter.h"
#include "QtnChartPrivate.h"


using namespace Qtitan;


/*!
    \inmodule QtitanChart
    \internal
*/
static inline int h2i(char hex)
{
    if (hex >= '0' && hex <= '9')
        return hex - '0';
    if (hex >= 'a' && hex <= 'f')
        return hex - 'a' + 10;
    if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 10;
    return -1;
}

/*!
    \inmodule QtitanChart
    \internal
*/
static inline int hex2int(const char *s)
{
    return (h2i(s[0]) << 4) | h2i(s[1]);
}

/*!
    \inmodule QtitanChart
    \internal
*/
bool qtn_get_hex_rgb(const char *name, QColor& color)
{
    if(name[0] != '#')
        return false;
    name++;

    int len = qstrlen(name);
    int r, g, b, a;
    if (len == 8) 
    {
        a = hex2int(name);
        r = hex2int(name + 2);
        g = hex2int(name + 4);
        b = hex2int(name + 6);
    }
    else if (len == 6)
    {
        a = 255;
        r = hex2int(name);
        g = hex2int(name + 2);
        b = hex2int(name + 4);
    }

    if ((uint)r > 255 || (uint)g > 255 || (uint)b > 255)
        return false;

    color = QColor(r, g, b, a);
    return true;
}

/*!
    \inmodule QtitanChart
    \internal
*/
static QColor parsColor(const QString& name)
{
    QColor color;
    if (!name.isEmpty() && name.startsWith(QLatin1Char('#'))) 
    {
        const QChar* str = name.constData();
        int len = name.length();

        if (len < 10)
        {
            char tmp[16];
            for(int i = 0; i < len; ++i)
                tmp[i] = str[i].toLatin1();
            tmp[len] = 0;

            ::qtn_get_hex_rgb(tmp, color);
        }
    }
    return color;
}

/*!
    \inmodule QtitanChart
    \internal
*/
static bool parsColors(const QString& name, QColor& startColor, QColor& finalColor)
{
    if (!name.isEmpty())
    {
        QStringList strList = name.split(QLatin1Char(';'));
        int count = strList.size();
        if (count <= 2 && count > 0)
        {
            if (count > 0)
                startColor = parsColor(strList.at(0));

            if (count == 2)
                finalColor = parsColor(strList.at(1));

            return true;
        }
    }
    return false;
}


/*!
    \class Qtitan::ChartTitleTheme
    \inmodule QtitanChart
    \internal
*/
ChartTitleTheme::ChartTitleTheme(QObject* parent)
    : QObject(parent)
{
}

void ChartTitleTheme::extractXML(const QDomElement& el)
{
    QDomElement elContent = el.firstChildElement("Title");
    if (!elContent.isNull())
    {
        QString color = elContent.text();
        if (!color.isEmpty())
            m_textColor = QColor(color);
    }
}

void ChartTitleTheme::resetAllElements()
{
    m_textColor = QColor();
}

/*!
    \class Qtitan::ChartLegendTheme
    \inmodule QtitanChart
    \internal
*/
ChartLegendTheme::ChartLegendTheme(QObject* parent)
    : QObject(parent)
{
    m_fillHelper = new ChartFillDrawHelper(this);
    m_round = 0;
}

void ChartLegendTheme::extractXML(const QDomElement& el)
{
    QDomElement elContent = el.firstChildElement("Legend");
    if (!elContent.isNull())
    {
        QDomElement elTextColor = elContent.firstChildElement("TextColor");
        if (!elTextColor.isNull())
        {
            QString color = elTextColor.text();
            if (!color.isEmpty())
                m_textColor = QColor(color);
        }

        QDomElement elBackground = elContent.firstChildElement("Background");
        if (!elBackground.isNull())
        {
            if (m_fillHelper)
                m_fillHelper->extractXML(elBackground);

/*
            QDomElement elBackgroundColor = elBackground.firstChildElement("Color");
            while(!elBackgroundColor.isNull())
            {
                QString positionColor = elBackgroundColor.attribute("position");
                QString color = elBackgroundColor.text();
                m_background.setColorAt(positionColor.toDouble(), QColor(color));

                elBackgroundColor = elBackgroundColor.nextSiblingElement();
            }
*/
        }

        QDomElement elBorder = elContent.firstChildElement("Border");
        if (!elBorder.isNull())
        {
            QDomElement elBorderColor = elBorder.firstChildElement("Color");
            if (!elBorderColor.isNull())
                m_borderColor = QColor(elBorderColor.text());

            QDomElement elBorderRounded = elBorder.firstChildElement("Rounded");
            if (!elBorderRounded.isNull())
            {
                QString xround = elBorderRounded.attribute("round");
                if (!xround.isEmpty())
                    m_round = xround.toInt();
            }
        }
    }
}

void ChartLegendTheme::resetAllElements()
{
    m_round = 0;
    m_textColor = QColor();
//    m_backgroundColor = QColor();
    m_fillHelper->resetAllElements();
    m_borderColor = QColor();
}


/*!
    \class Qtitan::ChartFrameTheme
    \inmodule QtitanChart
    \internal
*/
ChartFrameTheme::ChartFrameTheme(QObject* parent)
    : QObject(parent)
{
    m_titleTheme = new ChartTitleTheme(this);
    m_legendTheme = new ChartLegendTheme(this);
    m_fillHelper = new ChartFillDrawHelper(this);
    m_round = 0;
}

ChartFrameTheme::~ChartFrameTheme()
{
}

ChartTitleTheme* ChartFrameTheme::getTitleTheme() const 
{
    return m_titleTheme;
}

ChartLegendTheme* ChartFrameTheme::getLegendTheme() const 
{
    return m_legendTheme;
}

void ChartFrameTheme::extractXML(const QDomElement& el)
{
    QDomElement elChart = el.firstChildElement("Chart");
    if (!elChart.isNull())
    {
        QDomElement elBackground = elChart.firstChildElement("Background");
        if (!elBackground.isNull())
        {
            if (m_fillHelper)
                m_fillHelper->extractXML(elBackground);
        }

/*
        QDomElement elBackground = elArea.firstChildElement("Background");
        if (!elBackground.isNull())
        {
            QGradient* gradient = Q_NULL;
            QString gradientType = elBackground.attribute("type");
            if (gradientType == "linearGradient")
                gradient = new QLinearGradient;
            else if (gradientType == "radialGradient")
                gradient = new QRadialGradient;

            if (gradient)
            {
                QDomElement elBackgroundColor = elBackground.firstChildElement("Color");
                while(!elBackgroundColor.isNull())
                {
                    QString positionColor = elBackgroundColor.attribute("position");
                    QString color = elBackgroundColor.text();
                    gradient->setColorAt(positionColor.toDouble(), QColor(color));

                    elBackgroundColor = elBackgroundColor.nextSiblingElement();
                }
                m_background = QBrush(*gradient);
                delete gradient;
            }
        }
*/
        QDomElement elBorder = elChart.firstChildElement("Border");
        if (!elBorder.isNull())
        {
            QDomElement elBorderColor = elBorder.firstChildElement("Color");
            if (!elBorderColor.isNull())
                m_borderColor = QColor(elBorderColor.text());

            QDomElement elBorderRounded = elBorder.firstChildElement("Rounded");
            if (!elBorderRounded.isNull())
            {
                QString strRound = elBorderRounded.attribute("round");
                if (!strRound.isEmpty())
                    m_round = strRound.toInt();
            }
        }


        if (m_titleTheme)
            m_titleTheme->extractXML(elChart);

        if (m_legendTheme)
            m_legendTheme->extractXML(elChart);
    }
}

void ChartFrameTheme::resetAllElements()
{
    m_borderColor = QColor();
    m_round = 0;
    m_titleTheme->resetAllElements();
    m_legendTheme->resetAllElements();
    m_fillHelper->resetAllElements();
}

/*!
    \class Qtitan::ChartAxisTheme
    \inmodule QtitanChart
    \internal
*/
ChartAxisTheme::ChartAxisTheme(QObject* parent)
    : QObject(parent)
{
    m_interlacedFillHelper = new ChartFillDrawHelper(this);
}

ChartAxisTheme::~ChartAxisTheme()
{
}

void ChartAxisTheme::extractXML(const QDomElement& el)
{
    QDomElement elContent = el.firstChildElement("Axis");
    if (!elContent.isNull())
    {
        QDomElement elColor = elContent.firstChildElement("Color");
        if (!elColor.isNull())
            m_color = QColor(elColor.text());

        elColor = elContent.firstChildElement("InterlacedColor");
        if (!elColor.isNull())
            parsColors(elColor.text(), m_interlacedColor, m_interlacedColor2);

        QDomElement elContentGridLines = elContent.firstChildElement("GridLines");
        if (!elContentGridLines.isNull())
        {
            elColor = elContentGridLines.firstChildElement("Color");
            if (!elColor.isNull())
                m_gridLinesColor = QColor(elColor.text());

            elColor = elContentGridLines.firstChildElement("MinorColor");
            if (!elColor.isNull())
                m_gridLinesMinorColor = QColor(elColor.text());
        }

        QDomElement elContentAxisLabel = elContent.firstChildElement("AxisLabel");
        if (!elContentAxisLabel.isNull())
        {
            elColor = elContentAxisLabel.firstChildElement("TextColor");
            if (!elColor.isNull())
                m_axisLabelTextColor = QColor(elColor.text());
        }

        QDomElement elContentAxisTitle = elContent.firstChildElement("AxisTitle");
        if (!elContentAxisTitle.isNull())
        {
            elColor = elContentAxisTitle.firstChildElement("TextColor");
            if (!elColor.isNull())
                m_axisTitleTextColor = QColor(elColor.text());
        }

        QDomElement elContentConstantLine = elContent.firstChildElement("ConstantLine");
        if (!elContentConstantLine.isNull())
        {
            elColor = elContentConstantLine.firstChildElement("Color");
            if (!elColor.isNull())
                m_constantLineColor = QColor(elColor.text());

            elColor = elContentConstantLine.firstChildElement("TextColor");
            if (!elColor.isNull())
                m_constantLineTextColor = QColor(elColor.text());
        }

        QDomElement elContentStrip = elContent.firstChildElement("StripColor");
        if (!elContentStrip.isNull())
            parsColors(elContentStrip.text(), m_stripColor, m_stripColor2);
    }
}

void ChartAxisTheme::resetAllElements()
{
    m_color = QColor();
    m_interlacedColor = QColor();
    m_interlacedColor2 = QColor();

    m_gridLinesColor = QColor();
    m_gridLinesMinorColor = QColor();
    m_axisLabelTextColor = QColor();
    m_axisTitleTextColor = QColor();

    m_constantLineColor = QColor();
    m_constantLineTextColor = QColor();

    m_stripColor = QColor();
    m_stripColor2 = QColor();
    m_interlacedFillHelper->resetAllElements();
}

/*!
    \class Qtitan::ChartAreas2DTheme
    \inmodule QtitanChart
    \internal
*/
ChartAreas2DTheme::ChartAreas2DTheme(QObject* parent)
    : QObject(parent)
{
    m_axisTheme = new ChartAxisTheme(this);
    m_backgroundFillHelper = new ChartFillDrawHelper(this);
}

ChartAreas2DTheme::~ChartAreas2DTheme()
{
}

void ChartAreas2DTheme::extractXML(const QDomElement& el)
{
    QDomElement elArea = el.firstChildElement("Chart");
    if (!elArea.isNull())
    {
        QDomElement elContent = elArea.firstChildElement("Area2D");
        if (!elContent.isNull())
        {
            QDomElement elBackground = elContent.firstChildElement("Background");
            if (!elBackground.isNull())
            {
                if (m_backgroundFillHelper)
                    m_backgroundFillHelper->extractXML(elBackground);
            }

            QDomElement elBorder = elContent.firstChildElement("Border");
            if (!elBorder.isNull())
            {
                QDomElement elBorderColor = elBorder.firstChildElement("Color");
                if (!elBorderColor.isNull())
                    m_borderColor = QColor(elBorderColor.text());
            }
            if (m_axisTheme)
                m_axisTheme->extractXML(elContent);
        }
    }
}

void ChartAreas2DTheme::resetAllElements()
{
//    m_background = QBrush();
    m_borderColor = QColor();
    m_axisTheme->resetAllElements();
    m_backgroundFillHelper->resetAllElements();
}

/*!
    \class Qtitan::ChartSeriesLabelTheme
    \inmodule QtitanChart
    \internal
*/
ChartSeriesLabelTheme::ChartSeriesLabelTheme(QObject* parent)
    : QObject(parent)
{
}

ChartSeriesLabelTheme::~ChartSeriesLabelTheme()
{
}

void ChartSeriesLabelTheme::extractXML(const QDomElement& el)
{
    QDomElement elContent = el.firstChildElement("Label");
    if (!elContent.isNull())
    {
        QDomElement elColor = elContent.firstChildElement("BackgroundColor");
        if (!elContent.isNull())
        {
            int alpha = 255;
            QString strAlpha = elColor.attribute("alpha");
            if (!strAlpha.isEmpty())
                alpha = strAlpha.toInt();

            QString strColor = elContent.text();
            if (!strColor.isEmpty())
                m_backgroundColor = QColor(strColor);
            m_backgroundColor.setAlpha(alpha);
        }

        QDomElement elTextColor = elContent.firstChildElement("TextColor");
        if (!elTextColor.isNull())
        {
            QString strColor = elTextColor.text();
            if (!strColor.isEmpty())
                m_textColor = QColor(strColor);
        }

        QDomElement elBorderColor = elContent.firstChildElement("BorderColor");
        if (!elBorderColor.isNull())
        {
            int alpha = 255;
            QString strAlpha = elBorderColor.attribute("alpha");
            if (!strAlpha.isEmpty())
                alpha = strAlpha.toInt();

            QString strColor = elBorderColor.text();
            if (!strColor.isEmpty())
                m_borderColor = QColor(strColor);
            m_borderColor.setAlpha(alpha);
        }

        QDomElement elConnectorColor = elContent.firstChildElement("ConnectorColor");
        if (!elContent.isNull())
        {
            QString strColor = elConnectorColor.text();
            if (!strColor.isEmpty())
                m_connectorColor = QColor(strColor);
        }
/*
        QString strBackgroundColor = elContent.attribute("backgroundColor");
        if (!strBackgroundColor.isEmpty())
            m_backgroundColor = ::stringToColor(strBackgroundColor);
        QString strTextColor = elContent.attribute("textColor");
        if (!strTextColor.isEmpty())
            m_textColor = ::stringToColor(strTextColor);

        QString strBorderColor = elContent.attribute("borderColor");
        if (!strBorderColor.isEmpty())
            m_borderColor = ::stringToColor(strBorderColor);

        QString strConnectorColor = elContent.attribute("connectorColor");
        if (!strConnectorColor.isEmpty())
            m_connectorColor = ::stringToColor(strConnectorColor);
*/
    }
}

void ChartSeriesLabelTheme::resetAllElements()
{
    m_backgroundColor = QColor();
    m_textColor = QColor();
    m_borderColor = QColor();
    m_connectorColor = QColor();
}

/*!
    \class Qtitan::ChartSeriesTheme
    \inmodule QtitanChart
    \internal
*/
ChartSeriesTheme::ChartSeriesTheme(QObject* parent)
    : QObject(parent)
{
    m_labelTheme = new ChartSeriesLabelTheme(this);
}

ChartSeriesTheme::~ChartSeriesTheme()
{
}

ChartSeriesLabelTheme* ChartSeriesTheme::labelTheme() const
{
    return m_labelTheme;
}

void ChartSeriesTheme::extractXML(const QDomElement& el)
{
    QDomElement elContent = el.firstChildElement("SeriesImpl");
    if (!elContent.isNull())
        m_labelTheme->extractXML(elContent);
}

void ChartSeriesTheme::resetAllElements()
{
    m_labelTheme->resetAllElements();
}

/*!
    \class Qtitan::ChartPieSeriesTheme
    \inmodule QtitanChart
    \internal
*/
ChartPieSeriesTheme::ChartPieSeriesTheme(QObject* parent)
    : QObject(parent)
{
}

ChartPieSeriesTheme::~ChartPieSeriesTheme()
{
}

void ChartPieSeriesTheme::extractXML(const QDomElement& el)
{
    QDomElement elContent = el.firstChildElement("PieSeries");
    if (!elContent.isNull())
    {
        QDomElement elPieBorderColor = elContent.firstChildElement("BorderColor");
        if (!elPieBorderColor.isNull())
            m_borderColor = ::parsColor(elPieBorderColor.text());
    }
}

void ChartPieSeriesTheme::resetAllElements()
{
    m_borderColor = QColor();
}


/*!
    \class Qtitan::ChartTheme
    \inmodule QtitanChart
    \internal
*/
ChartTheme::ChartTheme(QObject* parent)
    : QObject(parent)
{
    m_area2DTheme = new ChartAreas2DTheme(this);
    m_frameTheme = new ChartFrameTheme(this);

    m_seriesTheme = new ChartSeriesTheme(this);
    m_pieImplTheme = new ChartPieSeriesTheme(this);
    m_palette = new ChartPalette(this);
    m_currentTheme = Chart::AzureOffice;
    setTheme(Chart::Default);
}

ChartTheme::~ChartTheme()
{
}

ChartAreas2DTheme* ChartTheme::area2DTheme() const 
{
    return m_area2DTheme;
}

ChartFrameTheme* ChartTheme::frameTheme() const 
{
    return m_frameTheme;
}

ChartPalette* ChartTheme::chartPalette() const 
{
    return m_palette;
}

ChartSeriesTheme* ChartTheme::seriesTheme() const 
{
    return m_seriesTheme;
}

ChartPieSeriesTheme* ChartTheme::pieTheme() const 
{
    return m_pieImplTheme;
}

void ChartTheme::setTheme(Chart::Theme theme)
{
    if (m_currentTheme == theme)
        return;

    m_currentTheme = theme;

    switch(m_currentTheme)
    {
        case Chart::Default:
                loadTheme(":/res/ChartTheme/default.xml");
            break;
        case Chart::Office :
                loadTheme(":/res/ChartTheme/office.xml");
            break;
        case Chart::Fire :
                loadTheme(":/res/ChartTheme/fire.xml");
            break;
        case Chart::Sunny :
                loadTheme(":/res/ChartTheme/sunny.xml");
            break;
        case Chart::Blue :
                loadTheme(":/res/ChartTheme/blue.xml");
            break;
        case Chart::Gray:
                loadTheme(":/res/ChartTheme/gray.xml");
            break;
        case Chart::Green:
                loadTheme(":/res/ChartTheme/green.xml");
            break;
        case Chart::AzureOffice:
                loadTheme(":/res/ChartTheme/azureoffice.xml");
            break;
        case Chart::BlueSunny:
                loadTheme(":/res/ChartTheme/bluesunny.xml");
            break;
        case Chart::GreenGreen:
                loadTheme(":/res/ChartTheme/greengreen.xml");
            break;
        case Chart::GrayGray:
                loadTheme(":/res/ChartTheme/graygray.xml");
            break;
        case Chart::BlackBlue:
                loadTheme(":/res/ChartTheme/blackblue.xml");
            break;
        default:
            break;
    }
}

Chart::Theme ChartTheme::theme() const
{
    return m_currentTheme;
}

bool ChartTheme::loadTheme(const QString& theme)
{
    resetAllElements();

    QDomDocument doc("theme");
    QFile file(theme);
    if (doc.setContent(&file))
    {
        extractXML(doc.firstChildElement("Theme"));
        if (QWidget* chart = qobject_cast<QWidget*>(parent()))
            chart->repaint();
        return true;
    }
    return false;
}

ChartTheme* ChartTheme::themeTopParent(const QObject* element)
{
    if (!element)
        return Q_NULL;

    const Chart* chart = qobject_cast<const Chart*>(element);
    if (!chart)
    {
        const QObject* currentObject = element->parent();
        if (currentObject)
        {
            chart = qobject_cast<const Chart*>(currentObject);

            while (chart == Q_NULL)
            {
                currentObject = currentObject->parent();
                if (currentObject)
                    chart = qobject_cast<const Chart*>(currentObject);
            }
        }
    }
    return chart ? chart->qtn_d().m_theme : Q_NULL;
}

QColor ChartTheme::lightColor(const QColor& clr)
{
    return clr.light();
}

void ChartTheme::extractXML(const QDomElement& el)
{
    m_frameTheme->extractXML(el);
    m_area2DTheme->extractXML(el);
    m_seriesTheme->extractXML(el);
    m_pieImplTheme->extractXML(el);

    m_palette->extractXML(el.firstChildElement("SeriePalette"));
}

void ChartTheme::resetAllElements()
{
    m_area2DTheme->resetAllElements();
    m_frameTheme->resetAllElements();
    m_seriesTheme->resetAllElements();
    m_pieImplTheme->resetAllElements();
}

