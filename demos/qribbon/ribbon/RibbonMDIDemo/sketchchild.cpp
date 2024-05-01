
#include "sketchchild.h"
#include "qribbon/ribbon.h"
#include "sketchdocument.h"
#include "sketchobject.h"
#include "sketchrepresentation.h"
#include "sketchtool.h"
#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPaintDevice>
#include <QPaintEngine>
#include <QPainter>
#include <QTextStream>

// #define QTN_TEST_ContextTab

/* SketchChildPrivate */
class SketchChildPrivate
{
public:
    SketchChildPrivate(SketchChild *p);
public:
    void init();
public:
    void setActive(bool bActivate);
    SketchDocument *document() const
    {
        return m_document;
    }
public:
    QRibbon::RibbonPage *m_pageHome;
#if defined(QTN_TEST_ContextTab)
    QRibbon::RibbonPage *m_pageContext;
#endif   // QTN_TEST_ContextTab

    QLineEdit *m_editX;
    QLineEdit *m_editY;

    QTransform m_matrix;
    bool m_activeThis;

    double m_aspectX;
    double m_aspectY;

    static int m_currentPageIndex;
protected:
    SketchDocument *m_document;
    SketchChild *m_parent;
};

int SketchChildPrivate::m_currentPageIndex = -1;
SketchChildPrivate::SketchChildPrivate(SketchChild *p)
{
    m_parent = p;
    m_document = Q_NULL;
    m_activeThis = false;
    m_aspectX = 0.0;
    m_aspectY = 0.0;
    m_pageHome = Q_NULL;
#if defined(QTN_TEST_ContextTab)
    m_pageContext = Q_NULL;
#endif   // QTN_TEST_ContextTab

    m_editX = Q_NULL;
    m_editY = Q_NULL;
}

void SketchChildPrivate::init()
{
    m_document = new SketchDocument(m_parent);
    m_aspectX = m_parent->metric(QPaintDevice::PdmDpiX);
    m_aspectY = m_parent->metric(QPaintDevice::PdmDpiY);
}

void SketchChildPrivate::setActive(bool bActivate)
{
    m_activeThis = bActivate;
}

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
    //    delete m_workTool;
    delete pimpl;
}

void SketchChild::buildUserInterface(QRibbon::RibbonBar *rib, QStatusBar *statusBar, QRibbon::RibbonPage *pageHome)
{
    Q_UNUSED(pageHome);
    fillRibbonSketch(rib);
    fillStatusBarSketch(statusBar);

    if (!rib->isMinimized()) {
        int indexPage = SketchChildPrivate::m_currentPageIndex != -1 ? SketchChildPrivate::m_currentPageIndex : 0;
        rib->setCurrentPageIndex(indexPage);
    }
    updateActions();
}

void SketchChild::removeUserInterface(QRibbon::RibbonBar *rib, QStatusBar *statusBar, QRibbon::RibbonPage *pageHome)
{
    Q_UNUSED(pageHome);
    SketchChildPrivate::m_currentPageIndex = rib->currentPageIndex();

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

bool SketchChild::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
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
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
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

SketchDocument *SketchChild::document() const
{
    return pimpl->document();
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

void SketchChild::fillRibbonSketch(QRibbon::RibbonBar *rib)
{
    pimpl->m_pageHome = rib->insertPage(0, tr("&Home"));
    if (pimpl->m_pageHome) {
        createGeometryGroup(pimpl->m_pageHome->addGroup(tr("Geometry")));
        createGrigGroup();
        createEditGroup();
    }

#if defined(QTN_TEST_ContextTab)
    pimpl->m_pageContext = rib->insertPage(1, tr("Test tab"));
    pimpl->m_pageContext->setContextColor(RibbonPage::ContextColorRed);
    pimpl->m_pageContext->setContextTitle(tr("Test tab"));
#endif   // QTN_TEST_ContextTab
}

void SketchChild::clearRibbonSketch(QRibbon::RibbonBar *rib)
{
#if defined(QTN_TEST_ContextTab)
    if (pimpl->m_pageContext) {
        delete pimpl->m_pageContext;
        //        rib->removePage(pimpl->m_pageContext);
        pimpl->m_pageContext = Q_NULL;
    }
#endif   // QTN_TEST_ContextTab

    if (pimpl->m_pageHome) {
        clearEditGroup();
        rib->removePage(pimpl->m_pageHome);
        pimpl->m_pageHome = Q_NULL;
        geomEditObject = Q_NULL;
        geomGridObject = Q_NULL;
    }
}

void SketchChild::createGeometryGroup(QRibbon::RibbonGroup *geomGroup)
{
    geomGroup->setContentAlignment(Qt::AlignVCenter);

    RibbonToolBarControl *toolBar = new RibbonToolBarControl(geomGroup);
    toolBar->addAction(m_selectionAction);
    toolBar->addSeparator();
    toolBar->addAction(m_lineAction);
    toolBar->addAction(m_rectangleAction);
    toolBar->addAction(m_circleAction);
    toolBar->addAction(m_ellipseAction);
    geomGroup->addControl(toolBar);
}

void SketchChild::createEditGroup()
{
    if (m_workTool) {
        if (geomEditObject)
            pimpl->m_pageHome->removeGroup(geomEditObject);

        geomEditObject = pimpl->m_pageHome->insertGroup(1, tr("Edit"));
        if (geomEditObject) {
            m_workTool->setHolder(geomEditObject);

            geomEditObject->addAction(m_createObjectAction);
            RibbonControl *control = geomEditObject->controlByAction(m_createObjectAction);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)
                ->setImageSize(RibbonControlSizeDefinition::ImageSmall);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setLabelVisible(false);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)
                ->setImageSize(RibbonControlSizeDefinition::ImageSmall);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(false);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)
                ->setImageSize(RibbonControlSizeDefinition::ImageSmall);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(false);

            geomEditObject->addAction(m_stopToolAction);
            control = geomEditObject->controlByAction(m_stopToolAction);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)
                ->setImageSize(RibbonControlSizeDefinition::ImageSmall);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setLabelVisible(false);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)
                ->setImageSize(RibbonControlSizeDefinition::ImageSmall);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(false);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)
                ->setImageSize(RibbonControlSizeDefinition::ImageSmall);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(false);

            geomEditObject->addAction(m_repeatToolAction);
            control = geomEditObject->controlByAction(m_repeatToolAction);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)
                ->setImageSize(RibbonControlSizeDefinition::ImageSmall);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setLabelVisible(false);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)
                ->setImageSize(RibbonControlSizeDefinition::ImageSmall);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(false);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)
                ->setImageSize(RibbonControlSizeDefinition::ImageSmall);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(false);
            geomEditObject->addSeparator();

            m_workTool->activate();
        }
    }
}

void SketchChild::clearEditGroup()
{
    if (m_workTool) {
        m_workTool->deactivate();
        if (geomEditObject) {
            geomEditObject->hide();
            geomEditObject->clear();
        }
    }
}

void SketchChild::createGrigGroup()
{
    geomGridObject = pimpl->m_pageHome->addGroup(tr("Grid"));
    if (geomGridObject) {
        QCheckBox *pShowGrid = new QCheckBox(tr("Show grid"));
        connect(pShowGrid, SIGNAL(stateChanged(int)), this, SLOT(showGrid(int)));
        pShowGrid->setCheckState(Qt::Checked);
        geomGridObject->addWidget(pShowGrid);

        QDoubleSpinBox *pStepGrid = new QDoubleSpinBox;
        pStepGrid->setMinimumWidth(100);
        pStepGrid->setSuffix(tr("mm"));
        pStepGrid->setSingleStep(2.5);
        pStepGrid->setRange(1.0, 100.0);
        connect(pStepGrid, SIGNAL(valueChanged(double)), this, SLOT(stepGrid(double)));
        pStepGrid->setValue(document()->getStepGrid());
        geomGridObject->addWidget(QIcon(), tr("Step:"), pStepGrid);
    }
}

void SketchChild::fillStatusBarSketch(QStatusBar *statusBar)
{
    if (pimpl->m_editX && pimpl->m_editY)
        return;

    pimpl->m_editX = new QLineEdit(statusBar);
    pimpl->m_editX->setMaximumWidth(60);
    pimpl->m_editY = new QLineEdit(statusBar);
    pimpl->m_editY->setMaximumWidth(60);

    //    pimpl->m_editX->setEnabled(false);
    //    pimpl->m_editY->setEnabled(false);

    statusBar->addPermanentWidget(pimpl->m_editX);
    statusBar->addPermanentWidget(pimpl->m_editY);
}

void SketchChild::clearStatusBarSketch(QStatusBar *statusBar)
{
    if (pimpl->m_editX && pimpl->m_editY) {
        statusBar->removeWidget(pimpl->m_editX);
        statusBar->removeWidget(pimpl->m_editY);
        delete pimpl->m_editX;
        delete pimpl->m_editY;
        pimpl->m_editX = Q_NULL;
        pimpl->m_editY = Q_NULL;
    }
}

void SketchChild::startTool(GeometricTool &tool)
{
    bool bHasTool = m_workTool && m_workTool->getIdTool() == tool.getIdTool();
    stopTool();

    if (!bHasTool) {
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
    if (m_workTool) {
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
    if (m_workTool) {
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

QPointF SketchChild::mapToScene(const QPoint &point) const
{
    QPointF p = point;
    QTransform mat = pimpl->m_matrix;
    return mat.inverted().map(p);
}

QPointF SketchChild::mapScreenToMM(const QPointF &point) const
{
    if (pimpl->m_aspectX > 0.0 && pimpl->m_aspectY > 0.0)
        return QPointF(25.4 / pimpl->m_aspectX * point.x(), 25.4 / pimpl->m_aspectY * point.y());
    return QPointF(0.0, 0.0);
}

void SketchChild::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

void SketchChild::mousePressEvent(QMouseEvent *event)
{
    QAbstractScrollArea::mousePressEvent(event);
    QPointF point = mapToScene(event->pos());
    point = mapScreenToMM(point);
    if (m_workTool) {
        m_workTool->mousePressEvent(point.x(), point.y());
    } else {
        bool bHasSelect = false;
        const QList<GeomObject *> &objects = document()->getObjects();
        for (int i = 0, count = objects.size(); count > i && !bHasSelect; i++) {
            GeomObject *pObject = objects[i];
            if (pObject->distance(point) <= 4.0) {
                document()->setSelectObject(pObject, event->modifiers() & Qt::ControlModifier);
                viewport()->update();
                bHasSelect = true;
            }
        }
        if (!bHasSelect) {
            document()->setSelectObject(Q_NULL, false);
            viewport()->update();
        }
    }
}

void SketchChild::mouseReleaseEvent(QMouseEvent *event)
{
    QAbstractScrollArea::mouseReleaseEvent(event);
}

void SketchChild::mouseDoubleClickEvent(QMouseEvent *event)
{
    QAbstractScrollArea::mouseDoubleClickEvent(event);
}

void SketchChild::mouseMoveEvent(QMouseEvent *event)
{
    QAbstractScrollArea::mouseMoveEvent(event);

    QPointF point = mapToScene(event->pos());
    point = mapScreenToMM(point);
    if (pimpl->m_editX && pimpl->m_editY) {
        pimpl->m_editX->setText(QString("%1").arg(point.x()));
        pimpl->m_editY->setText(QString("%1").arg(point.y()));
    }

    bool bHighlightObject = false;

    if (m_workTool)
        m_workTool->mouseMoveEvent(point.x(), point.y());
    else {
        const QList<GeomObject *> &objects = document()->getObjects();
        for (int i = 0, count = objects.size(); count > i; i++) {
            GeomObject *pObject = objects[i];
            if (pObject->distance(point) <= 4.0) {
                document()->setHighlightObject(pObject);
                bHighlightObject = true;
                viewport()->update();
            }
        }
    }
    if (!bHighlightObject && document()->highlightObject()) {
        document()->setHighlightObject(Q_NULL);
        viewport()->update();
    }
}

void SketchChild::keyPressEvent(QKeyEvent *event)
{
    QAbstractScrollArea::keyPressEvent(event);
    if (focusWidget() == this && event->key() == Qt::Key_Escape)
        stopTool();
}

void SketchChild::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(viewport());
    p.save();
    QRect r = viewport()->rect();

    QPaintDevice *device = p.paintEngine()->paintDevice();

    pimpl->m_aspectX = device->logicalDpiX();
    pimpl->m_aspectY = device->logicalDpiY();

    pimpl->m_matrix = QTransform().translate(r.width() / 2, r.height() / 2);
    p.setWorldTransform(pimpl->m_matrix, true);

    const QList<GeomObject *> &objects = document()->getObjects();
    for (int i = 0, count = objects.size(); count > i; i++) {
        GeomObject *object = objects[i];
        const QList<RepObject *> &listRepresentation = object->getRepresentationDraw();
        for (int j = 0, countJ = listRepresentation.size(); countJ > j; j++)
            listRepresentation[j]->drawObject(&p);
    }

    if (m_workTool) {
        GeomObject *pObject = m_workTool->getObject();
        if (pObject) {
            const QList<RepObject *> &listRepresentation = pObject->getRepresentationDraw();
            for (int j = 0, countJ = listRepresentation.size(); countJ > j; j++) {
                listRepresentation[j]->setStateObjectDraw(RepObject::so_phantom);
                listRepresentation[j]->drawObject(&p);
            }
        }
    }

    p.restore();
}

void SketchChild::resizeEvent(QResizeEvent *event)
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
    if (document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("MDI"),
                                   tr("'%1' has been modified.\n"
                                      "Do you want to save your changes?")
                                       .arg(userFriendlyCurrentFile()),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void SketchChild::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString SketchChild::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
