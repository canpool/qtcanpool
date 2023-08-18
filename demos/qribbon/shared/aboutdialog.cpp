#include "aboutdialog.h"

QRIBBON_USE_NAMESPACE

AboutDialog::AboutDialog(QWidget *parent) : QMessageBox(parent) {}

AboutDialog::AboutDialog(Icon icon, const QString &title, const QString &text, StandardButtons buttons, QWidget *parent,
                         Qt::WindowFlags f)
    : QMessageBox(icon, title, text, buttons, parent, f)
{
}

AboutDialog::~AboutDialog() {}

void AboutDialog::show(QWidget *parent, const QString &title, const QString &component, const QString &version)
{
    QString translatedTextAboutCaption;
    translatedTextAboutCaption = QMessageBox::tr("<h3>Qt Ribbon Software</h3>"
                                                 "<p>This program uses %1 version %2.</p>")
                                     .arg(component)
                                     .arg(version);

    QString translatedTextAboutText;
    translatedTextAboutText = QMessageBox::tr("<p>QRibbon is like Microsoft Office Style.</p>");

    AboutDialog *msgBox = new AboutDialog(parent);

    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setWindowTitle(title.isEmpty() ? tr("About QRibbon") : title);
    msgBox->setText(translatedTextAboutCaption);
    msgBox->setInformativeText(translatedTextAboutText);

    QPixmap pm(QLatin1String(":/res/logo.png"));
    if (!pm.isNull())
        msgBox->setIconPixmap(pm);

    msgBox->exec();
}
