#ifndef SKETCHDOCUMENT_H
#define SKETCHDOCUMENT_H

#include <QObject>
#include <QSize>
#include <QTransform>


class SketchDocumentPrivate;
class GeomObject;

/* SketchDocument */
class SketchDocument : public QObject
{
    Q_OBJECT
public:
    SketchDocument(QObject* p);
    virtual ~SketchDocument();

public:
    void addObject(GeomObject* pObject);

    void setModified(bool bModified);
    bool isModified() const;

    /* mesh grid */
    void setVisibleGrid(bool bShow);
    void setStepGrid(double step);
    double getStepGrid() const;

    GeomObject* highlightObject() const;
    void setHighlightObject(GeomObject* pObj);

    const QList<GeomObject*> selectObjects() const;
    void setSelectObject(GeomObject* pObj, bool bMultiSelect);

Q_SIGNALS:
    void contentsChanged();

public:
    void setSize(double w, double h);
    const QList<GeomObject*>& getObjects() const;

protected:
    void updateObjectSize();

protected:
    QTransform scaleMatrix_;

private:
    SketchDocumentPrivate* pimpl;
};

#endif // SKETCHDOCUMENT_H
