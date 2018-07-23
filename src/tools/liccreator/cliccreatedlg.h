#ifndef	CLICCREATEDLG_H
#define	CLICCREATEDLG_H

#include <QDialog>
#include <QWidget>

#include "ui_liccreatebase.h"
#include "license/nrlicense.h"

class CLicCreateDlg : public QDialog , public Ui::liccreatebase
{
	Q_OBJECT
public:
	CLicCreateDlg(QWidget * parent = 0, Qt::WindowFlags f = 0);
	~CLicCreateDlg();
	
	void Init();
	void updateComboBoxSoftName(const QString& strName);
	
public slots:
	void accept();
	void onComboBox_monthCurrentIndexChanged(int nIndex);
	void onRadioButton_permanentClicked(bool bChecked);
	void onRadioButton_temporaryClicked(bool bChecked);
	void onComboBox_month_oldCurrentIndexChanged(int nIndex);
	void onRadioButton_permanent_oldClicked(bool bChecked);
	void onRadioButton_temporary_oldClicked(bool bChecked);
	void on_addSoft_clicked();
	void on_configSoft_clicked();
	void onMinimumState();
	void onActiveState();
	void languageChange();

};

#endif
