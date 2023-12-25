#ifndef AUTOCOMPLETESETTINGSEDITOR_H
#define AUTOCOMPLETESETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
//#include "ui_autocompletesettingsform.h"

#include "qxmaterial/materialoverlaywidget.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialAutoComplete;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class AutoCompleteSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit AutoCompleteSettingsEditor(QWidget *parent = 0);
    ~AutoCompleteSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();
    void selectColor();

private:
    //Ui::AutoCompleteSettingsForm *const ui;
    MaterialAutoComplete       *const m_autocomplete;
};

#endif // AUTOCOMPLETESETTINGSEDITOR_H
