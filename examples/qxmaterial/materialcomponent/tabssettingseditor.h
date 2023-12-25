#ifndef TABSSETTINGSEDITOR_H
#define TABSSETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_tabssettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialTabBar;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class TabsSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit TabsSettingsEditor(QWidget *parent = 0);
    ~TabsSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();

private:
    Ui::TabsSettingsForm *const ui;
    MaterialTabBar       *const m_tabBar;
};

#endif // TABSSETTINGSEDITOR_H
