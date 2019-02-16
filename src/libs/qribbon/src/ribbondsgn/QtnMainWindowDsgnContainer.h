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
#ifndef QTITAN_MAINWINDOWCONTAINER_DSGN_H
#define QTITAN_MAINWINDOWCONTAINER_DSGN_H

#include <QWidget>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QExtensionFactory>

namespace Qtitan
{
    class RibbonStyle;
    class RibbonMainWindow;
    /* MainWindowContainer */
    class MainWindowContainer : public QObject, public QDesignerContainerExtension
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerContainerExtension)

    public:
        explicit MainWindowContainer(RibbonMainWindow* widget, QObject* parent = 0);
        virtual ~MainWindowContainer();
    public:
        virtual int count() const;
        virtual QWidget* widget(int index) const;
        virtual int currentIndex() const;
        virtual void setCurrentIndex(int index);
        virtual void addWidget(QWidget* widget);
        virtual void insertWidget(int index, QWidget* widget);
        virtual void remove(int index);
    private:
        RibbonMainWindow* m_mainWindow;
        QList<QWidget*> m_widgets;
    };

    /* MainWindowContainerFactory */
    class MainWindowContainerFactory : public QExtensionFactory
    {
        Q_OBJECT
    public:
        explicit MainWindowContainerFactory(QExtensionManager* parent);
    protected:
        virtual QObject *createExtension(QObject* object, const QString& iid, QObject* parent) const;
    };

    extern QWidget* create_fake_ribbon_style_widget(QWidget* parent);
    extern QWidget* create_fake_office_style_widget(QWidget* parent);

}; //namespace Qtitan

#endif // QTITAN_MAINWINDOWCONTAINER_DSGN_H
