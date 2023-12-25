#ifndef TEXTFIELDSETTINGSEDITOR_H
#define TEXTFIELDSETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_textfieldsettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialTextField;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class TextFieldSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit TextFieldSettingsEditor(QWidget *parent = 0);
    ~TextFieldSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();
    void selectColor();
    void setShowInputLine();

private:
    Ui::TextFieldSettingsForm *const ui;
    MaterialTextField       *const m_textField;
};

#endif // TEXTFIELDSETTINGSEDITOR_H
