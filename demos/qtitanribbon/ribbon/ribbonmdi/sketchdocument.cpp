#include <QtitanDef.h>

#include "sketchdocument.h"
#include "sketchobject.h"
#include "sketchrepresentation.h"

/* SketchDocumentPrivate */
class SketchDocumentPrivate
{
public:
    SketchDocumentPrivate(SketchDocument* p);

public:
    void init();
    void clear();

    GeomGrid* getGrid() const;

public:
    QList<GeomObject*> objects_;
    QList<GeomObject*> selectObjects_;
    GeomObject* highlightObject_;
    double width_;
    double height_; 
    bool m_bModified;

protected:
    SketchDocument* pParent_;
};

SketchDocumentPrivate::SketchDocumentPrivate(SketchDocument* p)
{
    pParent_ = p;
    width_ = 0.0;
    height_ = 0.0;
    m_bModified = false;
    highlightObject_ = Q_NULL;
}

void SketchDocumentPrivate::init()
{
    GeomGrid* pObject = new GeomGrid;
    pObject->setVisible(true);
    pParent_->addObject(pObject);
}

void SketchDocumentPrivate::clear()
{
    for (int i = objects_.size()-1; i >= 0; i--) 
    {
        GeomObject* object = objects_[i];
        objects_.removeAt(i);
        delete object;
    }
}

GeomGrid* SketchDocumentPrivate::getGrid() const
{
    for (int i = 0, count = objects_.size(); count > i; i++) 
    {
        if (GeomGrid* pGrid = dynamic_cast<GeomGrid*>(objects_[i]))
            return pGrid;
    }
    return Q_NULL;
}


#undef THIS
#define THIS this->pimpl

/* SketchDocument */
SketchDocument::SketchDocument(QObject* p)
   : QObject(p)
{
    pimpl = new SketchDocumentPrivate(this);
    THIS->init();
}

SketchDocument::~SketchDocument()
{
    THIS->clear();
    delete pimpl;
}

void SketchDocument::addObject(GeomObject* pObject)
{
    THIS->objects_.append(pObject);
    pObject->updateObject();

    if (!dynamic_cast<GeomGrid*>(pObject))
        setModified(true);
}

void SketchDocument::setModified(bool bModified)
{
    THIS->m_bModified = bModified;
    emit contentsChanged();
}

bool SketchDocument::isModified() const
{
    return THIS->m_bModified;
}

void SketchDocument::setVisibleGrid(bool bShow)
{
    if (GeomGrid* pGrid = THIS->getGrid())
        pGrid->setVisible(bShow);
}

void SketchDocument::setStepGrid(double step)
{
    if (GeomGrid* pGrid = THIS->getGrid())
        pGrid->setStepGrid(step);
}

double SketchDocument::getStepGrid() const
{
    if (GeomGrid* pGrid = THIS->getGrid())
        return pGrid->getStepGrid();
    return 0.0;
}

GeomObject* SketchDocument::highlightObject() const
{
    return THIS->highlightObject_;
}

void SketchDocument::setHighlightObject(GeomObject* pObj)
{
    if (THIS->highlightObject_ == pObj)
        return;

    if (THIS->highlightObject_)
        THIS->highlightObject_->setHighlight(false);

    if (pObj)
        pObj->setHighlight(true);

    THIS->highlightObject_ = pObj;
}

const QList<GeomObject*> SketchDocument::selectObjects() const
{
    return THIS->selectObjects_;
}

void SketchDocument::setSelectObject(GeomObject* pObj, bool bMultiSelect)
{
    if (!pObj || !bMultiSelect)
    {
        for (int i = THIS->selectObjects_.size()-1; i >= 0; i--) 
        {
            THIS->selectObjects_[i]->setSelect(false);
            THIS->selectObjects_.removeAt(i);
        }
        if (!pObj)
            return;
    }

    bool bFind = false;
    for (int j = 0, countJ = THIS->selectObjects_.size(); countJ > j && !bFind; j++)
        bFind = THIS->selectObjects_[j] == pObj;

    if (bFind)
    {
        pObj->setSelect(false);
        THIS->selectObjects_.removeOne(pObj);
    }
    else
    {
        pObj->setSelect(true);
        THIS->selectObjects_.append(pObj);
    }
}

void SketchDocument::setSize(double w, double h)
{
    THIS->width_ = w;
    THIS->height_ = h;
    updateObjectSize();
}

const QList<GeomObject*>& SketchDocument::getObjects() const
{
    return THIS->objects_;
}

void SketchDocument::updateObjectSize()
{
    GeomGrid* pGrid = THIS->getGrid();
    if (pGrid)
        pGrid->setSize(THIS->width_, THIS->height_);
}


#undef THIS


