#ifndef SKETCHCHILD_H
#define SKETCHCHILD_H

#include <QAbstractScrollArea>
#include "childinterface.h"

class QStatusBar;
namespace Qtitan 
{
    class RibbonBar; 
    class RibbonGroup;
}

class QCheckBox;
class SketchChildPrivate;
class GeometricTool;
class SketchDocument;


/* SketchChild */
class SketchChild : public QAbstractScrollArea,
                    public ChildInterface
{
    Q_OBJECT
    Q_INTERFACES(ChildInterface)
public:
    SketchChild();
    virtual ~SketchChild();

public:
    virtual TypeDocument typeDocument() const { return td_Sketch; }
    virtual void buildUserInterface(Qtitan::RibbonBar* rib, QStatusBar* statusBar, Qtitan::RibbonPage* pageHome);
    virtual void removeUserInterface(Qtitan::RibbonBar* rib, QStatusBar* statusBar, Qtitan::RibbonPage* pageHome);

    void newFile();
    bool loadFile(const QString& fileName);
    virtual bool save();
    virtual bool saveAs();
    bool saveFile(const QString& fileName);

    virtual QString userFriendlyCurrentFile();
    virtual QString currentFile() const { return curFile; }

    virtual bool hasSelection() const;

    SketchDocument* document() const;

protected: // section of the ribbon
    void createActions();
    void updateActions();

    void fillRibbonSketch(Qtitan::RibbonBar* rib);
    void clearRibbonSketch(Qtitan::RibbonBar* rib);

    void createGeometryGroup(Qtitan::RibbonGroup& geomGroup);
    void createEditGroup();
    void clearEditGroup();
    void createGrigGroup();

    void fillStatusBarSketch(QStatusBar* statusBar);
    void clearStatusBarSketch(QStatusBar* statusBar);
    
    void startTool(GeometricTool& tool);
    void stopTool();

private slots:
    void selectionAction();
    void lineAction();
    void rectangleAction();
    void circleAction();
    void ellipseAction();
    void stateChanged(int st);
    void createObject();
    void cyclicalTool();
    void showGrid(int);
    void stepGrid(double);

protected:
    QPointF mapToScene(const QPoint& point) const;
    QPointF mapScreenToMM(const QPointF& point) const;

protected:
    virtual void closeEvent(QCloseEvent*);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
    virtual void mouseDoubleClickEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void keyPressEvent(QKeyEvent*);

protected:
    virtual void paintEvent(QPaintEvent*);
    virtual void resizeEvent(QResizeEvent*);  

private slots:
    void documentWasModified();

private:
    bool maybeSave();
    void setCurrentFile(const QString& fileName);
    QString strippedName(const QString& fullFileName);

protected:
    QString curFile;
    bool isUntitled;

    Qtitan::RibbonGroup* geomGridObject;
    Qtitan::RibbonGroup* geomEditObject;

    QAction* m_selectionAction;
    QAction* m_lineAction;
    QAction* m_rectangleAction;
    QAction* m_circleAction;
    QAction* m_ellipseAction;

    QAction* m_createObjectAction;
    QAction* m_stopToolAction;
    QAction* m_repeatToolAction;


    GeometricTool* m_workTool;

private:
    friend class SketchChildPrivate;
    SketchChildPrivate* pimpl;
};

#endif // SKETCHCHILD_H
