#ifndef PROGRESSSETTINGSEDITOR_H
#define PROGRESSSETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_progresssettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialProgress;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class ProgressSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressSettingsEditor(QWidget *parent = 0);
    ~ProgressSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();
    void selectColor();

private:
    Ui::ProgressSettingsForm *const ui;
    MaterialProgress       *const m_progress;
};

#endif // PROGRESSSETTINGSEDITOR_H
