#ifndef SKETCHOBJECT_H
#define SKETCHOBJECT_H

#include <QObject>
#include <qpoint.h>


#define MATH_MAX_LENGTH 5e+007
#define MATH_EPSILON 1e+10

class RepObject;

/* GeomObject */
class GeomObject
{
public:
    GeomObject();
    GeomObject(const GeomObject& other);
    virtual ~GeomObject();

public:
    void setVisible(bool bVisible);
    void setSelect(bool bSelect);
    void setHighlight(bool bHighlight);
    void updateObject();

public:
    const QList<RepObject*>& getRepresentationDraw() const;
    void clearRepresentation();

public:
    virtual GeomObject* clone() const = 0;
    virtual void createRepresentation() = 0;
    // computes the distance between object and the point
    virtual double distance(const QPointF&) const { return MATH_MAX_LENGTH; }

protected:
    QList<RepObject*> representationDrawList;
    bool m_bVisible : 1;
    bool m_bSelect  : 1;
    bool m_bHighlight : 1;
};


/* GeomGrid */
class GeomGrid : public GeomObject
{
public:
    GeomGrid();
    GeomGrid(const GeomGrid& other);
    virtual ~GeomGrid();

public:
    virtual GeomObject* clone() const;

public:
    void setSize(double width, double height);
    void setStepGrid(double step);
    double getStepGrid() const;

public:
    virtual void createRepresentation();

private:
    double m_width; 
    double m_height;
    double m_step;
};


/* GeomLine */
class GeomLine : public GeomObject
{
public:
    GeomLine();
    GeomLine(const QPointF& p1, const QPointF& p2);
    GeomLine(const GeomLine& other);
    virtual ~GeomLine();

public:
    virtual GeomObject* clone() const;

public:
    QPointF getFirstPoint() const { return m_point1; }
    void setFirstPoint(const QPointF& p);

    QPointF getSecondPoint() const { return m_point2; }
    void setSecondPoint(const QPointF& p);

public:
    virtual void createRepresentation();
    // computes the distance between object and the point
    virtual double distance(const QPointF&) const;

protected:
    QPointF m_point1;
    QPointF m_point2;
};

/* GeomCircle */
class GeomCircle : public GeomObject
{
public:
    GeomCircle();
    GeomCircle(const GeomCircle& other);
    virtual ~GeomCircle();

public:
    virtual GeomObject* clone() const;

public:
    void setCenter(const QPointF& center);
    QPointF getCenter() const;

    void setRadius(double r);
    double getRadius() const;

public:
    virtual void createRepresentation();
    // computes the distance between object and the point
    virtual double distance(const QPointF&) const;

protected:
    QPointF m_center;
    double m_radius;
};


/* GeomLine */
class GeomRectangle : public GeomObject
{
public:
    GeomRectangle();
    GeomRectangle(const GeomRectangle& other);
    virtual ~GeomRectangle();

public:
    virtual GeomObject* clone() const;

public:
    QPointF getFirstPoint() const { return m_point1; }
    void setFirstPoint(const QPointF& p);

    QPointF getSecondPoint() const { return m_point2; }
    void setSecondPoint(const QPointF& p);

public:
    virtual void createRepresentation();
    // computes the distance between object and the point
    virtual double distance(const QPointF&) const;

protected:
    QPointF m_point1;
    QPointF m_point2;
};


#endif // SKETCHOBJECT_H
