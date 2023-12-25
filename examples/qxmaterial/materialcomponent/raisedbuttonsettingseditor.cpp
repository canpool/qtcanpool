#include "raisedbuttonsettingseditor.h"
#include <qxmaterial/materialraisedbutton.h>

QX_MATERIAL_USE_NAMESPACE

RaisedButtonSettingsEditor::RaisedButtonSettingsEditor(QWidget *parent)
    : FlatButtonSettingsEditor(new MaterialRaisedButton("Rise up"), parent)
{
    ui->transparentCheckBox->setDisabled(true);
    ui->defaultPresetPushButton->setDisabled(true);
    ui->checkablePresetPushButton->setDisabled(true);
}

RaisedButtonSettingsEditor::~RaisedButtonSettingsEditor()
{
}
