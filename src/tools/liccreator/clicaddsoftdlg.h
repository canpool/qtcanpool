#ifndef	CLICADDSOFTDLG_H
#define	CLICADDSOFTDLG_H

#include <QDialog>
#include <QWidget>
#include <QEvent>

#include "ui_licaddsoftbase.h"

#define SOFTWARE_FILE "software.txt"

class CLicAddSoftDlg : public QDialog , public Ui::addsoftnamebase
{
	Q_OBJECT
public:
	CLicAddSoftDlg(bool bAdd,QWidget * parent = 0, Qt::WindowFlags f = 0);
	~CLicAddSoftDlg();

public:
	QString m_sRawName;
	bool m_bAdd;

signals:
	void minimumState();
	void activeState();
	void closeState();
	
public slots:
	void on_addButton_clicked();
	void on_cancelButton_clicked();

protected:
	void changeEvent(QEvent* e);
	void languageChange();
	
};

#endif
