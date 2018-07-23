#include "cliccreatedlg.h"
#include "clicaddsoftdlg.h"
#include "clicconfigsoftdlg.h"
//#include "../include/nrlicregister.h"

#include <QSize>
#include <QTextCodec>
#include <QDateTime>
#include <QSettings>
#include <QCompleter>

CLicCreateDlg::CLicCreateDlg(QWidget * parent /* = 0 */, Qt::WindowFlags f /* = 0 */)
:QDialog(parent,f)
{
	setupUi(this);

	setFixedSize(minimumSize());

	radioButton_permanent->setChecked(true);
	radioButton_temporary->setChecked(false);
	comboBox_year->setEnabled(false);
	comboBox_month->setEnabled(false);
	comboBox_day->setEnabled(false);

	radioButton_permanent_old->setChecked(true);
	radioButton_temporary_old->setChecked(false);
	comboBox_year_old->setEnabled(false);
	comboBox_month_old->setEnabled(false);
	comboBox_day_old->setEnabled(false);

	connect(radioButton_permanent,SIGNAL(clicked(bool)),this,SLOT(onRadioButton_permanentClicked(bool)));
	connect(radioButton_temporary,SIGNAL(clicked(bool)),this,SLOT(onRadioButton_temporaryClicked(bool)));
	connect(comboBox_month,SIGNAL(currentIndexChanged(int)),this,SLOT(onComboBox_monthCurrentIndexChanged(int)));

	connect(radioButton_permanent_old,SIGNAL(clicked(bool)),this,SLOT(onRadioButton_permanent_oldClicked(bool)));
	connect(radioButton_temporary_old,SIGNAL(clicked(bool)),this,SLOT(onRadioButton_temporary_oldClicked(bool)));
	connect(comboBox_month_old,SIGNAL(currentIndexChanged(int)),this,SLOT(onComboBox_month_oldCurrentIndexChanged(int)));
	
	connect(pushButton_create,SIGNAL(clicked()),this,SLOT(accept()));
	connect(pushButton_cancel,SIGNAL(clicked()),this,SLOT(reject()));

	label_logo->setPixmap(QPixmap(QString::fromUtf8(":/ico/ico/sys_nr.png")));
	label_logo_2->setPixmap(QPixmap(QString::fromUtf8(":/ico/ico/sys_nr.png")));

	configSoft->setIcon(QIcon(QString::fromUtf8(":/ico/ico/tool_maintain.png")));

	Init();

	languageChange();

	onRadioButton_permanent_oldClicked(true);
	onRadioButton_permanentClicked(true);
}

CLicCreateDlg::~CLicCreateDlg()
{

}

void CLicCreateDlg::Init()
{
	char sHostID[100] = {'\0'};

	lineEdit_hostid->setText("");
	lineEdit_hostid_old->setText("");

	updateComboBoxSoftName(QString(""));

	int curYear = QDate::currentDate().year();

	QStringList strYear;
	for (int i = 0; i < 20; i++)
	{
		strYear.append(QString("%1").arg(curYear-5+i));
	}
	comboBox_year->insertItems(0,strYear);
	comboBox_year_old->insertItems(0,strYear);

	QDateTime dateTime = QDateTime::currentDateTime();
	int nYear = dateTime.date().year();
	int nMonth = dateTime.date().month();
	int nDay = dateTime.date().day();

	comboBox_year->setCurrentIndex(strYear.indexOf(QString("%1").arg(nYear)));
	comboBox_month->setCurrentIndex(nMonth-1);
	comboBox_day->setCurrentIndex(nDay-1);

	comboBox_year_old->setCurrentIndex(strYear.indexOf(QString("%1").arg(nYear)));
	comboBox_month_old->setCurrentIndex(nMonth-1);
	comboBox_day_old->setCurrentIndex(nDay-1);
}

void CLicCreateDlg::accept()
{
// 	installLicsenceInfo();
// 
// 	return;

// 	installLicenseTime();
// 
// 	return;

// 	CheckTime();
// 
// 	return;


	if(tabWidget->currentIndex() == 0)
	{
		if(radioButton_permanent->isChecked())
		{
			QString strText = "";
			QString strsoft;
			if(lineEdit_hostid->text().isEmpty() || lineEdit_hostid->text() == "" || lineEdit_hostid->text().trimmed().length() != NR_HOSTIDLEN)
				strText += tr("Error Host ID!");
			if(comboBox_softname->currentText().trimmed().isEmpty() || comboBox_softname->currentText().trimmed() == "")
				strsoft = " "+tr("Software name");
// 			if(lineEdit_contactid->text().trimmed().isEmpty() || lineEdit_contactid->text().trimmed() == "")
// 				strText += " "+tr("contact ID");
			if( comboBox_softname->currentText().trimmed().isEmpty() || comboBox_softname->currentText().trimmed() == "" )
				strText += " "+tr("%1 must be inputted!").arg(strsoft);
			if(strText.length() != 0)
			{
				QPalette pl = label_status->palette();
				pl.setColor(QPalette::WindowText,QColor(Qt::red));
				label_status->setPalette(pl);
				label_status->setText(strText);
				return;
			}
		}
		else
		{
			QString strText = "";
			QString strsoft;
			if(comboBox_softname->currentText().trimmed().isEmpty() || comboBox_softname->currentText().trimmed() == "")
				strsoft += tr("Software name");
			if( comboBox_softname->currentText().trimmed().isEmpty() || comboBox_softname->currentText().trimmed() == "" )
				strText += " "+tr("%1 must be inputted!").arg(strsoft);
			if(strText.length() != 0)
			{
				QPalette pl = label_status->palette();
				pl.setColor(QPalette::WindowText,QColor(Qt::red));
				label_status->setPalette(pl);
				label_status->setText(strText);
				return;
			}
		}
		
		QString strHostID = lineEdit_hostid->text();
		QString strContactID = lineEdit_contactid->text();
		QString strProName = ""/*lineEdit_proname->text()*/;
		QString strSoftName = comboBox_softname->currentText();
		char strLicense[100] = {'\0'};
		if(radioButton_permanent->isChecked())
		{
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
            if(createLicense(strSoftName.toAscii().data(),
                             strContactID.toAscii().data(),
                             strProName.toAscii().data(),
                             strHostID.toAscii().data(),strLicense))
#else
            if(createLicense(strSoftName.toLocal8Bit().data(),
                             strContactID.toLocal8Bit().data(),
                             strProName.toLocal8Bit().data(),
                             strHostID.toLocal8Bit().data(),strLicense))
#endif
            {
				lineEdit_license->setText(strLicense);
				QPalette pl = label_status->palette();
				pl.setColor(QPalette::WindowText,QColor(Qt::blue));
				label_status->setPalette(pl);
				label_status->setText(tr("Permanent license generated successfully!"));
			}
			else
			{
				lineEdit_license->setText("");
				QPalette pl = label_status->palette();
				pl.setColor(QPalette::WindowText,QColor(Qt::red));
				label_status->setPalette(pl);
				label_status->setText(tr("Failed to generate permanent license!"));
			}
		}
		else
		{
			int nYear = comboBox_year->currentText().toInt();
			int nMonth = comboBox_month->currentText().toInt();
			int nDay = comboBox_day->currentText().toInt();
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
            if(createTempLicense(strSoftName.toAscii().data(),
                                 strContactID.toAscii().data(),
                                 strProName.toAscii().data(),nYear,nMonth,nDay,strLicense))
#else
            if(createTempLicense(strSoftName.toLocal8Bit().data(),
                                 strContactID.toLocal8Bit().data(),
                                 strProName.toLocal8Bit().data(),nYear,nMonth,nDay,strLicense))
#endif
            {
				lineEdit_license->setText(strLicense);
				QPalette pl = label_status->palette();
				pl.setColor(QPalette::WindowText,QColor(Qt::blue));
				label_status->setPalette(pl);
				label_status->setText(tr("Temporary license generated successfully!"));
			}
			else
			{
				lineEdit_license->setText("");
				QPalette pl = label_status->palette();
				pl.setColor(QPalette::WindowText,QColor(Qt::red));
				label_status->setPalette(pl);
				label_status->setText(tr("Failed to generate temporary license!"));
			}
		}
	}
	else
	{
		if(radioButton_permanent_old->isChecked())
		{
			if( lineEdit_hostid_old->text().isEmpty() || lineEdit_hostid_old->text() == "" || lineEdit_hostid_old->text().trimmed().length() != NR_HOSTIDLEN)
			{
				QPalette pl = label_status->palette();
				pl.setColor(QPalette::WindowText,QColor(Qt::red));
				label_status_old->setPalette(pl);
				label_status_old->setText(tr("Error Host ID!"));
				return;
			}
		}
		
		QString strHostID = lineEdit_hostid_old->text();
		char strLicense[100] = {'\0'};
		if(radioButton_permanent_old->isChecked())
		{
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
			if(createLicenseOld(strHostID.toAscii().data(),strLicense))
#else
            if(createLicenseOld(strHostID.toLocal8Bit().data(),strLicense))
#endif
			{
				lineEdit_license_old->setText(strLicense);
				QPalette pl = label_status->palette();
				pl.setColor(QPalette::WindowText,QColor(Qt::blue));
				label_status_old->setPalette(pl);
				label_status_old->setText(tr("Permanent license generated successfully!"));
			}
			else
			{
				lineEdit_license_old->setText("");
				QPalette pl = label_status->palette();
				pl.setColor(QPalette::WindowText,QColor(Qt::red));
				label_status_old->setPalette(pl);
				label_status_old->setText(tr("Failed to generate permanent license!"));
			}
		}
		else
		{
			int nYear = comboBox_year_old->currentText().toInt();
			int nMonth = comboBox_month_old->currentText().toInt();
			int nDay = comboBox_day_old->currentText().toInt();
			if(createTempLicenseOld(nYear,nMonth,nDay,strLicense))
			{
				lineEdit_license_old->setText(strLicense);
				QPalette pl = label_status->palette();
				pl.setColor(QPalette::WindowText,QColor(Qt::blue));
				label_status_old->setPalette(pl);
				label_status_old->setText(tr("Temporary license generated successfully!"));
			}
			else
			{
				lineEdit_license_old->setText("");
				QPalette pl = label_status->palette();
				pl.setColor(QPalette::WindowText,QColor(Qt::red));
				label_status_old->setPalette(pl);
				label_status_old->setText(tr("Failed to generate temporary license!"));
			}
		}
	}
}

void CLicCreateDlg::onComboBox_monthCurrentIndexChanged(int nIndex)
{
	switch(nIndex)
	{
	case 0:
	case 2:
	case 4:
	case 6:
	case 7:
	case 9:
	case 11:
		{
			comboBox_day->clear();
			for(int i = 0; i < 31; ++i)
				comboBox_day->insertItem(i,QString("%1").arg(i+1));
			break;
		}
	case 3:
	case 5:
	case 8:
	case 10:
		{
			comboBox_day->clear();
			for(int i = 0; i < 30; ++i)
				comboBox_day->insertItem(i,QString("%1").arg(i+1));
			break;
		}
	case 1:
		{
			comboBox_day->clear();
			for(int i = 0; i < 29; ++i)
				comboBox_day->insertItem(i,QString("%1").arg(i+1));
			break;
		}
	}
}

void CLicCreateDlg::onRadioButton_permanentClicked(bool bChecked)
{
	radioButton_temporary->setChecked(!bChecked);
	comboBox_year->setEnabled(!bChecked);
	comboBox_month->setEnabled(!bChecked);
	comboBox_day->setEnabled(!bChecked);
	lineEdit_license->setText("");
	label_hostid->setText(QString("*")+tr("Host ID"));
	label_softname->setText(QString("*")+tr("Software name"));
	label_contactid->setText(/*QString("*")+*/tr("Contract No."));
	lineEdit_hostid->setEnabled(true);
}

void CLicCreateDlg::onRadioButton_temporaryClicked(bool bChecked)
{
	radioButton_permanent->setChecked(!bChecked);
	comboBox_year->setEnabled(bChecked);
	comboBox_month->setEnabled(bChecked);
	comboBox_day->setEnabled(bChecked);
	lineEdit_license->setText("");
	label_hostid->setText(tr("Host ID"));
	label_softname->setText(QString("*")+tr("Software name"));
	label_contactid->setText(tr("Contract No."));
	lineEdit_hostid->setEnabled(false);
	label_status->setText("");
}

void CLicCreateDlg::onComboBox_month_oldCurrentIndexChanged(int nIndex)
{
	switch(nIndex)
	{
	case 0:
	case 2:
	case 4:
	case 6:
	case 7:
	case 9:
	case 11:
		{
			comboBox_day_old->clear();
			for(int i = 0; i < 31; ++i)
				comboBox_day_old->insertItem(i,QString("%1").arg(i+1));
			break;
		}
	case 3:
	case 5:
	case 8:
	case 10:
		{
			comboBox_day_old->clear();
			for(int i = 0; i < 30; ++i)
				comboBox_day_old->insertItem(i,QString("%1").arg(i+1));
			break;
		}
	case 1:
		{
			comboBox_day_old->clear();
			for(int i = 0; i < 29; ++i)
				comboBox_day_old->insertItem(i,QString("%1").arg(i+1));
			break;
		}
	}
}

void CLicCreateDlg::onRadioButton_permanent_oldClicked(bool bChecked)
{
	radioButton_temporary_old->setChecked(!bChecked);
	comboBox_year_old->setEnabled(!bChecked);
	comboBox_month_old->setEnabled(!bChecked);
	comboBox_day_old->setEnabled(!bChecked);
	lineEdit_license_old->setText("");
	label_hostid_old->setText(QString("*")+tr("Host ID"));
	lineEdit_hostid_old->setEnabled(bChecked);
	label_status_old->setText("");
}

void CLicCreateDlg::onRadioButton_temporary_oldClicked(bool bChecked)
{
	radioButton_permanent_old->setChecked(!bChecked);
	comboBox_year_old->setEnabled(bChecked);
	comboBox_month_old->setEnabled(bChecked);
	comboBox_day_old->setEnabled(bChecked);
	lineEdit_license_old->setText("");
	label_hostid_old->setText(tr("Host ID"));
	lineEdit_hostid_old->setEnabled(!bChecked);
}

void CLicCreateDlg::onMinimumState()
{
	showMinimized();
}

void CLicCreateDlg::onActiveState()
{
	showNormal();
//	setWindowState(state);
}

void CLicCreateDlg::languageChange()
{
	setWindowTitle(tr("License Generator"));
	
	pushButton_create->setText(tr("Generate"));
	pushButton_cancel->setText(tr("Cancel"));
	label_hostid->setText("*"+tr("Host ID"));
	label_license->setText(tr("License No."));
	label_limited->setText(tr("Validation"));
	label_year->setText(tr("Year"));
	label_month->setText(tr("Month"));
	label_day->setText(tr("Day"));
	radioButton_permanent->setText(tr("Permanent license"));
	radioButton_temporary->setText(tr("Temporary license"));
	label_contactid->setText(tr("Contract No."));
	label_softname->setText("*"+tr("Software name"));
	label_bit->setText(tr("(12 bits)"));

	lineEdit_hostid->setToolTip(tr("Input Host ID"));
	lineEdit_contactid->setToolTip(tr("Input contract No."));
	lineEdit_license->setToolTip(tr("License No."));
	comboBox_softname->setToolTip(tr("Select software name"));
	comboBox_year->setToolTip(tr("Select year"));
	comboBox_month->setToolTip(tr("Select month"));
	comboBox_day->setToolTip(tr("Select date"));
	groupBox_2->setToolTip(tr("License type"));
	radioButton_permanent->setToolTip(tr("Generate permanent license"));
	radioButton_temporary->setToolTip(tr("Generate temporary license"));
	addSoft->setToolTip(tr("Add software name"));
	configSoft->setToolTip(tr("Config software name"));

	lineEdit_hostid_old->setToolTip(tr("Input Host ID"));
	lineEdit_license_old->setToolTip(tr("License No."));
	comboBox_year_old->setToolTip(tr("Select year"));
	comboBox_month_old->setToolTip(tr("Select month"));
	comboBox_day_old->setToolTip(tr("Select date"));
	groupBox_4->setToolTip(tr("License type"));
	radioButton_permanent_old->setToolTip(tr("Generate permanent license"));
	radioButton_temporary_old->setToolTip(tr("Generate temporary license"));
	
	
	textBrowser_info->setHtml(
		(
		"<p style=\" margin-top:0px; margin-bottom:0px; -qt-block-indent:0; color:#5f9ea0;\" align=center>"+tr("License Generator for PCS")+"</p>"
		"<p style=\" margin-top:0px; margin-bottom:0px; -qt-block-indent:0; color:#5f9ea0;\" align=center>"+tr("Vertion")+" V2.00"+"</p>"
		"<p style=\" margin-top:0px; margin-bottom:0px; -qt-block-indent:0; color:#5f9ea0;\" align=center>"+tr("Copyright(C) NR Electric Co., Ltd.")+"</p>"
		"<p style=\" margin-top:0px; margin-bottom:0px; -qt-block-indent:0; color:#5f9ea0;\" align=center>"+tr("www.nari-relays.com")+"</p>"
		));

	textBrowser_info_old->setHtml(
		(
		"<p style=\" margin-top:0px; margin-bottom:0px; -qt-block-indent:0; color:#5f9ea0;\" align=center>"+tr("License Generator for RCS")+"</p>"
		"<p style=\" margin-top:0px; margin-bottom:0px; -qt-block-indent:0; color:#5f9ea0;\" align=center>"+tr("Vertion")+" V1.00"+"</p>"
		"<p style=\" margin-top:0px; margin-bottom:0px; -qt-block-indent:0; color:#5f9ea0;\" align=center>"+tr("Copyright(C) NR Electric Co., Ltd.")+"</p>"
		"<p style=\" margin-top:0px; margin-bottom:0px; -qt-block-indent:0; color:#5f9ea0;\" align=center>"+tr("www.nari-relays.com")+"</p>"
		));
	
	label_hostid_old->setText("*"+tr("Host ID"));
	label_license_old->setText(tr("License No."));
	label_limited_old->setText(tr("Validation"));
	label_year_old->setText(tr("Year"));
	label_month_old->setText(tr("Month"));
	label_day_old->setText(tr("Day"));
	radioButton_permanent_old->setText(tr("Permanent license"));
	radioButton_temporary_old->setText(tr("Temporary license"));
	label_bit_old->setText(tr("(12 bits)"));
	
	tabWidget->setTabText(0,tr("For PCS(new)"));
	tabWidget->setTabText(1,tr("For RCS(old)"));

	tabWidget->removeTab(1);
}

void CLicCreateDlg::on_addSoft_clicked()
{
	CLicAddSoftDlg dlg(true,this,0);
	connect(&dlg,SIGNAL(minimumState()),this,SLOT(onMinimumState()));
	connect(&dlg,SIGNAL(closeState()),this,SLOT(onActiveState()));
	
	QSettings setting(QApplication::applicationDirPath()+"/"+"software.txt",QSettings::IniFormat);
	QStringList keys = setting.allKeys();
	int nkeys = keys.count();

	dlg.lineEdit_softno->setText(QString("%1").arg(nkeys+1));
	dlg.lineEdit_softno->setEnabled(false);
	dlg.lineEdit_softname->setText(comboBox_softname->currentText());
	dlg.textEdit_softdec->setText("");
	if(comboBox_softname->currentText().isEmpty())
		dlg.lineEdit_softname->setFocus();
	else
		dlg.textEdit_softdec->setFocus();
	if(dlg.exec() == QDialog::Accepted)
	{
		updateComboBoxSoftName(dlg.lineEdit_softname->text().trimmed());
	}
}

void CLicCreateDlg::on_configSoft_clicked()
{
	CLicConfigSoftDlg dlg(this,0);
	connect(&dlg,SIGNAL(minimumState()),this,SLOT(onMinimumState()));
	connect(&dlg,SIGNAL(closeState()),this,SLOT(onActiveState()));

	QSettings setting(QApplication::applicationDirPath()+"/"+"software.txt",QSettings::IniFormat);
	QStringList keys = setting.allKeys();

	dlg.exec();
	updateComboBoxSoftName("");
}

void CLicCreateDlg::updateComboBoxSoftName(const QString& strName)
{
	comboBox_softname->clear();
	QSettings setting(QApplication::applicationDirPath()+"/"+"software.txt",QSettings::IniFormat);
	QStringList softNameList = setting.allKeys();

	comboBox_softname->insertItems(0,softNameList);
	
	QCompleter* softCompleter = new QCompleter(softNameList,this);
	softCompleter->setCaseSensitivity(Qt::CaseInsensitive);
	comboBox_softname->setCompleter(softCompleter);
	
	comboBox_softname->setMaxVisibleItems(15);
	comboBox_softname->setCurrentIndex(softNameList.indexOf(strName));
}
