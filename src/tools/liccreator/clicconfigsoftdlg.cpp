#include "clicconfigsoftdlg.h"
#include "clicaddsoftdlg.h"

#include <QSettings>
#include <QHeaderView>
#include <QMessageBox>

CLicConfigSoftDlg::CLicConfigSoftDlg(QWidget * parent /* = 0 */, Qt::WindowFlags f /* = 0 */)
:QDialog(parent,f)
{
	setupUi(this);

	m_sCurrentSoftName = "";

	setWindowFlags(Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint);
	resize(500,400);

	Init();
	languageChange();
}

CLicConfigSoftDlg::~CLicConfigSoftDlg()
{

}

void CLicConfigSoftDlg::Init()
{
	QSettings setting(QApplication::applicationDirPath()+"/"+"software.txt",QSettings::IniFormat);
	QStringList keys = setting.allKeys();

	tableWidget->setRowCount(0);
	tableWidget->clear();
	tableWidget->setColumnCount(2);
	tableWidget->setRowCount(keys.count());
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
	tableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
	tableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
#else
    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
#endif
	tableWidget->resizeColumnToContents(0);
	tableWidget->resizeColumnToContents(1);
	QTableWidgetItem* namaeItem = new QTableWidgetItem(tr("Software name"));
	QTableWidgetItem* decItem = new QTableWidgetItem(tr("Software description"));
	tableWidget->setHorizontalHeaderItem(0,namaeItem);
	tableWidget->setHorizontalHeaderItem(1,decItem);
	for (int i = 0; i < keys.count(); i++)
	{
		tableWidget->setRowHeight(i,20);
		QTableWidgetItem* item = new QTableWidgetItem(keys[i]);
		tableWidget->setItem(i,0,item);
		item = new QTableWidgetItem(setting.value(keys[i]).toString());
		tableWidget->setItem(i,1,item);
	}
	tableWidget->setCurrentCell(keys.indexOf(m_sCurrentSoftName),0);
}

void CLicConfigSoftDlg::on_deleteButton_clicked()
{
	int row = tableWidget->currentRow();
	if(row < 0 || row >= tableWidget->rowCount())
		return;
	QString strName = tableWidget->item(row,0)->text();
	
	QSettings setting(QApplication::applicationDirPath()+"/"+"software.txt",QSettings::IniFormat);
	QMessageBox msg;
	msg.addButton(tr("OK"),QMessageBox::AcceptRole);
	msg.addButton(tr("Cancel"),QMessageBox::RejectRole);
	msg.setWindowTitle(tr("Prompt"));
	msg.setText(tr("Confirm to delete this software name?"));
	if(msg.exec() == QMessageBox::AcceptRole)
	{
		setting.remove(strName);
		Init();
	}
}

void CLicConfigSoftDlg::on_cancelButton_clicked()
{
	accept();
}

void CLicConfigSoftDlg::on_editButton_clicked()
{
	int row = tableWidget->currentRow();
	if(row < 0 || row >= tableWidget->rowCount())
		return;

	QSettings setting(QApplication::applicationDirPath()+"/"+"software.txt",QSettings::IniFormat);
	QStringList keys = setting.allKeys();
	
	CLicAddSoftDlg dlg(false,this,0);
	connect(&dlg,SIGNAL(minimumState()),this,SLOT(onMinimumState()));
	connect(&dlg,SIGNAL(closeState()),this,SLOT(onActiveState()));

	dlg.lineEdit_softno->setEnabled(false);
	dlg.setWindowTitle(tr("Edit software name"));
	dlg.addButton->setText(tr("OK"));
	dlg.m_sRawName = tableWidget->item(row,0)->text();
	dlg.lineEdit_softno->setText(QString("%1").arg(row+1));
	dlg.lineEdit_softname->setText(tableWidget->item(row,0)->text());
	dlg.textEdit_softdec->setText(tableWidget->item(row,1)->text());
	if(dlg.exec()==QDialog::Accepted)
	{
		m_sCurrentSoftName = dlg.lineEdit_softname->text();
		Init();
	}
}

void CLicConfigSoftDlg::on_addButton_clicked()
{
	QSettings setting(QApplication::applicationDirPath()+"/"+"software.txt",QSettings::IniFormat);
	QStringList keys = setting.allKeys();
	
	CLicAddSoftDlg dlg(true,this,0);
	connect(&dlg,SIGNAL(minimumState()),this,SLOT(onMinimumState()));
	connect(&dlg,SIGNAL(closeState()),this,SLOT(onActiveState()));
	
	dlg.lineEdit_softno->setEnabled(false);
	dlg.setWindowTitle(tr("Add software name"));
	dlg.addButton->setText(tr("Add"));
	dlg.m_sRawName = "";
	dlg.lineEdit_softno->setText(QString("%1").arg(keys.count()+1));
	dlg.lineEdit_softname->setText("");
	dlg.textEdit_softdec->setText("");
	if(dlg.exec()==QDialog::Accepted)
	{
		m_sCurrentSoftName = dlg.lineEdit_softname->text();
		Init();
	}
}

void CLicConfigSoftDlg::on_tableWidget_cellDoubleClicked(int row, int column)
{
	QSettings setting(QApplication::applicationDirPath()+"/"+"software.txt",QSettings::IniFormat);
	QStringList keys = setting.allKeys();

	CLicAddSoftDlg dlg(false,this,0);
	connect(&dlg,SIGNAL(minimumState()),this,SLOT(onMinimumState()));
	connect(&dlg,SIGNAL(closeState()),this,SLOT(onActiveState()));

	dlg.lineEdit_softno->setEnabled(false);
	dlg.setWindowTitle(tr("Edit software name"));
	dlg.addButton->setText(tr("OK"));
	dlg.m_sRawName = tableWidget->item(row,0)->text();
	dlg.lineEdit_softno->setText(QString("%1").arg(row+1));
	dlg.lineEdit_softname->setText(tableWidget->item(row,0)->text());
	dlg.textEdit_softdec->setText(tableWidget->item(row,1)->text());
	if(dlg.exec()==QDialog::Accepted)
	{
		m_sCurrentSoftName = dlg.lineEdit_softname->text();
		Init();
	}
}

void CLicConfigSoftDlg::changeEvent(QEvent* e)
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

void CLicConfigSoftDlg::onMinimumState()
{
	showMinimized();
}

void CLicConfigSoftDlg::onActiveState()
{
	showNormal();
//	setWindowState(state);
}

void CLicConfigSoftDlg::languageChange()
{
	setWindowTitle(tr("Config software name"));

	addButton->setText(tr("Add"));
	editButton->setText(tr("Edit"));
	deleteButton->setText(tr("Delete"));
	cancelButton->setText(tr("OK"));
}
