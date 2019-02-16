#ifndef DOCKWIDGETS_H
#define DOCKWIDGETS_H

#include <QFrame>


/* StyleDock */
class StyleDock : public QFrame
{
public:
    StyleDock(QWidget* parent = 0);

public:
    void createWidgets();
};

/* TreeDock */
class TreeDock : public QFrame
{
public:
    TreeDock(QWidget* parent = 0);

public:
    void createWidgets();
};


/* ModulesDock */
class ModulesDock : public QFrame
{
public:
    ModulesDock(QWidget* parent = 0);
public:
    void createWidgets();
};


/* ToolBoxDock */
class ToolBoxDock : public QFrame
{
public:
    ToolBoxDock(QWidget* parent = 0);
public:
    void createWidgets();
};


/* TreeBoxDock */
class TreeBoxDock : public QFrame
{
public:
    TreeBoxDock(QWidget* parent = 0);
public:
    void createWidgets();
};


#endif // DOCKWIDGETS_H
