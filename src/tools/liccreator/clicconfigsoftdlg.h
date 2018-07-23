#ifndef	CLICCONFIGSOFTDLG_H
#define	CLICCONFIGSOFTDLG_H

#include <QDialog>
#include <QWidget>

#include "ui_licconfigsoftbase.h"

class CLicConfigSoftDlg : public QDialog , public Ui::configsoftbase
{
	Q_OBJECT
public:
	CLicConfigSoftDlg(QWidget * parent = 0, Qt::WindowFlags f = 0);
	~CLicConfigSoftDlg();
	
	void Init();

public:
	QString m_sCurrentSoftName;
	
signals:
	void minimumState();
	void activeState();
	void closeState();

public slots:
	void on_deleteButton_clicked();
	void on_cancelButton_clicked();
	void on_editButton_clicked();
	void on_addButton_clicked();
	void on_tableWidget_cellDoubleClicked(int row, int column);
	void onMinimumState();
	void onActiveState();
	
protected:
	void changeEvent(QEvent* e);
	void languageChange();
	
};

#endif
