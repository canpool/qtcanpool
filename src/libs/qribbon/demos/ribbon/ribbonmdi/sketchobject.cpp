#include <math.h>

#include <QtitanDef.h>

#include "sketchobject.h"
#include "sketchrepresentation.h"

/* GeomObject */
GeomObject::GeomObject()
    : m_bVisible(false)
    , m_bSelect(false)
    , m_bHighlight(false)
{
}

GeomObject::GeomObject(const GeomObject& other)
    : m_bVisible(other.m_bVisible)
    , m_bSelect(false)
    , m_bHighlight(false)
{
}

GeomObject::~GeomObject()
{
    clearRepresentation();
}

void GeomObject::setVisible(bool bVisible)
{
    m_bVisible = bVisible;
    updateObject();
}

void GeomObject::setSelect(bool bSelect)
{
    m_bSelect = bSelect;
    updateObject();
}

void GeomObject::setHighlight(bool bHighlight)
{
    m_bHighlight = bHighlight;
    updateObject();
}

void GeomObject::updateObject()
{
    if (m_bVisible)
    {
        createRepresentation();
        if (m_bHighlight)
        {
            const QList<RepObject*>& listRepresentation = getRepresentationDraw();
            for (int j = 0, countJ = listRepresentation.size(); countJ > j; j++)
                listRepresentation[j]->setStateObjectDraw(RepObject::so_highlight);
        }
        else if (m_bSelect)
        {
            const QList<RepObject*>& listRepresentation = getRepresentationDraw();
            for (int j = 0, countJ = listRepresentation.size(); countJ > j; j++)
                listRepresentation[j]->setStateObjectDraw(RepObject::so_select);
        } 
        else
        {
            const QList<RepObject*>& listRepresentation = getRepresentationDraw();
            for (int j = 0, countJ = listRepresentation.size(); countJ > j; j++)
                listRepresentation[j]->setStateObjectDraw(RepObject::so_normal);
        }
    }
    else
        clearRepresentation();
}

const QList<RepObject*>& GeomObject::getRepresentationDraw() const
{
    return representationDrawList;
}

void GeomObject::clearRepresentation()
{
    for (int i = representationDrawList.size()-1; i >= 0; i--) 
    {
        RepObject* object = representationDrawList[i];
        representationDrawList.removeAt(i);
        delete object;
    }
}



/* GeomGrid */
GeomGrid::GeomGrid()
    : m_width(0)
    , m_height(0)
    , m_step(5.0)  
{
}

GeomGrid::GeomGrid(const GeomGrid& other)
    : GeomObject(other)
    , m_width(other.m_width)
    , m_height(other.m_height)
    , m_step(other.m_step)  
{
}

GeomGrid::~GeomGrid()
{
}

GeomObject* GeomGrid::clone() const
{
    return new GeomGrid(*this);
}

void GeomGrid::setStepGrid(double step)
{
    m_step = step;
    updateObject();
}

double GeomGrid::getStepGrid() const
{
    return m_step;
}

void GeomGrid::setSize(double w, double h)
{
    m_width = w;
    m_height = h;
    updateObject();
}

void GeomGrid::createRepresentation()
{
    clearRepresentation();
    representationDrawList.append(new RepGrid(m_width, m_height, m_step));
}


/* GeomLine */
GeomLine::GeomLine()
{
}

GeomLine::GeomLine(const QPointF& p1, const QPointF& p2)
    : m_point1(p1)
    , m_point2(p2)
{
}

GeomLine::~GeomLine()
{
}

GeomObject* GeomLine::clone() const
{
    return new GeomLine(*this);
}

GeomLine::GeomLine(const GeomLine& other)
    : GeomObject(other)
    , m_point1(other.m_point1)
    , m_point2(other.m_point2)
{
}

void GeomLine::setFirstPoint(const QPointF& p)
{ 
    m_point1.setX(p.x()); 
    m_point1.setY(p.y()); 
    updateObject();
}

void GeomLine::setSecondPoint(const QPointF& p) 
{ 
    m_point2.setX(p.x()); 
    m_point2.setY(p.y()); 
    updateObject();
}

void GeomLine::createRepresentation()
{
    clearRepresentation();
    if (m_bVisible)
        representationDrawList.append(new RepSegmentLine(m_point1, m_point2));
}

// computes the distance between object and the point
double GeomLine::distance(const QPointF& p) const
{
    QPointF p2(m_point2.x() - m_point1.x(), m_point2.y() - m_point1.y()); 
    double something = p2.x()*p2.x() + p2.y()*p2.y(); 
    double u = ((p.x() - m_point1.x()) * p2.x() + (p.y() - m_point1.y()) * p2.y()) / something; 
    if (u > 1) 
        u = 1; 
    else if (u < 0) 
        u = 0; 
    double x = m_point1.x() + u * p2.x(); 
    double y = m_point1.y() + u * p2.y(); 

    double dx = x - p.x(); 
    double dy = y - p.y();

    double dist = ::sqrt(dx*dx + dy*dy); 
    return dist;
/* for direct
    double numerator = (m_point1.y()-m_point2.y())*p.x() + (m_point2.x()-m_point1.x())*p.y() + (m_point1.x()*m_point2.y()-m_point2.x()*m_point1.y());
    double denominator = sqrt(pow(m_point2.x()-m_point1.x(), 2.0) + pow(m_point2.y()-m_point1.y(), 2.0));
    double dis = fabs(numerator)/denominator;
    return dis;
*/
}

/* GeomCircle */
GeomCircle::GeomCircle()
    : m_center(0.0, 0.0)
    , m_radius(0.0)
{
}

GeomCircle::GeomCircle(const GeomCircle& other)
    : GeomObject(other)
    , m_center(other.m_center)
    , m_radius(other.m_radius)
{
}

GeomCircle::~GeomCircle()
{
}

GeomObject* GeomCircle::clone() const
{
    return new GeomCircle(*this);
}

void GeomCircle::setCenter(const QPointF& center)
{
    m_center = center;
    updateObject();
}

QPointF GeomCircle::getCenter() const
{
    return m_center;
}

void GeomCircle::setRadius(double r)
{
    m_radius = r;
    updateObject();
}

double GeomCircle::getRadius() const
{
    return m_radius;
}

void GeomCircle::createRepresentation()
{
    clearRepresentation();
    if (m_bVisible)
        representationDrawList.append(new RepCircle(m_center, m_radius));
}

// computes the distance between object and the point
double GeomCircle::distance(const QPointF& p) const
{
    double x = m_center.x() - p.x();
    double y = m_center.y() - p.y();
    double t = sqrt(x*x + y*y) - m_radius;
    double dist = t*t;
    return dist;
}


/* GeomLine */
GeomRectangle::GeomRectangle()
{
}

GeomRectangle::~GeomRectangle()
{
}

GeomObject* GeomRectangle::clone() const
{
    return new GeomRectangle(*this);
}

GeomRectangle::GeomRectangle(const GeomRectangle& other)
    : GeomObject(other)
    , m_point1(other.m_point1)
    , m_point2(other.m_point2)
{
}

void GeomRectangle::setFirstPoint(const QPointF& p)
{ 
    m_point1.setX(p.x()); 
    m_point1.setY(p.y()); 
    updateObject();
}

void GeomRectangle::setSecondPoint(const QPointF& p) 
{ 
    m_point2.setX(p.x()); 
    m_point2.setY(p.y()); 
    updateObject();
}

void GeomRectangle::createRepresentation()
{
    clearRepresentation();
    if (m_bVisible)
        representationDrawList.append(new RepRectangle(m_point1, m_point2));
}

// computes the distance between object and the point
double GeomRectangle::distance(const QPointF& p) const
{
    double dist = MATH_MAX_LENGTH;
    GeomLine line1(m_point1, QPointF(m_point1.x(), m_point2.y()));
    dist = qMin(dist, line1.distance(p));

    GeomLine line2(QPointF(m_point1.x(), m_point2.y()), m_point2);
    dist = qMin(dist, line2.distance(p));

    GeomLine line3(m_point2, QPointF(m_point2.x(), m_point1.y()));
    dist = qMin(dist, line3.distance(p));

    GeomLine line4(QPointF(m_point2.x(), m_point1.y()), m_point1);
    dist = qMin(dist, line4.distance(p));
    return dist;
}
