/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
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
#ifndef QTN_RIBBONCONTROLS_H
#define QTN_RIBBONCONTROLS_H

#include <QWidget>
#include <QResizeEvent>
#include <QStyleOption>
#include <QLabel>
#include <QToolButton>
#include <QWidgetAction>
#include <QMenu>

#include "QtnRibbonButtonPrivate.h"
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class RibbonControl;
class RibbonControlSizeDefinitionPrivate;
/* RibbonControlSizeDefinition */
class QTITAN_EXPORT RibbonControlSizeDefinition : public QObject
{
    Q_OBJECT
public:
    enum GroupSize
    {
        GroupLarge  = 0,
        GroupMedium = 1,
        GroupSmall  = 2,
        GroupPopup  = 3
    };

    enum ControlImageSize 
    {
        ImageNone,
        ImageLarge,
        ImageSmall
    };

public:
    explicit RibbonControlSizeDefinition(RibbonControl* parentControl, GroupSize size);
    virtual ~RibbonControlSizeDefinition();

public:
    ControlImageSize imageSize() const;
    void setImageSize(ControlImageSize size);

    bool isLabelVisible() const;
    void setLabelVisible(bool visible);

    bool isPopup() const;
    void setPopup(bool popup);

    int visualIndex() const;
    void setVisualIndex(int index);

    bool showSeparator() const;
    void setShowSeparator(bool showSeparator);

    bool isVisible() const;
    void setVisible(bool visible);

    bool isEnabled() const;
    void setEnabled(bool enabled);

    bool isStretchable() const;
    void setStretchable(bool stretchable);

    int minimumItemCount() const;
    void setMinimumItemCount(int count);

    int maximumItemCount() const;
    void setMaximumItemCount(int count);

private:
    QTN_DECLARE_PRIVATE(RibbonControlSizeDefinition)
    Q_DISABLE_COPY(RibbonControlSizeDefinition)
};

class RibbonGroup;
class RibbonControlPrivate;
/* RibbonControl */
class QTITAN_EXPORT RibbonControl : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonControl(RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonControl();

public:
    RibbonGroup* parentGroup() const;
    QAction* defaultAction() const;
    void setDefaultAction(QAction* action);

    virtual void adjustCurrentSize(bool expand);
    virtual void sizeChanged(RibbonControlSizeDefinition::GroupSize size);

    virtual void actionChanged();
    virtual void updateLayout();

    RibbonControlSizeDefinition::GroupSize currentSize() const;
    RibbonControlSizeDefinition* sizeDefinition(RibbonControlSizeDefinition::GroupSize size) const;
private:
    friend class RibbonGroup;
    QTN_DECLARE_PRIVATE(RibbonControl)
    Q_DISABLE_COPY(RibbonControl)
};

class RibbonWidgetControlPrivate;
/* RibbonWidgetControl */
class QTITAN_EXPORT RibbonWidgetControl : public RibbonControl
{
    Q_OBJECT
public:
    explicit RibbonWidgetControl(RibbonGroup* parentGroup, bool ignoreActionSettings);
    virtual ~RibbonWidgetControl();

public:
    int margin() const;
    void setMargin(int margin);

public:
    virtual void sizeChanged(RibbonControlSizeDefinition::GroupSize size);
    virtual QSize sizeHint() const;

protected:
    QWidget* contentWidget() const;
    void setContentWidget(QWidget* widget);

protected:
    virtual void childEvent(QChildEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void resizeEvent(QResizeEvent* event);

private:
    friend class RibbonGroup;
    friend class RibbonGroupPrivate;
    QTN_DECLARE_PRIVATE(RibbonWidgetControl)
    Q_DISABLE_COPY(RibbonWidgetControl)
};

class RibbonColumnBreakControlPrivate;
/* RibbonColumnBreakControl */
class QTITAN_EXPORT RibbonColumnBreakControl : public RibbonWidgetControl
{
    Q_OBJECT
public:
    explicit RibbonColumnBreakControl(RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonColumnBreakControl();

public:
    Qt::Alignment alignmentText() const;
    void setAlignmentText(Qt::Alignment alignment);

public:
    virtual void sizeChanged(RibbonControlSizeDefinition::GroupSize size);
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent*);

private:
    QTN_DECLARE_PRIVATE(RibbonColumnBreakControl)
    Q_DISABLE_COPY(RibbonColumnBreakControl)
};

/* RibbonLabelControl */
class QTITAN_EXPORT RibbonLabelControl : public RibbonWidgetControl
{
public: 
    explicit RibbonLabelControl(RibbonGroup* parentGroup = Q_NULL);
    explicit RibbonLabelControl(const QString& text, RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonLabelControl();

public:
    QLabel* widget() const;

private:
    Q_DISABLE_COPY(RibbonLabelControl)
};

class RibbonToolBarControlPrivate;
/* RibbonToolBarControl */
class QTITAN_EXPORT RibbonToolBarControl : public RibbonControl
{
    Q_OBJECT
public:
    explicit RibbonToolBarControl(RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonToolBarControl();

public:
    virtual void sizeChanged(RibbonControlSizeDefinition::GroupSize size);
    virtual void updateLayout();
    void clear();
    QAction* addWidget(QWidget* widget);
    QAction* addWidget(const QIcon& icon, const QString& text, QWidget* widget);
    QAction* addMenu(const QIcon& icon, const QString& text, QMenu* menu, QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);
    QAction* addAction(QAction* action, Qt::ToolButtonStyle style, QMenu* menu = Q_NULL, QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);
    QAction* insertAction(QAction* before, QAction* action, Qt::ToolButtonStyle style, QMenu* menu = Q_NULL, QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);
    QAction* addAction(const QString& text);
    QAction* addAction(const QIcon& icon, const QString& text);
    QAction* addAction(const QString& text, const QObject* receiver, const char* member);
    QAction* addAction(const QIcon& icon, const QString& text, const QObject* receiver, const char * member);
    QAction* addAction(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style, QMenu* menu = Q_NULL, QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);
    QAction* addSeparator();
    virtual QSize sizeHint() const;
    void setMargin(int margin);
    int margin() const;
    QWidget* widgetByAction(QAction* action);
    using QWidget::addAction;
    using QWidget::insertAction;

protected:
    int rowsCount();
    void setRowsCount(int count);

protected:
    virtual bool event(QEvent* event);
    virtual void actionEvent(QActionEvent* event);
    virtual void changeEvent(QEvent* event);

private:
    QTN_DECLARE_PRIVATE(RibbonToolBarControl)
    Q_DISABLE_COPY(RibbonToolBarControl)
};


QTITAN_END_NAMESPACE


#endif // QTN_RIBBONCONTROLS_H


