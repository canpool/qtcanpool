#ifndef TOGGLESETTINGSEDITOR_H
#define TOGGLESETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_togglesettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialToggle;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class ToggleSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ToggleSettingsEditor(QWidget *parent = 0);
    ~ToggleSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();
    void selectColor();

private:
    Ui::ToggleSettingsForm *const ui;
    MaterialToggle       *const m_toggle;
};

#endif // TOGGLESETTINGSEDITOR_H
