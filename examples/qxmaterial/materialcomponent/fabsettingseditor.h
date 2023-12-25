#ifndef FABSETTINGSEDITOR_H
#define FABSETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_fabsettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialFloatingActionButton;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class FloatingActionButtonSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingActionButtonSettingsEditor(QWidget *parent = 0);
    ~FloatingActionButtonSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();
    void selectColor();

private:
    Ui::FloatingActionButtonSettingsForm *const ui;
    MaterialFloatingActionButton       *const m_fab;
};

#endif // FABSETTINGSEDITOR_H
