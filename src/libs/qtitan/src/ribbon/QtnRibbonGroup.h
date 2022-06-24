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
#ifndef QTN_RIBBONGROUP_H
#define QTN_RIBBONGROUP_H

#include <QToolButton>
#include <QMenu>

#include "QtnRibbonControls.h"
#include "QtnRibbonPage.h"
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class RibbonGroupPrivate;
class QTITAN_EXPORT RibbonGroup : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool reduced READ isReduced)
    Q_PROPERTY(const QString& title READ title WRITE setTitle)
    Q_PROPERTY(const QFont& titleFont READ titleFont WRITE setTitleFont)
    Q_PROPERTY(const QColor& titleColor READ titleColor WRITE setTitleColor)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(bool isOptionButtonVisible READ isOptionButtonVisible WRITE setOptionButtonVisible DESIGNABLE false)
    Q_PROPERTY(Qt::Alignment contentAlignment READ contentAlignment WRITE setContentAlignment)
    Q_PROPERTY(Qt::TextElideMode titleElideMode READ titleElideMode WRITE setTitleElideMode)
public:
    explicit RibbonGroup(QWidget* parent = Q_NULL);
    explicit RibbonGroup(RibbonPage* page, const QString& title);
    virtual ~RibbonGroup();

public:
    RibbonBar* ribbonBar() const;

    bool isReduced() const;

    const QString& title() const;
    void setTitle(const QString& title);

    const QFont& titleFont() const;
    void setTitleFont(const QFont& font);

    const QColor& titleColor() const;
    void setTitleColor(const QColor& color);

    const QIcon& icon() const;
    void setIcon(const QIcon& icon);

    bool isOptionButtonVisible() const;
    void setOptionButtonVisible(bool visible = true);

    QAction* optionButtonAction() const;
    void setOptionButtonAction(QAction* action);

    Qt::Alignment contentAlignment() const;
    void setContentAlignment(Qt::Alignment alignment);

    Qt::Alignment controlsAlignment() const;
    void setControlsAlignment(Qt::Alignment alignment);

    int controlCount() const;
    RibbonControl* controlByIndex(int index) const;
    RibbonControl* controlByAction(QAction* action) const;
    RibbonWidgetControl* controlByWidget(QWidget* widget) const;
    RibbonControlSizeDefinition::GroupSize currentSize() const;

    void addControl(RibbonControl* control);
    void removeControl(RibbonControl* control);

    QAction* addAction(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style, 
        QMenu* menu = Q_NULL, QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);
    QAction* addAction(QAction* action, Qt::ToolButtonStyle style, QMenu* menu = Q_NULL, 
        QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);
    QAction* insertAction(QAction* before, QAction* action, Qt::ToolButtonStyle style, QMenu* menu = Q_NULL, 
        QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);

    QAction* addWidget(QWidget* widget);
    QAction* addWidget(const QIcon& icon, const QString& text, QWidget* widget);
    QAction* addWidget(const QIcon& icon, const QString& text, bool stretch, QWidget* widget);
    QMenu* addMenu(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style = Qt::ToolButtonFollowStyle);
    QAction* addSeparator();

    void remove(QWidget* widget);
    void clear();

    Qt::TextElideMode titleElideMode() const;
    void setTitleElideMode(Qt::TextElideMode mode);

public:
    void setControlsCentering(bool controlsCentering = true); //Deprecated, user setContentAlignment instead
    bool isControlsCentering() const; //Deprecated
    void setControlsGrouping(bool controlsGrouping = true); //Deprecated, user RibbonToolBarControl instead
    bool isControlsGrouping() const; //Deprecated
    virtual QSize sizeHint() const;

public:
    using QWidget::addAction;
    using QWidget::insertAction;

Q_SIGNALS:
    /*! \internal */
    void released();
    /*! \internal */
    void actionTriggered(QAction*);
    /*! \internal */
    void hidePopup();

Q_SIGNALS:
    void titleChanged(const QString& title);
    void titleFontChanged(const QFont& font);

protected Q_SLOTS:
    /*! \internal */
    void currentIndexChanged(int index);

protected:
    virtual bool event(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void actionEvent(QActionEvent* event);
    virtual void enterEvent(QEvent* event);
    virtual void leaveEvent(QEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);

private:
    friend class RibbonPagePrivate;
    friend class RibbonGroupPopup;
    friend class RibbonGroupPopupButton;

    QTN_DECLARE_PRIVATE(RibbonGroup)
    Q_DISABLE_COPY(RibbonGroup)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONGROUP_H
