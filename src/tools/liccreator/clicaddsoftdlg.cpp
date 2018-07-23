#include "clicaddsoftdlg.h"
#include <QSettings>
#include <QMessageBox>

CLicAddSoftDlg::CLicAddSoftDlg(bool bAdd,QWidget * parent /* = 0 */, Qt::WindowFlags f /* = 0 */)
:QDialog(parent,f),m_bAdd(bAdd)
{
	setupUi(this);
	
	languageChange();

	setWindowFlags(Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint);
	resize(300,minimumHeight());
	m_sRawName = "";
}

CLicAddSoftDlg::~CLicAddSoftDlg()
{

}

void CLicAddSoftDlg::on_addButton_clicked()
{
	QString strSoftName = lineEdit_softname->text().trimmed();
	QString strSoftDec = textEdit_softdec->toPlainText();
	QString strSoftNo = lineEdit_softno->text();
	if(strSoftName.length() == 0)
	{
		QMessageBox msg;
		msg.addButton(tr("OK"),QMessageBox::AcceptRole);
		msg.setWindowTitle(tr("Prompt"));
		msg.setText(tr("Software name must be inputted!"));
		msg.exec();
		return;
	}

	QSettings setting(QApplication::applicationDirPath()+"/"+"software.txt",QSettings::IniFormat);
	QStringList keys = setting.allKeys();
	if(m_bAdd)
	{
		if(keys.indexOf(strSoftName) >= 0)
		{
			QMessageBox msg;
			msg.addButton(tr("OK"),QMessageBox::AcceptRole);
			msg.setWindowTitle(tr("Prompt"));
			msg.setText(tr("The software name is occupied, please input again!"));
			msg.exec();
			return;
		}
		setting.setValue(strSoftName,strSoftDec);
	}
	else
	{
		setting.remove(m_sRawName);
		setting.setValue(strSoftName,strSoftDec);
	}

	accept();
}

void CLicAddSoftDlg::on_cancelButton_clicked()
{
	reject();
}

void CLicAddSoftDlg::changeEvent(QEvent* e)
{
	int nttt = e->type();
	if(e->type()==QEvent::WindowStateChange)
	{
		int nt = windowState();
		if(windowState() & Qt::WindowMinimized)
		{
			emit minimumState();
		}
	}
	else if(e->type() == QEvent::ActivationChange)
	{
		if(windowState() & Qt::WindowActive)
			emit activeState();
	}
	else if(e->type() == QEvent::Close)
	{
		emit closeState();
	}
	e->ignore();
}

void CLicAddSoftDlg::languageChange()
{
	setWindowTitle(tr("Add software name"));

	label_softno->setText(tr("Software No."));
	label_softname->setText(tr("Software name"));
	label_softdec->setText(tr("Software description"));
	addButton->setText(tr("OK"));
	cancelButton->setText(tr("Cancel"));
}