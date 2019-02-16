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
#ifndef QTN_MESSAGEBOX_H
#define QTN_MESSAGEBOX_H

#include <QMessageBox>

#include "QtitanDef.h"


namespace Qtitan
{
    class AboutDialog: public QMessageBox
    {
        Q_OBJECT
    public:
        explicit AboutDialog(QWidget* parent = Q_NULL);
        AboutDialog(Icon icon, const QString& title, const QString& text, StandardButtons buttons = NoButton, QWidget* parent = Q_NULL,
                   Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
        virtual ~AboutDialog();
    public:
        static void show(QWidget* parent, const QString& title, const QString& component, const QString& version);
    private:
        Q_DISABLE_COPY(AboutDialog)
    };

}; //namespace Qtitan

#endif // QTN_MESSAGEBOX_H
