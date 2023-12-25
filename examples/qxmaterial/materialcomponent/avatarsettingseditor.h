#ifndef AVATARSETTINGSEDITOR_H
#define AVATARSETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_avatarsettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialAvatar;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class AvatarSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit AvatarSettingsEditor(QWidget *parent = 0);
    ~AvatarSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();
    void selectColor();

private:
    Ui::AvatarSettingsForm *const ui;
    MaterialAvatar       *const m_avatar;
};

#endif // AVATARSETTINGSEDITOR_H
