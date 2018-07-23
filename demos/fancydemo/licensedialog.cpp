#include "licensedialog.h"
#include "license/nrlicense.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>

LicenseDialog::LicenseDialog(QWidget* parent /* = 0 */)
:QDialog(parent)
{
    initUi();

    QMetaObject::connectSlotsByName(this);

    setWindowTitle(tr("Serial number register"));
    setMinimumWidth(300);
    setStyleSheet("color:black;");
}

LicenseDialog::~LicenseDialog()
{

}

void LicenseDialog::initUi()
{
    hostLabel = new QLabel(this);
    hostLabel->setText(tr("Host id: "));
    hostLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    licenseLabel = new QLabel(this);
    licenseLabel->setText(tr("Serial number: "));
    licenseLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    hostEdit = new QLineEdit(this);
    hostEdit->setReadOnly(true);

    licenseEdit = new QLineEdit(this);
    licenseEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    emailLabel = new QLabel(this);
    emailLabel->setText(tr("Contact way: "));
    emailLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    emailTo = new QLabel(this);
    emailTo->setText("<a href = 1074867723@qq.com>1074867723@qq.com</a>");
    connect(emailTo,SIGNAL(linkActivated(const QString&)),this,SLOT(on_open_email_url(const QString&)));

    phoneLabel = new QLabel(this);
    phoneLabel->setText(tr("Telephone: ")+QString("025-84985368"));
    phoneLabel->setAlignment(Qt::AlignRight);

    registerButton = new QPushButton(this);
    registerButton->setText(tr("Register"));
    registerButton->setObjectName("registerButton");

    cancelButton = new QPushButton(this);
    cancelButton->setText(tr("Cancel"));
    cancelButton->setObjectName("cancelButton");

    QGridLayout* gridlayout = new QGridLayout;
    gridlayout->addWidget(hostLabel,0,0,1,2);
    gridlayout->addWidget(hostEdit,0,1,1,2);
    gridlayout->addWidget(licenseLabel,1,0,1,2);
    gridlayout->addWidget(licenseEdit,1,1,1,2);
    gridlayout->addWidget(emailLabel,2,0,1,1);
    gridlayout->addWidget(emailTo,2,1,1,1);
    gridlayout->addWidget(phoneLabel,2,2,1,1);

    QHBoxLayout* hlayout = new QHBoxLayout;
    hlayout->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    hlayout->addWidget(registerButton);
    hlayout->addWidget(cancelButton);
    hlayout->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Fixed));

    QVBoxLayout* vlayout = new QVBoxLayout(this);
    vlayout->addLayout(gridlayout);
    vlayout->addLayout(hlayout);

    char hostid[20];
    memset(hostid,0,sizeof(hostid));
    bool nret = createHostIDNew(hostid);
    if(nret)
        hostEdit->setText(QString(hostid));
    else
        hostEdit->setText(tr("Please check configuration!"));
}

void LicenseDialog::on_registerButton_clicked()
{
    QString softname = "fancydemo";
    QString shostid = hostEdit->text();
    QString slicense = licenseEdit->text();
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    if (registerLicense(softname.toAscii().data(),"","",slicense.toAscii().data()))
#else
    if (registerLicense(softname.toLocal8Bit().data(),"","",slicense.toLocal8Bit().data()))
#endif
    {
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
        int nret = CheckLicense(softname.toAscii().data());
#else
        int nret = CheckLicense(softname.toLocal8Bit().data());
#endif
        if(nret > 0)
            accept();
        else
            QMessageBox::information(this,tr("Tips"),tr("Serial number registration failure!"));
    }
}

void LicenseDialog::on_cancelButton_clicked()
{
    reject();
}

void LicenseDialog::on_open_email_url(const QString& url)
{
    Q_UNUSED(url);
    QString hostid = hostEdit->text();
    QDesktopServices::openUrl(QUrl(tr("mailto:1074867723@qq.com?subject=Intelligent Substation Base &body=host id: %1").arg(hostid)));
}
