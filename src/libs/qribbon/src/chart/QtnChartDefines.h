#ifndef QTN_CHARTDEFINES_H
#define QTN_CHARTDEFINES_H

#include "QPoint"
#include "QtitanDef.h"

namespace Qtitan
{
    #define QTN_VERSION_CHART_STR "2.1"

    #define QTN_CHART_Y_PRIMARY_AXIS        0
    #define QTN_CHART_Y_SECONDARY_AXIS      1
    #define QTN_CHART_X_PRIMARY_AXIS        2
    #define QTN_CHART_X_SECONDARY_AXIS      3
    #define QTN_CHART_Z_PRIMARY_AXIS        4
    #define QTN_CHART_Z_SECONDARY_AXIS      5
    #define QTN_CHART_Y_POLAR_AXIS          6
    #define QTN_CHART_X_POLAR_AXIS          7
    #define QTN_CHART_FIRST_CUSTOM_ID       8

    class Chart;
    /* ChartHitInfo */
    struct QTITAN_EXPORT ChartHitInfo
    {
        enum HitInfoTest
        {
            HitNone = 0x0,
            HitChartArea = 0x2,
            HitAxis = 0x4,
            HitAxisGridLine = 0x8,
            HitLegend = 0x10,
            HitLegendEntry = 0x20,
            HitTitle = 0x40,
            HitAxisName = 0x80,
            HitArea = 0x100,
            HitDataPoint = 0x200,
            HitDataLabel = 0x400,
            HitAxisScrollBar = 0x800,
            HitAxisThumb = 0x1000,
            HitAxisResizeBand = 0x2000,
            HitObject = 0x4000,
            HitAllAxisElements = HitAxis | HitAxisScrollBar | HitAxisThumb | HitAxisResizeBand | HitAxisName,
            HitAllElements = 0xFFFF,
        };

        ChartHitInfo()
        {
            m_index1 = -1;
            m_index2 = -1;
            m_index3 = -1;
            m_hitInfo = ChartHitInfo::HitNone;
            m_chartObject = Q_NULL;
            m_mouseButton = Qt::NoButton;
        }

        ChartHitInfo(const ChartHitInfo& other)
        {
            m_index1 = other.m_index1;
            m_index2 = other.m_index2;
            m_index3 = other.m_index2;
            m_hitInfo = other.m_hitInfo;
            m_chartObject = other.m_chartObject;
            m_mouseButton = other.m_mouseButton;
        }

        ChartHitInfo& operator=( const ChartHitInfo& other)
        {
            m_index1 = other.m_index1;
            m_index2 = other.m_index2;
            m_index3 = other.m_index2;
            m_hitInfo = other.m_hitInfo;
            m_chartObject = other.m_chartObject;
            m_mouseButton = other.m_mouseButton;
            return *this;
        }

        HitInfoTest m_hitInfo;
        int         m_index1;
        int         m_index2;
        int         m_index3;

        Chart* m_chartObject;
        QPoint  m_ptHit;
        Qt::MouseButton m_mouseButton;
    };

    enum DashStyle
    {
        DashStyleEmpty,
        DashStyleSolid,
        DashStyleDash,
        DashStyleDot,
        DashStyleDashDot,
        DashStyleDashDotDot
    };

    enum NearTextPosition
    {
        TextNearLeft,
        TextNearRight,
        TextNearTop,
        TextNearBottom
    };

    enum MarkerType
    {
        MarkerCircle,
        MarkerSquare,
        MarkerDiamond,
        MarkerTriangle,
        MarkerPentagon,
        MarkerHexagon,
        MarkerStar
    };

}; //namespace Qtitan

#endif // QTN_CHARTDEFINES_H

