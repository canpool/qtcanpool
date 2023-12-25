#ifndef FLATBUTTONSETTINGSMANAGER_H
#define FLATBUTTONSETTINGSMANAGER_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_flatbuttonsettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialFlatButton;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class FlatButtonSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit FlatButtonSettingsEditor(QWidget *parent = 0);
    ~FlatButtonSettingsEditor();

protected:
    explicit FlatButtonSettingsEditor(MaterialFlatButton *button, QWidget *parent = 0);

    Ui::FlatButtonSettingsForm *const ui;

protected slots:
    void setupForm();
    void updateWidget();
    void selectColor();
    void applyDefaultPreset();
    void applyCheckablePreset();

private:
    void init();

    MaterialFlatButton *const m_button;
};

#endif // FLATBUTTONSETTINGSMANAGER_H
