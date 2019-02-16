
#include <QApplication>
#include <QLineEdit>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QPaintEngine>
#include <QPaintDevice>
#include <QPainter>

#include <QtitanRibbon.h>

#include "sketchobject.h"
#include "sketchrepresentation.h"

#include "sketchchild.h"
#include "sketchdocument.h"
#include "sketchtool.h"

/* SketchChildPrivate */
class SketchChildPrivate
{
public:
    SketchChildPrivate(SketchChild* p);

public:
    void init();

public:
    void setActive(bool bActivate);
    SketchDocument* document() const { return document_; }

public:
    Qtitan::RibbonPage* pageHome_;

    QLineEdit* editX_;
    QLineEdit* editY_;

    QTransform matrix_;
    bool bActiveThis_;
    
    double aspectX_;
    double aspectY_;

    static int currentIndexPage_;

protected:
    SketchDocument* document_;
    SketchChild* parent_;
};

int SketchChildPrivate::currentIndexPage_ = -1;
SketchChildPrivate::SketchChildPrivate(SketchChild* p)
{
    parent_ = p;
    document_ = Q_NULL;
    bActiveThis_ = false;
    aspectX_ = 0.0;
    aspectY_ = 0.0;
    pageHome_ = Q_NULL;
    editX_ = Q_NULL;
    editY_ = Q_NULL;
}

void SketchChildPrivate::init()
{
    document_ = new SketchDocument(parent_);
    aspectX_ = parent_->metric(QPaintDevice::PdmDpiX);
    aspectY_ = parent_->metric(QPaintDevice::PdmDpiY);
}

void SketchChildPrivate::setActive(bool bActivate)
{
    bActiveThis_ = bActivate;
}



#undef THIS
#define THIS this->pimpl
/* SketchChild */
SketchChild::SketchChild()
{
    geomEditObject = Q_NULL;
    geomGridObject = Q_NULL;

    setWindowIcon(QIcon(":/res/sketchchild.png"));
    m_workTool = Q_NULL;
    pimpl = new SketchChildPrivate(this);
    pimpl->init();

    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_MouseTracking);
    isUntitled = true;

    createActions();
    setCursor(QCursor(QPixmap(QLatin1String(":/res/findObjectCursor.png"))));
}

SketchChild::~SketchChild()
{
    delete pimpl;
}

void SketchChild::buildUserInterface(Qtitan::RibbonBar* rib, QStatusBar* statusBar, Qtitan::RibbonPage* pageHome)
{
    Q_UNUSED(pageHome);
    fillRibbonSketch(rib);
    fillStatusBarSketch(statusBar);
        
    if (!rib->isMinimized())
    {
        int indexPage = THIS->currentIndexPage_ != -1 ? THIS->currentIndexPage_ : 0;
        rib->setCurrentPage(indexPage);
    }
    updateActions();
}

void SketchChild::removeUserInterface(Qtitan::RibbonBar* rib, QStatusBar* statusBar, Qtitan::RibbonPage* pageHome)
{
    Q_UNUSED(pageHome);
    THIS->currentIndexPage_ = rib->currentIndexPage();

    clearRibbonSketch(rib);
    clearStatusBarSketch(statusBar);
}

void SketchChild::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("sketch%1.skt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
}

bool SketchChild::loadFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) 
    {
        QMessageBox::warning(this, tr("MDI"), tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
    return true;
}

bool SketchChild::save()
{
    return isUntitled ? saveAs() : saveFile(curFile);
}

bool SketchChild::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool SketchChild::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) 
    {
        QMessageBox::warning(this, tr("MDI"), tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }

//    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
//    out << toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    return true;
}

QString SketchChild::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

bool SketchChild::hasSelection() const
{
    return false;
}

SketchDocument* SketchChild::document() const
{
    return THIS->document();
}

void SketchChild::createActions()
{
    m_createObjectAction = new QAction(QIcon(":/res/createobject.png"), tr("Create object"), this);
    connect(m_createObjectAction, SIGNAL(triggered()), this, SLOT(createObject()));
    m_stopToolAction = new QAction(QIcon(":/res/stoptool.png"), tr("Stop Tool"), this);
    connect(m_stopToolAction, SIGNAL(triggered()), this, SLOT(selectionAction()));
    m_repeatToolAction = new QAction(QIcon(":/res/repeat.png"), tr("Cyclical tool"), this);
    connect(m_repeatToolAction, SIGNAL(triggered()), this, SLOT(cyclicalTool()));
    m_repeatToolAction->setCheckable(true);

    m_selectionAction = new QAction(QIcon(":/res/selection.png"), tr("Selection Tool"), this);
    m_selectionAction->setStatusTip(tr("Choose object selection tool"));
    connect(m_selectionAction, SIGNAL(triggered()), this, SLOT(selectionAction()));

    m_lineAction = new QAction(QIcon(":/res/line.png"), tr("Line Tool"), this);
    m_lineAction->setStatusTip(tr("Choose line drawing tool"));
    m_lineAction->setCheckable(true);
    connect(m_lineAction, SIGNAL(triggered()), this, SLOT(lineAction()));

    m_rectangleAction = new QAction(QIcon(":/res/rectangle.png"), tr("Rectangle Tool"), this);
    m_rectangleAction->setStatusTip(tr("Choose rectangle drawing tool"));
    m_rectangleAction->setCheckable(true);
    connect(m_rectangleAction, SIGNAL(triggered()), this, SLOT(rectangleAction()));

    m_circleAction = new QAction(QIcon(":/res/circle.png"), tr("Circle Tool"), this);
    m_circleAction->setStatusTip(tr("Choose circle drawing tool"));
    m_circleAction->setCheckable(true);
    connect(m_circleAction, SIGNAL(triggered()), this, SLOT(circleAction()));

    m_ellipseAction = new QAction(QIcon(":/res/ellipse.png"), tr("Ellipse Tool"), this);
    m_ellipseAction->setStatusTip(tr("Choose ellipse drawing tool"));
    m_ellipseAction->setCheckable(true);
    m_ellipseAction->setEnabled(false);
    connect(m_ellipseAction, SIGNAL(triggered()), this, SLOT(ellipseAction()));
}

void SketchChild::updateActions()
{
    m_selectionAction->setEnabled(m_workTool != Q_NULL);
    m_lineAction->setChecked(m_workTool && m_workTool->getIdTool() == it_line);
    m_rectangleAction->setChecked(m_workTool && m_workTool->getIdTool() == it_rectangle);
    m_circleAction->setChecked(m_workTool && m_workTool->getIdTool() == it_circle);
    m_ellipseAction->setChecked(m_workTool && m_workTool->getIdTool() == it_ellipse);

    m_createObjectAction->setEnabled(!m_repeatToolAction->isChecked());
    m_repeatToolAction->setChecked(m_workTool->isAutoCreateObject());
}

void SketchChild::fillRibbonSketch(Qtitan::RibbonBar* rib)
{
    THIS->pageHome_ = rib->insertPage(0, tr("&Home"));
    if (THIS->pageHome_)
    {
        createGeometryGroup(*THIS->pageHome_->addGroup(tr("Geometry")));
        createGrigGroup();
        createEditGroup();
    }
}

void SketchChild::clearRibbonSketch(Qtitan::RibbonBar* rib)
{
    if (THIS->pageHome_)
    {
        clearEditGroup();
        rib->removePage(THIS->pageHome_);
        THIS->pageHome_ = Q_NULL;
        geomEditObject = Q_NULL;
        geomGridObject = Q_NULL;
    }
}

void SketchChild::createGeometryGroup(Qtitan::RibbonGroup& geomGroup)
{
    geomGroup.setControlsGrouping(true);
    geomGroup.setControlsCentering(true);
    geomGroup.addAction(m_selectionAction);
    geomGroup.addSeparator();
    geomGroup.addAction(m_lineAction);
    geomGroup.addAction(m_rectangleAction);
    geomGroup.addAction(m_circleAction);
    geomGroup.addAction(m_ellipseAction);
}

void SketchChild::createEditGroup()
{
    if (m_workTool)
    {
        if (geomEditObject)
            THIS->pageHome_->removeGroup(geomEditObject);

        geomEditObject = THIS->pageHome_->insertGroup(1, tr("Edit"));
        if (geomEditObject)
        {
            m_workTool->setHolder(geomEditObject);

            geomEditObject->addAction(m_createObjectAction);
            geomEditObject->addAction(m_stopToolAction);
            geomEditObject->addAction(m_repeatToolAction);
            geomEditObject->addSeparator();

            m_workTool->activate();
        }
    }
}

void SketchChild::clearEditGroup()
{
    if (m_workTool)
    {
        m_workTool->deactivate();
        if (geomEditObject)
        {
            geomEditObject->clear();
            geomEditObject->hide();
        }
    }
}

void SketchChild::createGrigGroup()
{
    geomGridObject = THIS->pageHome_->addGroup(tr("Grid"));
    if (geomGridObject)
    {
        QCheckBox* pShowGrid = new QCheckBox(tr("Show grid"));
        connect(pShowGrid, SIGNAL(stateChanged(int)), this, SLOT(showGrid(int)));
        pShowGrid->setCheckState(Qt::Checked);
        geomGridObject->addWidget(pShowGrid);

        QDoubleSpinBox* pStepGrid = new QDoubleSpinBox;
        pStepGrid->setMinimumWidth(100);
        pStepGrid->setSuffix(tr("mm"));
        pStepGrid->setSingleStep(2.5);
        pStepGrid->setRange(1.0, 100.0);
        connect(pStepGrid, SIGNAL(valueChanged(double)), this, SLOT(stepGrid(double)));
        pStepGrid->setValue(document()->getStepGrid());
        geomGridObject->addWidget(QIcon(), tr("Step:"), pStepGrid);
    }
}

void SketchChild::fillStatusBarSketch(QStatusBar* statusBar)
{
    if (THIS->editX_ && THIS->editY_)
        return;

    THIS->editX_ = new QLineEdit(statusBar);
    THIS->editX_->setMaximumWidth(60);
    THIS->editY_ = new QLineEdit(statusBar);
    THIS->editY_->setMaximumWidth(60);
    statusBar->addPermanentWidget(THIS->editX_);
    statusBar->addPermanentWidget(THIS->editY_);
}

void SketchChild::clearStatusBarSketch(QStatusBar* statusBar)
{
    if (THIS->editX_ && THIS->editY_)
    {
        statusBar->removeWidget(THIS->editX_);
        statusBar->removeWidget(THIS->editY_);
        delete THIS->editX_;
        delete THIS->editY_;
        THIS->editX_ = Q_NULL;
        THIS->editY_ = Q_NULL;
    }
}

void SketchChild::startTool(GeometricTool& tool)
{
    bool bHasTool = m_workTool && m_workTool->getIdTool() == tool.getIdTool();
    stopTool();

    if (!bHasTool)
    {
        m_workTool = &tool;
        createEditGroup();
        connect(m_workTool, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
        if (!m_workTool->startTool())
            stopTool();
    }
    document()->setSelectObject(Q_NULL, false);
    viewport()->update();
    updateActions();
}

void SketchChild::stopTool()
{
    if (m_workTool)
    {
        clearEditGroup();
        m_workTool->stopTool();
        disconnect(m_workTool, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
        delete m_workTool;
        m_workTool = Q_NULL;
    }
    viewport()->update();
    updateActions();
}

void SketchChild::selectionAction()
{
    stopTool();
}

void SketchChild::lineAction()
{
    startTool(*new LineTool(viewport(), document()));
}

void SketchChild::rectangleAction()
{
    startTool(*new RectangleTool(viewport(), document()));
}

void SketchChild::circleAction()
{
    startTool(*new CircleTool(viewport(), document()));
}

void SketchChild::ellipseAction()
{
    updateActions();
}

void SketchChild::stateChanged(int state)
{
    m_createObjectAction->setEnabled(!m_workTool->isAutoCreateObject() && state == ss_readyCreateObject);
}

void SketchChild::createObject()
{
    if (m_workTool && m_workTool->createObject())
        stopTool();
}

void SketchChild::cyclicalTool()
{
    if (m_workTool)
    {
        bool bIsAuto = !m_workTool->isAutoCreateObject();
        m_repeatToolAction->setChecked(bIsAuto);
        m_workTool->setAutoCreateObject(bIsAuto);

        if (m_workTool)
            m_workTool->createObject();
        updateActions();
    }
}

void SketchChild::showGrid(int state)
{
    document()->setVisibleGrid(state == Qt::Checked);
    viewport()->update();
}

void SketchChild::stepGrid(double val)
{
    document()->setStepGrid(val);
    viewport()->update();
}

QPointF SketchChild::mapToScene(const QPoint& point) const
{
    QPointF p = point;
    QTransform mat = THIS->matrix_;
    return mat.inverted().map(p);
}

QPointF SketchChild::mapScreenToMM(const QPointF& point) const
{
    if ( THIS->aspectX_ > 0.0 && THIS->aspectY_ > 0.0)
        return QPointF(25.4/THIS->aspectX_*point.x(), 25.4/THIS->aspectY_*point.y());
    return QPointF(0.0, 0.0);
}

void SketchChild::closeEvent(QCloseEvent* event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

void SketchChild::mousePressEvent(QMouseEvent* event)
{
    QAbstractScrollArea::mousePressEvent(event);
    QPointF point = mapToScene(event->pos());
    point = mapScreenToMM(point);
    if (m_workTool)
    {
        m_workTool->mousePressEvent(point.x(), point.y());
    }
    else
    {
        bool bHasSelect = false;
        const QList<GeomObject*>& objects = document()->getObjects();
        for (int i = 0, count = objects.size(); count > i && !bHasSelect; i++)
        {
            GeomObject* pObject = objects[i];
            if (pObject->distance(point) <= 4.0) 
            {
                document()->setSelectObject(pObject, event->modifiers() & Qt::ControlModifier);
                viewport()->update();
                bHasSelect = true;
            }
        }
        if (!bHasSelect)
        {
            document()->setSelectObject(Q_NULL, false);
            viewport()->update();
        }
    }
}

void SketchChild::mouseReleaseEvent(QMouseEvent* event)
{
    QAbstractScrollArea::mouseReleaseEvent(event);
}

void SketchChild::mouseDoubleClickEvent(QMouseEvent* event)
{
    QAbstractScrollArea::mouseDoubleClickEvent(event);
}

void SketchChild::mouseMoveEvent(QMouseEvent* event)
{
    QAbstractScrollArea::mouseMoveEvent(event);
    
    QPointF point = mapToScene(event->pos());
    point = mapScreenToMM(point);
    if (THIS->editX_ && THIS->editY_)
    {
        THIS->editX_->setText(QString("%1").arg(point.x()));
        THIS->editY_->setText(QString("%1").arg(point.y()));
    }

    bool bHighlightObject = false;

    if (m_workTool)
        m_workTool->mouseMoveEvent(point.x(), point.y());
    else
    {
        const QList<GeomObject*>& objects = document()->getObjects();
        for (int i = 0, count = objects.size(); count > i; i++)
        {
            GeomObject* pObject = objects[i];
            if (pObject->distance(point) <= 4.0) 
            {
                document()->setHighlightObject(pObject);
                bHighlightObject = true;
                viewport()->update();
            }
        }
    }
    if (!bHighlightObject && document()->highlightObject())
    {
        document()->setHighlightObject(Q_NULL);
        viewport()->update();
    }
}

void SketchChild::keyPressEvent(QKeyEvent* event)
{
    QAbstractScrollArea::keyPressEvent(event);
    if (focusWidget() == this && event->key() == Qt::Key_Escape)
        stopTool();
}

void SketchChild::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(viewport());
    p.save();
    QRect r = viewport()->rect();

    QPaintDevice* device = p.paintEngine()->paintDevice();

    THIS->aspectX_ = device->logicalDpiX();
    THIS->aspectY_ = device->logicalDpiY();

    THIS->matrix_ = QTransform().translate(r.width()/2, r.height()/2);
    p.setWorldTransform(THIS->matrix_, true);

    const QList<GeomObject*>& objects = document()->getObjects();
    for (int i = 0, count = objects.size(); count > i; i++)
    {
        GeomObject* object = objects[i];
        const QList<RepObject*>& listRepresentation = object->getRepresentationDraw();
        for (int j = 0, countJ = listRepresentation.size(); countJ > j; j++)
            listRepresentation[j]->drawObject(&p);
    }

    if (m_workTool)
    {
        GeomObject* pObject = m_workTool->getObject();
        if (pObject)
        {
            const QList<RepObject*>& listRepresentation = pObject->getRepresentationDraw();
            for (int j = 0, countJ = listRepresentation.size(); countJ > j; j++)
            {
                listRepresentation[j]->setStateObjectDraw(RepObject::so_phantom);
                listRepresentation[j]->drawObject(&p);
            }
        }
    }

    p.restore();
}

void SketchChild::resizeEvent(QResizeEvent* event)
{
    QAbstractScrollArea::resizeEvent(event);
    //    QSizeF baseSize = d->matrix.mapRect(sceneRect()).size();
    QRect rc = rect();
    QPointF p = mapScreenToMM(QPointF(rc.width(), rc.height()));
    document()->setSize(p.x(), p.y());
}

void SketchChild::documentWasModified()
{
    setWindowModified(document()->isModified());
}

bool SketchChild::maybeSave()
{
    if (document()->isModified()) 
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("MDI"),
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
            | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void SketchChild::setCurrentFile(const QString& fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString SketchChild::strippedName(const QString& fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
#undef THIS
