#ifndef SNACKBARSETTINGSEDITOR_H
#define SNACKBARSETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_snackbarsettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialSnackbar;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class SnackbarSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit SnackbarSettingsEditor(QWidget *parent = 0);
    ~SnackbarSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();
    void showSnackbar();

private:
    Ui::SnackbarSettingsForm *const ui;
    MaterialSnackbar       *const m_snackbar;
};

#endif // SNACKBARSETTINGSEDITOR_H
