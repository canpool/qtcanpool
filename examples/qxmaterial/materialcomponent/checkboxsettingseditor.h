#ifndef CHECKBOXSETTINGSEDITOR_H
#define CHECKBOXSETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_checkboxsettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialCheckable;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class CheckBoxSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit CheckBoxSettingsEditor(QWidget *parent = 0);
    ~CheckBoxSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();
    void selectColor();

private:
    Ui::CheckBoxSettingsForm *const ui;
    MaterialCheckable      *const m_checkBox;
};

#endif // CHECKBOXSETTINGSEDITOR_H
