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
#ifndef QTN_CHARTTITLE_H
#define QTN_CHARTTITLE_H

#include <QColor>
#include <QRect>
#include <QFont>

#include "QtitanDef.h"
#include "QtnChartComponent.h"

namespace Qtitan
{
    enum ChartDockPosition
    {
        TopDock,
        BottomDock,
        LeftDock,
        RightDock
    };

    class ChartTitlePrivate;
    /* ChartTitle */
    class QTITAN_EXPORT ChartTitle : public ChartTextComponent
    {
        Q_OBJECT
        Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
        Q_PROPERTY(ChartDockPosition dockPosition READ dockPosition WRITE setDockPosition)
        Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    public:
        explicit ChartTitle(QObject* parent = Q_NULL);
        virtual ~ChartTitle();

    public:
        bool isVisible() const;

        void setText(const QString& title);
        const QString& text() const;

        void setTextColor(const QColor& txtColor);
        QColor textColor() const;

        void setFont(const QFont& font);
        const QFont& font() const;

        void setDockPosition(ChartDockPosition dockPosition);
        ChartDockPosition dockPosition() const;

        void setIndent(int indent);
        int indent() const;

        void setAlignment(Qt::Alignment alignment);
        Qt::Alignment alignment() const;

        void setAntialiasing(bool antialiasing);
        bool antialiasing() const;

        int textAngle() const;

    public Q_SLOTS:
        void setVisible(bool visible);

    Q_SIGNALS:
        void visibleChanged(bool visible);

    private:
        friend class ChartAreaPrivate;
        friend class Chart2DItem;
        friend class ChartTitleItem;

        Q_DISABLE_COPY(ChartTitle);
        QTN_DECLARE_PRIVATE(ChartTitle)
    };
}; //namespace Qtitan


#endif // QTN_CHARTTITLE_H
