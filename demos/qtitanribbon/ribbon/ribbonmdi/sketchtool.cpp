#include <math.h>
#include <qevent.h>
#include <QApplication>
#include <QLineEdit>
#include <QLabel>
#include <QIcon>
#include <QWidget>

#include <QtitanDef.h>
#include <QtnRibbonGroup.h>

#include "sketchdocument.h"
#include "sketchobject.h"
#include "sketchtool.h"

static double distance(const QPointF& p1, const QPointF& p2)
{
    double x = p1.x() - p2.x();
    double y = p1.y() - p2.y();
    return sqrt (x * x + y * y);
}


/* Cursor */
Cursor::Cursor()
{
}

Cursor::~Cursor()
{
}


/* State */
State::State(const QString& strNameState, const QString& strNextNameState, StateState ss, initFunction init, workFunction work)
{
    m_init = init;
    m_work = work;
    m_bActive = false;
    m_strNameState = strNameState;
    m_strNextNameState = strNextNameState;
    m_pCursor = new Cursor;
    m_stateState = ss;
}

State::~State()
{
    delete m_pCursor;
}


/* GeometricTool */
bool GeometricTool::m_bAutoCreateObject = true;
GeometricTool::GeometricTool(QWidget* pViewPort, SketchDocument* pDoc, GeomObject* pObject/*= Q_NULL*/)
{
    m_pEditableObject = pObject;
    m_pViewPort = pViewPort;
    m_pDocument = pDoc;
    m_pCurrentState = Q_NULL;
    m_bStartTool = false;
}

GeometricTool::~GeometricTool()
{
    for (int i = m_statesTool.size()-1; i >= 0; i--) 
    {
        State* object = m_statesTool[i];
        m_statesTool.removeAt(i);
        delete object;
    }
    delete m_pEditableObject;
}

bool GeometricTool::startTool()
{
    m_oldCursor = m_pViewPort->cursor(); 
    m_bStartTool = true;
    return true;
}

void GeometricTool::stopTool()
{
    if (m_bStartTool)
    {
        m_bStartTool = false;
        m_pViewPort->setCursor(m_oldCursor);
    }
    m_pViewPort->repaint();

    QStatusTipEvent tip(QString(""));
    QApplication::sendEvent(m_pViewPort->parentWidget(), &tip);
}

bool GeometricTool::createObject()
{
    if (m_pCurrentState->getStateState() != ss_readyCreateObject)
        return false;

    m_pDocument->addObject(m_pEditableObject->clone());

    if (m_bAutoCreateObject && m_statesTool.size() > 0)
    {
        delete m_pEditableObject;
        m_pEditableObject = Q_NULL;

        setCurrentState(m_statesTool[0]->nameState());
    }
    m_pViewPort->repaint();

    return true;
}

bool GeometricTool::isReady() const
{
    if (m_pCurrentState)
        return m_pCurrentState->getStateState() == ss_readyCreateObject;
    return false;
}

void GeometricTool::mousePressEvent(double posX, double posY)
{
    if (m_pCurrentState)
    {
        QString nextState = m_pCurrentState->nameNextState();
        if (!nextState.isEmpty())
        {
            State* pState = findState(nextState);
            pState->cursor()->setPosCursor(QPointF(posX, posY));
            setCurrentState(nextState);
        }
    }
}

void GeometricTool::mouseReleaseEvent(double posX, double posY)
{
    Q_UNUSED(posX);
    Q_UNUSED(posY);
}

void GeometricTool::mouseMoveEvent(double posX, double posY)
{
    if (m_pCurrentState && m_pCurrentState->m_work)
    {
        m_pCurrentState->cursor()->setPosCursor(QPointF(posX, posY));
        (this->*(m_pCurrentState->m_work))();
    }
    QStatusTipEvent tip(m_pCurrentState ? m_pCurrentState->getStatusTip() : QString(""));
    QApplication::sendEvent(m_pViewPort->parentWidget(), &tip);
}

State* GeometricTool::addState(QString strNameState, QString strNextNameState, StateState ss, initFunction init, workFunction work)
{
    State* pState = findState(strNameState);
    Q_ASSERT(!pState);
    if (pState)
        return pState;

    pState = new State(strNameState, strNextNameState, ss, init, work);
    m_statesTool.append(pState);
    return pState;
}

void GeometricTool::setCurrentState(const QString& strNameState)
{
    Q_ASSERT(!strNameState.isEmpty());
    if (!m_bStartTool)
        return;

    if (strNameState.isEmpty())
        return;

    m_pCurrentState = Q_NULL;

    int nIndexCurrent = -1;
    for(int i= 0, count = m_statesTool.count(); count > i; i++)
    {
        bool bCurrent = strNameState == m_statesTool[i]->nameState();
        m_statesTool[i]->setActive(bCurrent);
        if (bCurrent)
            nIndexCurrent = i;
    }

    Q_ASSERT(nIndexCurrent != -1);

    if (nIndexCurrent != -1)
    {
        m_pCurrentState = m_statesTool[nIndexCurrent];
        if (m_pCurrentState->m_init)
            (this->*(m_pCurrentState->m_init))();
    }

    if (m_bAutoCreateObject && m_pCurrentState && m_pCurrentState->getStateState() == ss_readyCreateObject)
        createObject();

    setCursorState(m_pCurrentState ? m_pCurrentState->nameState() : QString(""));

    QStatusTipEvent tip(m_pCurrentState ? m_pCurrentState->getStatusTip() : QString(""));
    QApplication::sendEvent(m_pViewPort->parentWidget(), &tip);

    emit stateChanged(m_pCurrentState ? m_pCurrentState->getStateState() : ss_notDefined);
}

State* GeometricTool::currentState() const
{
    return m_pCurrentState;
}

void GeometricTool::setCursorState(const QString& nameState)
{
    Q_UNUSED(nameState)
}

State* GeometricTool::findState(const QString& strNameState)
{
    for(int i= 0, count = m_statesTool.count(); count > i; i++)
    {
        if (strNameState == m_statesTool[i]->nameState())
            return m_statesTool[i];
    }
    return Q_NULL;
}

#define MAX_WIDTH_WIDGET 80
#define LINE dynamic_cast<GeomLine*>(m_pEditableObject)

/* LineTool */
LineTool::LineTool(QWidget* pView, SketchDocument* pDoc, GeomObject* pObject/*= Q_NULL*/)
    : GeometricTool(pView, pDoc, pObject)
{
    Q_UNUSED(pObject);
    State* pState = Q_NULL;
    pState = ADDSTATE("initStart",  "",           ss_notDefined,        EV_INIT(LineTool::initStart),  Q_NULL                       );
    pState = ADDSTATE("findPoint1", "findPoint2", ss_notDefined,        Q_NULL,                        EV_WORK(LineTool::findPoint1));
    pState->setStatusTip(tr("Specify the first point of the segment"));
    pState = ADDSTATE("findPoint2", "ready",      ss_notDefined,        EV_INIT(LineTool::initP2),     EV_WORK(LineTool::findPoint2));
    pState->setStatusTip(tr("Specify the second point of the segment"));
    pState = ADDSTATE("ready",      "",           ss_readyCreateObject, EV_INIT(LineTool::initReady),  Q_NULL                       );
    pState->setStatusTip(tr("Create a circle"));
}

LineTool::~LineTool()
{
}

bool LineTool::startTool()
{
    if (GeometricTool::startTool())
    {
        setCurrentState("initStart");
        return true;
    }
    return false;
}

void LineTool::activate()
{
    m_pLabel1 = new QLabel(tr("Point 1, mm"));
    m_pHolder->addWidget(m_pLabel1);

    m_pEditX1 = new QLineEdit();
    m_pEditX1->setReadOnly(true);
    m_pEditX1->setMaximumWidth(MAX_WIDTH_WIDGET);
    m_pHolder->addWidget(QIcon(), tr("X:"), true, m_pEditX1);

    m_pEditY1 = new QLineEdit();
    m_pEditY1->setReadOnly(true);
    m_pEditY1->setMaximumWidth(MAX_WIDTH_WIDGET);
    m_pHolder->addWidget(QIcon(), tr("Y:"), true, m_pEditY1);

    m_pLabel2 = new QLabel(tr(" Point 2, mm"));
    m_pHolder->addWidget(m_pLabel2);

    m_pEditX2 = new QLineEdit();
    m_pEditX2->setReadOnly(true);
    m_pEditX2->setMaximumWidth(MAX_WIDTH_WIDGET);
    m_pHolder->addWidget(QIcon(), tr(" X:"), true, m_pEditX2);

    m_pEditY2 = new QLineEdit();
    m_pEditY2->setReadOnly(true);
    m_pEditY2->setMaximumWidth(MAX_WIDTH_WIDGET);
    m_pHolder->addWidget(QIcon(), tr(" Y:"), true, m_pEditY2);

    m_pLabel3 = new QLabel(tr(" Length, mm"));
    m_pHolder->addWidget(m_pLabel3);
    m_pLength = new QLineEdit();
    m_pLength->setReadOnly(true);
    m_pLength->setMaximumWidth(MAX_WIDTH_WIDGET);
    m_pHolder->addWidget(QIcon(), tr(" L:"), m_pLength);

    if (LINE)
    {
        m_pEditX1->setText(QString("%1").arg(LINE->getFirstPoint().x()));
        m_pEditY1->setText(QString("%1").arg(LINE->getFirstPoint().y()));

        m_pEditX2->setText(QString("%1").arg(LINE->getSecondPoint().x()));
        m_pEditY2->setText(QString("%1").arg(LINE->getSecondPoint().y()));

        m_pLength->setText(QString("%1").arg(::distance(LINE->getFirstPoint(), LINE->getSecondPoint())));
    }
}

void LineTool::deactivate()
{
    m_pHolder->remove(m_pLabel1);
    delete m_pLabel1;
    m_pLabel1 = Q_NULL;

    m_pHolder->remove(m_pEditX1);
    delete m_pEditX1;
    m_pEditX1 = Q_NULL;

    m_pHolder->remove(m_pEditY1);
    delete m_pEditY1;
    m_pEditY1 = Q_NULL;

    m_pHolder->remove(m_pLabel2);
    delete m_pLabel2;
    m_pLabel2 = Q_NULL;

    m_pHolder->remove(m_pEditX2);
    delete m_pEditX2;
    m_pEditX2 = Q_NULL;

    m_pHolder->remove(m_pEditY2);
    delete m_pEditY2;
    m_pEditY2 = Q_NULL;

    m_pHolder->remove(m_pLabel3);
    delete m_pLabel3;
    m_pLabel3 = Q_NULL;

    m_pHolder->remove(m_pLength);
    delete m_pLength;
    m_pLength = Q_NULL;
}

void LineTool::setCursorState(const QString& nameState)
{
    if (QString("findPoint1") == nameState)
        m_pViewPort->setCursor(QCursor(QPixmap(QLatin1String(":/res/firstCursor.png"))));
    else if (QString("findPoint2") == nameState)
        m_pViewPort->setCursor(QCursor(QPixmap(QLatin1String(":/res/secondCursor.png"))));
    else
        m_pViewPort->setCursor(QCursor(QPixmap(QLatin1String(":/res/findObjectCursor.png"))));
}

bool LineTool::initStart()
{
    Q_ASSERT(!m_pEditableObject);
    m_pEditableObject = new GeomLine();
    LINE->setVisible(false);
    setCurrentState("findPoint1");

    if (m_pEditX1 && m_pEditY1 && m_pEditX2 && m_pEditY2 && m_pLength)
    {
        m_pEditX1->setText(QString(""));
        m_pEditY1->setText(QString(""));

        m_pEditX2->setText(QString(""));
        m_pEditY2->setText(QString(""));
        m_pLength->setText(QString(""));
    }
    return false;
}

void LineTool::findPoint1()
{
    if (m_pEditX1 && m_pEditY1)
    {
        QPointF pos(currentState()->cursor()->posCursor());
        m_pEditX1->setText(QString("%1").arg(pos.x()));
        m_pEditY1->setText(QString("%1").arg(pos.y()));
    }
}

bool LineTool::initP2()
{
    QPointF pos(currentState()->cursor()->posCursor());
    LINE->setFirstPoint(pos);
    LINE->setSecondPoint(pos);
    LINE->setVisible(true);
    m_pViewPort->repaint();
    return true;
}

void LineTool::findPoint2()
{
    QPointF pos(currentState()->cursor()->posCursor());
    LINE->setSecondPoint(pos);

    if (m_pEditX2 && m_pEditY2)
    {
        m_pEditX2->setText(QString("%1").arg(pos.x()));
        m_pEditY2->setText(QString("%1").arg(pos.y()));
    }

    if (m_pLength)
        m_pLength->setText(QString("%1").arg(::distance(LINE->getFirstPoint(), LINE->getSecondPoint())));

    m_pViewPort->repaint();
}

bool LineTool::initReady()
{
    QPointF pos(currentState()->cursor()->posCursor());
    LINE->setSecondPoint(pos);
    m_pViewPort->repaint();
    return true;
}
#undef LINE

#define CIRCLE dynamic_cast<GeomCircle*>(m_pEditableObject)

/* CircleTool */
CircleTool::CircleTool(QWidget* pView, SketchDocument* pDoc, GeomObject* pObject/*= Q_NULL*/)
    : GeometricTool(pView, pDoc, pObject)
{
    Q_UNUSED(pObject);
    State* pState = Q_NULL;
    pState = ADDSTATE("initStart",  "",           ss_notDefined,        EV_INIT(CircleTool::initStart),  Q_NULL                         );
    pState = ADDSTATE("findCenter", "findRadius", ss_notDefined,        Q_NULL,                          EV_WORK(CircleTool::findCenter));
    pState->setStatusTip(tr("Specify the center of the circle"));
    pState = ADDSTATE("findRadius", "ready",      ss_notDefined,        EV_INIT(CircleTool::initCenter), EV_WORK(CircleTool::findRadius ));
    pState->setStatusTip(tr("Specify the radius of the circle"));
    pState = ADDSTATE("ready",      "",           ss_readyCreateObject, EV_INIT(CircleTool::initReady),  Q_NULL                          );
    pState->setStatusTip(tr("Create a circle"));
}

CircleTool::~CircleTool()
{
}

bool CircleTool::startTool()
{
    if (GeometricTool::startTool())
    {
        setCurrentState("initStart");
        return true;
    }
    return false;
}

void CircleTool::activate()
{
    m_pLabel1 = new QLabel(tr("Center, mm"));
    m_pHolder->addWidget(m_pLabel1);

    m_pEditX1 = new QLineEdit();
    m_pEditX1->setReadOnly(true);
    m_pEditX1->setMaximumWidth(MAX_WIDTH_WIDGET);
    m_pHolder->addWidget(QIcon(), tr("X:"), true, m_pEditX1);

    m_pEditY1 = new QLineEdit();
    m_pEditY1->setReadOnly(true);
    m_pEditY1->setMaximumWidth(MAX_WIDTH_WIDGET);
    m_pHolder->addWidget(QIcon(), tr("Y:"), true, m_pEditY1);

    m_pLabel2 = new QLabel(tr(" Radius, mm"));
    m_pHolder->addWidget(m_pLabel2);

    m_pRadius  = new QLineEdit();;
    m_pRadius->setReadOnly(true);
    m_pRadius->setMaximumWidth(MAX_WIDTH_WIDGET);
    m_pHolder->addWidget(QIcon(), tr(" R:"), m_pRadius);

    if (CIRCLE)
    {
        m_pEditX1->setText(QString("%1").arg(CIRCLE->getCenter().x()));
        m_pEditY1->setText(QString("%1").arg(CIRCLE->getCenter().y()));
        m_pRadius->setText(QString("%1").arg(CIRCLE->getRadius()));
    }
}

void CircleTool::deactivate()
{
    m_pHolder->remove(m_pLabel1);
    delete m_pLabel1;
    m_pLabel1 = Q_NULL;

    m_pHolder->remove(m_pEditX1);
    delete m_pEditX1;
    m_pEditX1 = Q_NULL;

    m_pHolder->remove(m_pEditY1);
    delete m_pEditY1;
    m_pEditY1 = Q_NULL;

    m_pHolder->remove(m_pLabel2);
    delete m_pLabel2;
    m_pLabel2 = Q_NULL;

    m_pHolder->remove(m_pRadius);
    delete m_pRadius;
    m_pRadius = Q_NULL;
}

void CircleTool::setCursorState(const QString& nameState)
{
    if (QString("findCenter") == nameState)
        m_pViewPort->setCursor(QCursor(QPixmap(QLatin1String(":/res/centerCursor.png"))));
    else if (QString("findRadius") == nameState)
        m_pViewPort->setCursor(QCursor(QPixmap(QLatin1String(":/res/secondCursor.png"))));
    else
        m_pViewPort->setCursor(QCursor(QPixmap(QLatin1String(":/res/findObjectCursor.png"))));
}

bool CircleTool::initStart()
{
    Q_ASSERT(!m_pEditableObject);
    m_pEditableObject = new GeomCircle();
    CIRCLE->setVisible(false);
    setCurrentState("findCenter");

    if (m_pEditX1 && m_pEditY1 && m_pRadius)
    {
        m_pEditX1->setText(QString(""));
        m_pEditY1->setText(QString(""));

        m_pRadius->setText(QString(""));
    }

    return false;
}

void CircleTool::findCenter()
{
    QPointF pos(currentState()->cursor()->posCursor());
    if (m_pEditX1 && m_pEditY1)
    {
        m_pEditX1->setText(QString("%1").arg(pos.x()));
        m_pEditY1->setText(QString("%1").arg(pos.y()));
    }
}

bool CircleTool::initCenter()
{
    QPointF pos(currentState()->cursor()->posCursor());
    CIRCLE->setCenter(pos);
    CIRCLE->setRadius(::distance(pos, pos));
    CIRCLE->setVisible(true);
    if (m_pRadius)
        m_pRadius->setText(QString("%1").arg(CIRCLE->getRadius()));
    m_pViewPort->repaint();
    return true;
}

void CircleTool::findRadius()
{
    QPointF pos(currentState()->cursor()->posCursor());
    CIRCLE->setRadius(::distance(CIRCLE->getCenter(), pos));
    if (m_pRadius)
        m_pRadius->setText(QString("%1").arg(CIRCLE->getRadius()));
    m_pViewPort->repaint();
}

bool CircleTool::initReady()
{
    QPointF pos(currentState()->cursor()->posCursor());
    CIRCLE->setRadius(::distance(CIRCLE->getCenter(), pos));
    m_pViewPort->repaint();
    return true;
}
#undef CIRCLE



#define RECTANGLE dynamic_cast<GeomRectangle*>(m_pEditableObject)

/* LineTool */
RectangleTool::RectangleTool(QWidget* pView, SketchDocument* pDoc, GeomObject* pObject/*= Q_NULL*/)
    : GeometricTool(pView, pDoc, pObject)
{
    Q_UNUSED(pObject);
    State* pState = Q_NULL;
    pState = ADDSTATE("initStart",  "",           ss_notDefined,        EV_INIT(RectangleTool::initStart),  Q_NULL                            );
    pState = ADDSTATE("findPoint1", "findPoint2", ss_notDefined,        Q_NULL,                             EV_WORK(RectangleTool::findPoint1));
    pState->setStatusTip(tr("Specify the first point of the segment"));
    pState = ADDSTATE("findPoint2", "ready",      ss_notDefined,        EV_INIT(RectangleTool::initP2),     EV_WORK(RectangleTool::findPoint2));
    pState->setStatusTip(tr("Specify the second point of the segment"));
    pState = ADDSTATE("ready",      "",           ss_readyCreateObject, EV_INIT(RectangleTool::initReady),  Q_NULL                            );
    pState->setStatusTip(tr("Create a circle"));
}

RectangleTool::~RectangleTool()
{
}

bool RectangleTool::startTool()
{
    if (GeometricTool::startTool())
    {
        setCurrentState("initStart");
        return true;
    }
    return false;
}

void RectangleTool::setCursorState(const QString& nameState)
{
    if (QString("findPoint1") == nameState)
        m_pViewPort->setCursor(QCursor(QPixmap(QLatin1String(":/res/firstCursor.png"))));
    else if (QString("findPoint2") == nameState)
        m_pViewPort->setCursor(QCursor(QPixmap(QLatin1String(":/res/secondCursor.png"))));
    else
        m_pViewPort->setCursor(QCursor(QPixmap(QLatin1String(":/res/findObjectCursor.png"))));
}

bool RectangleTool::initStart()
{
    Q_ASSERT(!m_pEditableObject);
    m_pEditableObject = new GeomRectangle();
    RECTANGLE->setVisible(false);
    setCurrentState("findPoint1");
    return false;
}

void RectangleTool::findPoint1()
{
    //    QPointF pos(currentState()->cursor()->posCursor());
    //    LINE->setPoint1(pos);
}

bool RectangleTool::initP2()
{
    QPointF pos(currentState()->cursor()->posCursor());
    RECTANGLE->setFirstPoint(pos);
    RECTANGLE->setSecondPoint(pos);
    RECTANGLE->setVisible(true);
    m_pViewPort->repaint();
    return true;
}

void RectangleTool::findPoint2()
{
    QPointF pos(currentState()->cursor()->posCursor());
    RECTANGLE->setSecondPoint(pos);
    m_pViewPort->repaint();
}

bool RectangleTool::initReady()
{
    QPointF pos(currentState()->cursor()->posCursor());
    RECTANGLE->setSecondPoint(pos);
    m_pViewPort->repaint();
    return true;
}
#undef RECTANGLE
