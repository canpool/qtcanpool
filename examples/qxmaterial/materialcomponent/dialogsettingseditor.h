#ifndef DIALOGSETTINGSEDITOR_H
#define DIALOGSETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_dialogsettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialDialog;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class DialogSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit DialogSettingsEditor(QWidget *parent = 0);
    ~DialogSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();

private:
    Ui::DialogSettingsForm *const ui;
    MaterialDialog       *const m_dialog;
};

#endif // DIALOGSETTINGSEDITOR_H
