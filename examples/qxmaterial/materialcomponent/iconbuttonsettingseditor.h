#ifndef ICONBUTTONSETTINGSMANAGER_H
#define ICONBUTTONSETTINGSMANAGER_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_iconbuttonsettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialIconButton;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class IconButtonSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit IconButtonSettingsEditor(QWidget *parent = 0);
    ~IconButtonSettingsEditor();

protected:
    explicit IconButtonSettingsEditor(MaterialIconButton *button, QWidget *parent = 0);

    Ui::IconButtonSettingsForm *const ui;

protected slots:
    void setupForm();
    void updateWidget();
    void selectColor();

private:
    void init();

    MaterialIconButton *const m_button;
};

#endif // ICONBUTTONSETTINGSMANAGER_H
