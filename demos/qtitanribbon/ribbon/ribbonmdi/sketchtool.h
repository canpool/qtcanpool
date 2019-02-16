#ifndef SKETCHTOOL_H
#define SKETCHTOOL_H

#include <QObject>
#include <qcursor.h>
#include <qpoint.h>

#include <QtitanDef.h>

class QMouseEvent;

//class SketchChild;
class SketchDocument;
class GeomObject;
class QLineEdit;
class QLabel;

namespace Qtitan 
{
    class RibbonGroup;
}

enum IdTool
{
    it_none,
    it_line,
    it_rectangle,
    it_circle,
    it_ellipse,
};

enum StateState
{
    ss_notDefined,
    ss_readyCreateObject,
};

typedef bool (QObject::*initFunction)();
typedef void (QObject::*workFunction)();

/* Cursor */
class Cursor : public QObject
{
public:
    Cursor();
    virtual ~Cursor();

public:
    QPointF posCursor() const { return m_point; }
    void setPosCursor(QPointF p) { m_point = p; }

protected:
    QPointF m_point;
};


/* State */
class State : public QObject
{
public:
    State(const QString& strNameState, const QString& strNextNameState, StateState, initFunction init, workFunction work);
    virtual ~State();

public:
    bool isActive() const { return m_bActive; } 
    void setActive(bool bActive) { m_bActive = bActive; } 
    
    QString nameState() const { return m_strNameState; }
    QString nameNextState() const { return m_strNextNameState; }

    Cursor* cursor() const { return m_pCursor; }

    StateState getStateState() const { return m_stateState; }

    void setStatusTip(QString tip) { m_strStatusTip = tip; }
    QString getStatusTip() const { return m_strStatusTip; }

public:
    initFunction m_init;
    workFunction m_work;

protected:
    bool m_bActive;
    StateState m_stateState;
    Cursor*  m_pCursor;
    QString  m_strStatusTip;
    QString  m_strNameState;
    QString  m_strNextNameState;
};

/* GeometricTool */
class GeometricTool : public QObject
{
    Q_OBJECT
public:
    GeometricTool(QWidget* pView, SketchDocument* pDoc, GeomObject* pObject = Q_NULL);
    virtual ~GeometricTool();

public:
    GeomObject* getObject() const { return m_pEditableObject; }
    virtual IdTool getIdTool() const = 0;

    virtual bool startTool();
    virtual void stopTool();
    virtual bool createObject();

    virtual bool isReady() const;

    virtual void activate() {}
    virtual void deactivate(){}

public:
    void setAutoCreateObject(bool bAuto) { m_bAutoCreateObject = bAuto; }
    bool isAutoCreateObject() const { return m_bAutoCreateObject; }

    void setHolder(Qtitan::RibbonGroup* pHolder) { m_pHolder = pHolder; }
public:
    void mousePressEvent(double posX, double posY);
    void mouseReleaseEvent(double posX, double posY);
    void mouseMoveEvent(double posX, double posY);

protected:
    State* addState(QString strNameState, QString strNextNameState, StateState ss, initFunction init, workFunction work);

    void setCurrentState(const QString& strNameState);
    State* currentState() const;

    virtual void setCursorState(const QString& nameState);

Q_SIGNALS:
    void stateChanged(int st);

private:
    State* findState(const QString& strNameState);

protected:
    QList<State*> m_statesTool;
    State* m_pCurrentState;

    GeomObject* m_pEditableObject;
    
    QWidget* m_pViewPort; 
    SketchDocument* m_pDocument;
    Qtitan::RibbonGroup* m_pHolder;

    QCursor m_oldCursor;

    bool m_bStartTool;
    static bool m_bAutoCreateObject;
};

#define EV_INIT(func) \
    (initFunction)(&func)

#define EV_WORK(func) \
    (workFunction)(&func)

#define ADDSTATE(name, nameNext, ss, init, work) \
    addState(name, nameNext, ss, init, work)


/* LineTool */
class LineTool : public GeometricTool
{
public:
    LineTool(QWidget* pView, SketchDocument* pDoc, GeomObject* pObject = Q_NULL);
    virtual ~LineTool();

public:
    virtual IdTool getIdTool() const { return it_line; }
    virtual bool startTool();

    virtual void activate();
    virtual void deactivate();

protected:
    virtual void setCursorState(const QString& nameState);

protected:
    bool initStart();

    void findPoint1();

    bool initP2();
    void findPoint2();

    bool initReady();
private:
    QLabel* m_pLabel1;
    QLineEdit* m_pEditX1;
    QLineEdit* m_pEditY1;
    QLabel* m_pLabel2;
    QLineEdit* m_pEditX2;
    QLineEdit* m_pEditY2;
    QLabel* m_pLabel3;
    QLineEdit* m_pLength;
};


/* CircleTool */
class CircleTool : public GeometricTool
{
public:
    CircleTool(QWidget* pView, SketchDocument* pDoc, GeomObject* pObject = Q_NULL);
    virtual ~CircleTool();

public:
    virtual IdTool getIdTool() const { return it_circle; }
    virtual bool startTool();

    virtual void activate();
    virtual void deactivate();

protected:
    virtual void setCursorState(const QString& nameState);

protected:
    bool initStart();

    void findCenter();

    bool initCenter();
    void findRadius();

    bool initReady();

private:
    QLabel* m_pLabel1;
    QLineEdit* m_pEditX1;
    QLineEdit* m_pEditY1;

    QLabel* m_pLabel2;
    QLineEdit* m_pRadius;
};

/* LineTool */
class RectangleTool : public GeometricTool
{
public:
    RectangleTool(QWidget* pView, SketchDocument* pDoc, GeomObject* pObject = Q_NULL);
    virtual ~RectangleTool();

public:
    virtual IdTool getIdTool() const { return it_rectangle; }
    virtual bool startTool();

protected:
    virtual void setCursorState(const QString& nameState);

protected:
    bool initStart();

    void findPoint1();

    bool initP2();
    void findPoint2();

    bool initReady();
};


#endif // SKETCHTOOL_H
