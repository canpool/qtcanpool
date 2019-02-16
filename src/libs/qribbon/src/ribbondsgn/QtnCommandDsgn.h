/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
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

#ifndef QTITAN_COMMAND_DSGN_H
#define QTITAN_COMMAND_DSGN_H

#include <QUndoCommand>
#include <QtCore/QPointer>

#include "QtitanDef.h"

class QMainWindow;
class QDesignerFormWindowInterface;
class QDesignerFormEditorInterface;

namespace Qtitan
{
    class RibbonBar;

    class FormDsgnCommand : public QUndoCommand
    {
    public:
        FormDsgnCommand(const QString& description, QDesignerFormWindowInterface* formWindow)
            : QUndoCommand(description)
            , m_formWindow(formWindow)
        {
        }

    public:
        QDesignerFormWindowInterface *formWindow() const { return m_formWindow; }
        QDesignerFormEditorInterface *core() const;

    private:
        QPointer<QDesignerFormWindowInterface> m_formWindow;
    };

    /* CreateRibbonBarCommand */
    class CreateRibbonBarCommand : public FormDsgnCommand
    {
    public:
        explicit CreateRibbonBarCommand(QDesignerFormWindowInterface *formWindow);

    public:
        void init(QMainWindow* mainWindow);

        virtual void undo();
        virtual void redo();

    private:
        QPointer<QMainWindow> m_mainWindow;
        QPointer<QWidget> m_ribbonBar;
    };

    /* DeleteRibbonBarCommand */
    class DeleteRibbonBarCommand : public FormDsgnCommand
    {
    public:
        explicit DeleteRibbonBarCommand(QDesignerFormWindowInterface *formWindow);

    public:
        void init(RibbonBar* ribbonBar);

        virtual void undo();
        virtual void redo();

    private:
        QPointer<QMainWindow> m_mainWindow;
        QPointer<QWidget> m_ribbonBar;
    };

    /* RibbonCommand */
    class RibbonBarCommand : public FormDsgnCommand
    {
    public:
        RibbonBarCommand(const QString& description, QDesignerFormWindowInterface* formWindow);

    public:
        RibbonBar* mainWidget() const { return (RibbonBar*)(m_ribbonBar.data()); }

    protected:
        QPointer<QWidget> m_ribbonBar;
        QPointer<QWidget> m_ribbonPage;
    };

    /* AddRibbonPageCommand */
    class AddRibbonPageCommand : public RibbonBarCommand
    {
    public:
        explicit AddRibbonPageCommand(QDesignerFormWindowInterface* formWindow);

    public:
        void init(RibbonBar* ribbonBar);

        virtual void undo();
        virtual void redo();
    };

    /* DeleteRibbonPageCommand */
    class DeleteRibbonPageCommand : public RibbonBarCommand
    {
    public:
        explicit DeleteRibbonPageCommand(QDesignerFormWindowInterface *formWindow);
        virtual ~DeleteRibbonPageCommand();

        void init(RibbonBar* ribbonBar);

    public:
        virtual void redo();
        virtual void undo();
    };

    /* DeleteStyleCommand */
    class DeleteStyleCommand : public QUndoCommand
    {
    public:
        DeleteStyleCommand(QDesignerFormWindowInterface* formWindow);
    public:
        void init(QWidget* widget);

        virtual void redo();
        virtual void undo();

    private:
        QPointer<QWidget> m_widget;
        QPointer<QWidget> m_parentWidget;
        QDesignerFormWindowInterface* m_formWindow;
    };

}; //namespace Qtitan

#endif //QTITAN_COMMAND_DSGN_H
