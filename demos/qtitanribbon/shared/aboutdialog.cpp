/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced AboutDialog for Qt)
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#include "aboutdialog.h"

using namespace Qtitan;

AboutDialog::AboutDialog(QWidget* parent)
    : QMessageBox(parent)
{
}

AboutDialog::AboutDialog(Icon icon, const QString& title, const QString& text, 
                       StandardButtons buttons, QWidget* parent, Qt::WindowFlags f)
    : QMessageBox(icon, title, text, buttons, parent, f)
{
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::show(QWidget* parent, const QString& title, const QString& component, const QString& version)
{
    QString translatedTextAboutQtnCaption;
    translatedTextAboutQtnCaption = QMessageBox::tr(
        "<h3>Developer Machines Software</h3>"
        "<p>This program uses %1 version %2.</p>"
        ).arg(component).arg(version);

    QString translatedTextAboutQtText;
    translatedTextAboutQtText = QMessageBox::tr(
        "<p>Qtitan is a Qt/C++ framework for cross-platform application "
        "development.</p>"
        "<p>Qtitan provides single-source portability across MS&nbsp;Windows, "
        "Mac&nbsp;OS&nbsp;X, Linux (all major commercial variants).</p>"
        "<p>Series of a components Qtitan product of Developer Machines. See <a href=\"http://www.devmachines.com/\">www.devmachines.com</a> "
        "for more information.</p>"
        "<p>Copyright (c) 2009-2013 Developer Machines. All Rights Reserved.</p>"
        );

    AboutDialog* msgBox = new AboutDialog(parent);

    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setWindowTitle(title.isEmpty() ? tr("About Developer Machines") : title);
    msgBox->setText(translatedTextAboutQtnCaption);
    msgBox->setInformativeText(translatedTextAboutQtText);

    QPixmap pm(QLatin1String(":/res/qtitan.png"));
    if (!pm.isNull())
        msgBox->setIconPixmap(pm);

    msgBox->exec();
}
