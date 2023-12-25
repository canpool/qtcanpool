#ifndef SCROLLBARSETTINGSEDITOR_H
#define SCROLLBARSETTINGSEDITOR_H

#include "qxmaterial/qxmaterial_global.h"
#include <QWidget>
#include "ui_scrollbarsettingsform.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialScrollBar;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class ScrollBarSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ScrollBarSettingsEditor(QWidget *parent = 0);
    ~ScrollBarSettingsEditor();

protected slots:
    void setupForm();
    void updateWidget();

private:
    Ui::ScrollBarSettingsForm *const ui;
    MaterialScrollBar       *const m_verticalScrollbar;
    MaterialScrollBar       *const m_horizontalScrollbar;
};

#endif // SCROLLBARSETTINGSEDITOR_H
