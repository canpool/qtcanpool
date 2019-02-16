#ifndef SKETCHREPRESENTATION_H
#define SKETCHREPRESENTATION_H

#include <QObject>
#include <qpoint.h>

class QPainter;
class QPaintDevice;

/* RepObject */
class RepObject
{
public:
    enum StateObjectDraw
    {
        so_phantom,
        so_normal,
        so_select,
        so_highlight,
    };

public:
    RepObject();
    virtual ~RepObject();

public:
    void setStateObjectDraw(StateObjectDraw state);

public:
    virtual void drawObject(QPainter* p) = 0;

protected:
    QPoint mapToLogic(QPaintDevice* device, QPointF point);

protected:
    StateObjectDraw m_stateObject;

};


/* RepGrid */
class RepGrid : public RepObject
{
public:
    RepGrid(double width, double height, double step);
    virtual ~RepGrid();

public:
    virtual void drawObject(QPainter* p);

private:
    double m_step;
    double m_width; 
    double m_height;
};


/* RepSegmentLine */
class RepSegmentLine : public RepObject
{
public:
    RepSegmentLine(const QPointF& p1, const QPointF& p2);
    virtual ~RepSegmentLine();

public:
    virtual void drawObject(QPainter* p);

private:
    QPointF m_point1;
    QPointF m_point2;
};


/* RepCircle */
class RepCircle : public RepObject
{
public:
    RepCircle(const QPointF& center, double radius);
    virtual ~RepCircle();

public:
    virtual void drawObject(QPainter* p);

private:
    QPointF m_center;
    double m_radius;
};

/* RepRectangle */
class RepRectangle : public RepObject
{
public:
    RepRectangle(const QPointF& p1, const QPointF& p2);
    virtual ~RepRectangle();

public:
    virtual void drawObject(QPainter* p);

private:
    QPointF m_point1;
    QPointF m_point2;
};


#endif // SKETCHREPRESENTATION_H
