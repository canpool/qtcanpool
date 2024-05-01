#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qribbon/ribbonbar.h"
#include <QDialog>

/* MainDialog */
class MainDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MainDialog(QWidget *parent = Q_NULL);
    virtual ~MainDialog();
protected:
    QRibbon::RibbonBar *m_dlgRibbonBar;
private:
    Q_DISABLE_COPY(MainDialog)
};

#endif   // MAINWINDOW_H
