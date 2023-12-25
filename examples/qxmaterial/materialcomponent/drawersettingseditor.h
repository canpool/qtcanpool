#ifndef DRAWERSETTINGSEDITOR_H
#define DRAWERSETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_drawersettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialDrawer;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class DrawerSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit DrawerSettingsEditor(QWidget *parent = 0);
    ~DrawerSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();

private:
    Ui::DrawerSettingsForm *const ui;
    MaterialDrawer       *const m_drawer;
};

#endif // DRAWERSETTINGSEDITOR_H
