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
#ifndef QTN_RIBBONSTYLE_H
#define QTN_RIBBONSTYLE_H

#include <QStylePlugin>

#include "QtnOfficeStyle.h"

#include "QtitanDef.h"

namespace Qtitan
{
    class RibbonStylePrivate;
    /* RibbonStyle */
    class QTITAN_EXPORT RibbonStyle : public OfficeStyle
    {
      Q_OBJECT

    public:
        enum QPixelMetric
        {
            PM_RibbonReducedGroupWidth = PM_CustomBase+1,
            PM_RibbonHeightGroup, 
            PM_RibbonHeightCaptionGroup, 
            PM_RibbonTabsHeight,
        };

        enum QPrimitiveElement
        {
            // PE_CustomBase = 0xf0000000 - base
            PE_RibbonPopupBarButton = PE_QtnCustomBase+1,
            PE_RibbonFileButton,
            PE_RibbonOptionButton,
            PE_RibbonGroupScrollButton,
            PE_RibbonSliderButton,
            PE_RibbonTab,
            PE_RibbonContextHeaders,
            PE_RibbonQuickAccessButton,
            PE_RibbonFillRect,
            PE_RibbonRect,
            PE_RibbonKeyTip,
            PE_Backstage,
        };

        enum QContentsType
        {
            CT_RibbonSliderButton = CT_CustomBase+1,
            CT_RibbonGroupButton,
        };

        enum QControlElement
        {
            CE_RibbonTabShapeLabel = CE_CustomBase + 1,
            CE_RibbonFileButtonLabel,
            CE_RibbonBar,
            CE_RibbonGroups,
            CE_Group,
            CE_ReducedGroup,
            CE_PopupSizeGrip,
        };
        enum QStyleHint
        {
            SH_FlatFrame = SH_CustomBase + 1,
        };

    public:
        RibbonStyle();
        RibbonStyle(QMainWindow* mainWindow);
        virtual ~RibbonStyle();

    public:
        virtual void polish(QApplication*);
        virtual void unpolish(QApplication*);
        virtual void polish(QWidget* widget);
        virtual void unpolish(QWidget* widget);

        virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = 0) const;
        virtual void drawControl(ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* w = 0 ) const;
        virtual void drawComplexControl(ComplexControl cc, const QStyleOptionComplex* option, QPainter* p, const QWidget* widget = 0) const;

        virtual int pixelMetric(PixelMetric pm, const QStyleOption* option = 0, const QWidget* widget = 0) const;
        virtual int styleHint(StyleHint hint, const QStyleOption* opt = Q_NULL, const QWidget* widget = Q_NULL, QStyleHintReturn *returnData = Q_NULL) const;
        virtual QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption* opt = 0, const QWidget* widget = 0) const;
        virtual QSize sizeFromContents(ContentsType ct, const QStyleOption* opt, const QSize& csz, const QWidget* widget) const;
        virtual QRect subControlRect(ComplexControl cc, const QStyleOptionComplex* opt, SubControl sc, const QWidget* widget) const;
        virtual QRect subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *widget) const;

    protected:
        // for QForm
        virtual bool drawFrame(const QStyleOption*, QPainter*, const QWidget*) const;
        virtual bool drawShapedFrame(const QStyleOption*, QPainter*, const QWidget*) const;

        // for stausBar
        virtual bool drawPanelStatusBar(const QStyleOption*, QPainter*, const QWidget*) const;
        // for RibbonBar
        virtual void drawRibbonBar(const QStyleOption* option, QPainter* p, const QWidget* widget) const;
        virtual void drawRibbonGroups(const QStyleOption* option, QPainter* p, const QWidget* widget) const;
        virtual void drawGroup(const QStyleOption* option, QPainter* p, const QWidget* widget) const;
        virtual void drawReducedGroup(const QStyleOption* option, QPainter* p, const QWidget* widget) const;
        // for SizeGrip
        virtual bool drawSizeGrip(const QStyleOption*, QPainter*, const QWidget*) const; 
        // for TitleBar
        virtual bool drawIndicatorToolBarSeparator(const QStyleOption*, QPainter*, const QWidget*) const;
        virtual bool drawTitleBar(const QStyleOptionComplex*, QPainter*, const QWidget*) const;
        virtual void drawContextHeaders(const QStyleOption* opt, QPainter* p) const;

        // for Menu
        virtual bool drawFrameMenu(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
        // for Slider
        virtual bool drawSlider(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const;
        // for file menu
        virtual void drawSystemButton(const QStyleOption* option, QPainter* p, const QWidget* widget) const;
        virtual void drawOptionButton(const QStyleOption* option, QPainter* p, const QWidget* widget) const;
        virtual void drawGroupScrollButton(const QStyleOption* option, QPainter* p, const QWidget* widget) const;
        virtual void drawFileButtonLabel(const QStyleOption* option, QPainter* p, const QWidget* widget) const;
        virtual void drawPopupSystemButton(const QStyleOption* option, QPainter* p, const QWidget* widget) const;
        virtual void drawQuickAccessButton(const QStyleOption* option, QPainter* p, const QWidget* widget) const;
        virtual void drawPopupResizeGripper(const QStyleOption* option, QPainter* p, const QWidget* widget) const;
        virtual bool drawMenuItem(const QStyleOption*, QPainter*, const QWidget*) const;
        // for ribbon tab
        virtual void drawTabShape(const QStyleOption* opt, QPainter* p, const QWidget* widget) const;
        virtual void drawTabShapeLabel(const QStyleOption* opt, QPainter* p, const QWidget* widget) const;
        // for TabBar
        virtual bool drawTabBarTabShape(const QStyleOption*, QPainter*, const QWidget*) const;
        virtual bool drawTabBarTabLabel(const QStyleOption*, QPainter*, const QWidget*) const;
        // for ToolBar
        virtual bool drawToolBar(const QStyleOption*, QPainter*, const QWidget*) const;
        virtual bool drawPanelButtonTool(const QStyleOption*, QPainter*, const QWidget*) const; 
        virtual bool drawToolButtonLabel(const QStyleOption*, QPainter*, const QWidget*) const;
        virtual bool drawToolButton(const QStyleOption*, QPainter*, const QWidget*) const; 
        virtual bool drawGroupControlEntry(const QStyleOption*, QPainter*, const QWidget*) const;
        virtual void drawItemText(QPainter* painter, const QRect& rect, int flags, const QPalette& pal, bool enabled,
                                  const QString& text, QPalette::ColorRole textRole = QPalette::NoRole) const;
        // for BackstageButton
        virtual void drawPanelBackstageButton(const QStyleOption*, QPainter*, const QWidget*) const; 


        virtual bool drawIndicatorArrow(PrimitiveElement, const QStyleOption*, QPainter*, const QWidget*) const;
        virtual void drawRectangle(QPainter* p, const QRect& rect, bool selected, bool pressed, bool enabled, 
                                   bool checked, bool popuped, BarType barType, BarPosition barPos) const;
        virtual void drawSplitButtonPopup(QPainter* p, const QRect& rect, bool selected, bool enabled, bool popuped) const;

        void drawFillRect(const QStyleOption* opt, QPainter* p, const QWidget* widget) const;
        void drawRect(const QStyleOption* opt, QPainter* p, const QWidget* widget) const;
        void drawKeyTip(const QStyleOption* opt, QPainter* p, const QWidget* widget) const;
        void drawBackstage(const QStyleOption* opt, QPainter* p, const QWidget* widget) const;

    protected:
        void calcRects(const QRect& rc, const QSize& pmSize, const QFontMetrics& fMetric, const QString& text,
            bool hasMenu, QRect* pmRect, QRect* strFirsRect, QRect* strSecondRect) const;
        virtual void fillRibbonTabControl(const QStyleOption* opt, QPainter* p, const QWidget* widget) const;
        virtual bool showToolTip(const QPoint& pos, QWidget* w);
        QString getPathForBackstageImages() const;

    protected:
        virtual bool eventFilter(QObject* watched, QEvent* event);

    protected:
        QColor             m_clrRibbonGroupCaptionText;
        // [Ribbon]
        QColor             m_clrRibbonFace;
        QColor             m_clrRibbonText;
        QColor             m_clrGroupClientText;
        QColor             m_clrGroupClientGrayText;
        QColor             m_clrRibbonInactiveFace;
        QColor             m_clrRibbonGrayText;
        QColor             m_clrFileButtonText;
        QColor             m_clrMinimizedFrameEdgeHighLight;
        QColor             m_clrMinimizedFrameEdgeShadow;

    private:
        friend class RibbonProxyStyle;

        QTN_DECLARE_PRIVATE(RibbonStyle)
        Q_DISABLE_COPY(RibbonStyle)
    };

    /* RibbonStylePlugin */
    class RibbonStylePlugin : public QStylePlugin
    {
        Q_OBJECT
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
//      Q_PLUGIN_METADATA(IID "org.qtn-project.Qt.QStyleFactoryInterface" FILE "ribbonstyle.json")
        #endif
    public: // overrides
        QStringList keys() const;
        QStyle* create(const QString& key);
    };

}; //namespace Qtitan

#endif // QTN_RIBBONSTYLE_H
