#ifndef BADGESETTINGSEDITOR_H
#define BADGESETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_badgesettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialAvatar;
class MaterialBadge;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class BadgeSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit BadgeSettingsEditor(QWidget *parent = 0);
    ~BadgeSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();
    void selectColor();

private:
    Ui::BadgeSettingsForm *const ui;
    MaterialAvatar      *const m_avatar;
    MaterialBadge       *const m_badge;
};

#endif // BADGESETTINGSEDITOR_H
