#include <QtGui>

#include "sketchrepresentation.h"

/* GeometricObject */
RepObject::RepObject()
{
    m_stateObject = so_normal;
}

RepObject::~RepObject()
{
}

void RepObject::setStateObjectDraw(StateObjectDraw state)
{
    m_stateObject = state;
}

QPoint RepObject::mapToLogic(QPaintDevice* device, QPointF point)
{
    int dX = device->logicalDpiX();
    int dY = device->logicalDpiY();
    return QPoint(int(point.x()*dX/25.4), int(point.y()*dY/25.4));
}


/* GeometricGrid */
RepGrid::RepGrid(double w, double h, double step)
{
    m_width = w;
    m_height = h;
    m_step = step;
}

RepGrid::~RepGrid()
{
}

void RepGrid::drawObject(QPainter* p)
{
    QTransform matrixRotate_;
    matrixRotate_.rotate(0.0);

    QPaintDevice* device = p->paintEngine()->paintDevice();
    p->save();
    double width  = m_width;
    double height = m_height;

    QRectF rect;
    rect.setLeft(-width / 2);
    rect.setTop(-height / 2);
    rect.setRight(width);
    rect.setBottom(height);

    const QPen & savePen = p->pen();

    QPen penDash;
    penDash.setStyle(Qt::CustomDashLine);
    penDash.setColor(QColor(0, 0, 128));
    penDash.setWidth(1);
    p->setPen(penDash);

    QPointF ppH1(0, rect.top());
    ppH1 = matrixRotate_.map(ppH1);
    QPointF ppH2(0, rect.bottom());
    ppH2 = matrixRotate_.map(ppH2);
    p->drawLine(mapToLogic(device, ppH1), mapToLogic(device, ppH2));

    QPointF ppV1(rect.left(), 0);
    ppV1 = matrixRotate_.map(ppV1);
    QPointF ppV2(rect.right(), 0);
    ppV2 = matrixRotate_.map(ppV2);
    p->drawLine(mapToLogic(device, ppV1), mapToLogic(device, ppV2));


    QPen penDot;
    penDot.setStyle(Qt::DotLine);
    penDot.setColor(QColor(0, 0, 128));
    penDot.setWidth(1);
    p->setPen(penDot);

    for (double x = 0; x < rect.right(); x += m_step)
    {
        if (x != 0)
        {
            QPoint p1(x, rect.top());
            p1 = matrixRotate_.map(p1);
            QPoint p2(x, rect.bottom());
            p2 = matrixRotate_.map(p2);
            p->drawLine(mapToLogic(device,p1), mapToLogic(device,p2));
        }
    }
    for (double x = 0; x > rect.left(); x -= m_step)
    {
        if (x != 0)
        {
            QPoint p1(x, rect.top());
            p1 = matrixRotate_.map(p1);
            QPoint p2(x, rect.bottom());
            p2 = matrixRotate_.map(p2);
            p->drawLine(mapToLogic(device,p1), mapToLogic(device,p2));
        }
    }

    for (int y = 0; y < rect.bottom(); y += m_step)
    {
        if (y != 0)
        {
            QPoint p1(rect.left(), y);
            p1 = matrixRotate_.map(p1);
            QPoint p2(rect.right(), y);
            p2 = matrixRotate_.map(p2);
            p->drawLine(mapToLogic(device,p1), mapToLogic(device,p2));
        }
    }

    for (int y = 0; y > rect.top(); y -= m_step)
    {
        if (y != 0)
        {
            QPoint p1(rect.left(), y);
            p1 = matrixRotate_.map(p1);
            QPoint p2(rect.right(), y);
            p2 = matrixRotate_.map(p2);
            p->drawLine(mapToLogic(device,p1), mapToLogic(device,p2));
        }
    }

    p->setPen(savePen);
    p->restore();
}

/* RepSegmentLine */ 
RepSegmentLine::RepSegmentLine(const QPointF& p1, const QPointF& p2)
{
    m_point1 = p1;
    m_point2 = p2;
}

RepSegmentLine::~RepSegmentLine()
{
}


void RepSegmentLine::drawObject(QPainter* p)
{
    if (m_point1 != m_point2)
    {
        QPaintDevice* device = p->paintEngine()->paintDevice();
        p->save();

        const QPen & savePen = p->pen();

        QPen penDash;
//        penDash.setStyle(Qt::CustomDashLine);
        QColor color = QColor(0, 0, 255);
        if (m_stateObject ==so_phantom)
            color = QColor(0, 0, 0);
        else if (m_stateObject == so_highlight)
            color = QColor(255, 198, 30);
        else if (m_stateObject == so_select)
            color = QColor(28, 186, 69);

        penDash.setColor(color);
        penDash.setWidth(1);
        p->setPen(penDash);

        p->drawLine(mapToLogic(device,m_point1), mapToLogic(device,m_point2));

        p->setPen(savePen);
        p->restore();
    }
}


/* RepSegmentLine */ 
RepCircle::RepCircle(const QPointF& center, double radius)
{
    m_center = center;
    m_radius = radius;
}

RepCircle::~RepCircle()
{
}


void RepCircle::drawObject(QPainter* p)
{
    if (m_radius > 0)
    {
        QPaintDevice* device = p->paintEngine()->paintDevice();
        p->save();

        const QPen & savePen = p->pen();

        QPen penDash;
        //        penDash.setStyle(Qt::CustomDashLine);
        QColor color = QColor(0, 0, 255);
        if (m_stateObject ==so_phantom)
            color = QColor(0, 0, 0);
        else if (m_stateObject == so_highlight)
            color = QColor(255, 198, 30);
        else if (m_stateObject == so_select)
            color = QColor(28, 186, 69);

        penDash.setColor(color);
        penDash.setWidth(1);
        p->setPen(penDash);
        QPoint point = mapToLogic(device,m_center);

        int dX = device->logicalDpiX();
        int r = (m_radius*dX/25.4);

        int ox = point.x() - ((r*2) / 2);
        int oy = point.y() - ((r*2) / 2);
        int ow = r*2;
        p->drawEllipse(ox,oy,ow,ow);

        p->setPen(savePen);
        p->restore();
    }
}

/* RepRectangle */
RepRectangle::RepRectangle(const QPointF& p1, const QPointF& p2)
{
    m_point1 = p1;
    m_point2 = p2;
}

RepRectangle::~RepRectangle()
{
}

void RepRectangle::drawObject(QPainter* p)
{
    if (m_point1 != m_point2)
    {
        RepSegmentLine line1(m_point1, QPointF(m_point1.x(), m_point2.y()));
        line1.setStateObjectDraw(m_stateObject);
        line1.drawObject(p);

        RepSegmentLine line2(QPointF(m_point1.x(), m_point2.y()), m_point2);
        line2.setStateObjectDraw(m_stateObject);
        line2.drawObject(p);

        RepSegmentLine line3(m_point2, QPointF(m_point2.x(), m_point1.y()));
        line3.setStateObjectDraw(m_stateObject);
        line3.drawObject(p);

        RepSegmentLine line4(QPointF(m_point2.x(), m_point1.y()), m_point1);
        line4.setStateObjectDraw(m_stateObject);
        line4.drawObject(p);
    }
}
