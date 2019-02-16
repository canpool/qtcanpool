#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include <QtnOfficePopupWindow.h>

#include "ui_popupwindowdlg.h"


#define PopupStyleCustom_HTML_Text Qtitan::OfficeStyle::PopupMSNDecoration + 1

/* PopupDemoDialog */
class PopupDemoDialog : public QDialog
{
    Q_OBJECT
public:
    PopupDemoDialog();
    virtual ~PopupDemoDialog();

protected:
    void initDialog();

public slots:
    void currentIndexChangedStyle(int index);
    void showPopup();
    void displayTime(int);
    void animationSpeed(int);
    void transparencylSlider(int);
    void allowMove(bool);
    void aboutToShowPopup();
    void aboutToHidePopup();

protected:
    void setOptions(QVariant lookValue);
    void setEnabledWidgets(bool enabled);

    void fillSystemDecoration(Qtitan::OfficePopupWindow* popup);
    void fillOfficeCompatibleDecoration(Qtitan::OfficePopupWindow* popup);
    void fillOfficeFlatDecoration(Qtitan::OfficePopupWindow* popup);
    void fillMSNDecoration(Qtitan::OfficePopupWindow* popup);
    void fillCustomHTMLTextDecoration(Qtitan::OfficePopupWindow* popup);

private:
    QStyle* m_defaultStyle;
    Qtitan::OfficeStyle* m_officeStyle;

    Ui::PopupDemoDlg ui;
    Qtitan::PopupAnimation m_animType;
    int m_animationSpeed;
    int m_displayTime;
    int m_transparency;
    bool m_dragDropPopup;
};

#endif
