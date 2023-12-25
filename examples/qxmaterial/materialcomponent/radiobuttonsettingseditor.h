#ifndef RADIOBUTTONSETTINGSEDITOR_H
#define RADIOBUTTONSETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_radiobuttonsettingsform.h"

class RadioButton;

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialRadioButton;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class RadioButtonSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit RadioButtonSettingsEditor(QWidget *parent = 0);
    ~RadioButtonSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();
    void selectColor();

private:
    Ui::RadioButtonSettingsForm *const ui;
    MaterialRadioButton       *const m_radioButton1;
    MaterialRadioButton       *const m_radioButton2;
    MaterialRadioButton       *const m_radioButton3;
};

#endif // RADIOBUTTONSETTINGSEDITOR_H
