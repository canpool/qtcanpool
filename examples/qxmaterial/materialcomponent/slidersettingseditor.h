#ifndef SLIDERSETTINGSEDITOR_H
#define SLIDERSETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_slidersettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialSlider;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class SliderSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit SliderSettingsEditor(QWidget *parent = 0);
    ~SliderSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();

private:
    Ui::SliderSettingsForm *const ui;
    MaterialSlider       *const m_slider;
};

#endif // SLIDERSETTINGSEDITOR_H
