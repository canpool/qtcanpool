#ifndef CIRCULARPROGRESSSETTINGSEDITOR_H
#define CIRCULARPROGRESSSETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_circularprogresssettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialCircularProgress;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class CircularProgressSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit CircularProgressSettingsEditor(QWidget *parent = 0);
    ~CircularProgressSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();
    void selectColor();

private:
    Ui::CircularProgressSettingsForm *const ui;
    MaterialCircularProgress       *const m_progress;
};

#endif // CIRCULARPROGRESSSETTINGSEDITOR_H
