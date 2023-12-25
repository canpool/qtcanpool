#ifndef APPBARSETTINGSEDITOR_H
#define APPBARSETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_appbarsettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialAppBar;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class AppBarSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit AppBarSettingsEditor(QWidget *parent = 0);
    ~AppBarSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();
    void selectColor();

private:
    Ui::AppBarSettingsForm *const ui;
    MaterialAppBar       *const m_appBar;
};

#endif // APPBARSETTINGSEDITOR_H
