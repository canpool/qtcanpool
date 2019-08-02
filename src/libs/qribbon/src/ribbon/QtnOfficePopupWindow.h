/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2011 Developer Machines (http://www.devmachines.com)
** Copyright (c) 2019 MaMinJie <canpool@163.com>
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
#ifndef QTN_OFFICEPOPUPWINDOW_H
#define QTN_OFFICEPOPUPWINDOW_H

#include <QWidget>
#include <QTimer>

#include "QtitanDef.h"
#include "QtnOfficeStyle.h"

QTITAN_BEGIN_NAMESPACE

enum PopupLocation
{
#ifdef Q_OS_WIN
    PopupLocationNearTaskBar,
#endif // Q_OS_WIN
    PopupLocationBottomRight,
    PopupLocationCenter
};

enum PopupAnimation
{
    PopupAnimationNone,
    PopupAnimationFade,
    PopupAnimationSlide,
    PopupAnimationUnfold
};

class OfficePopupWindowPrivate;
/* OfficePopupWindow */
class QTITAN_EXPORT OfficePopupWindow : public QWidget
{
    Q_OBJECT
    Q_ENUMS(PopupAnimation)
    Q_ENUMS(PopupLocation)
    Q_PROPERTY(PopupAnimation animation READ animation WRITE setAnimation)
    Q_PROPERTY(qreal transparency READ transparency WRITE setTransparency)
    Q_PROPERTY(int displayTime READ displayTime WRITE setDisplayTime)
    Q_PROPERTY(int animationSpeed READ animationSpeed WRITE setAnimationSpeed)
    Q_PROPERTY(QIcon titleIcon READ titleIcon WRITE setTitleIcon)
    Q_PROPERTY(QString titleText READ titleText WRITE setTitleText)
    Q_PROPERTY(QString bodyText READ bodyText WRITE setBodyText)
    Q_PROPERTY(bool titleCloseButtonVisible READ isTitleCloseButtonVisible WRITE setTitleCloseButtonVisible)
    Q_PROPERTY(PopupLocation location READ location WRITE setLocation)
public:
    OfficePopupWindow(QWidget* parent);
    virtual ~OfficePopupWindow();

    static void showPopup(QWidget* parent, const QIcon& titleIcon, const QString& titleText, const QString& bodyText,
        bool closeButtonVisible = true, OfficeStyle::OfficePopupDecoration decoration = OfficeStyle::PopupSystemDecoration);

public:
    bool showPopup();
    bool showPopup(const QPoint& pos);

    void setCentralWidget(QWidget* widget);
    QWidget* centralWidget() const;

    void setBodyText(const QString& text);
    const QString& bodyText() const;

    void setTitleIcon(const QIcon& icon);
    const QIcon& titleIcon() const;

    void setTitleText(const QString& text);
    const QString& titleText() const;

    void setCloseButtonPixmap(const QPixmap& pm);
    const QPixmap& closeButtonPixmap() const;

    void setTitleCloseButtonVisible(bool visible = true);
    bool isTitleCloseButtonVisible() const;

    QPoint getPosition() const;
    void setPosition(const QPoint& pos);

    void setAnimation(PopupAnimation popupAnimation);
    PopupAnimation animation() const;

    void setTransparency(qreal transparency);
    qreal transparency() const;

    void setDisplayTime(int time);
    int displayTime() const;

    void setAnimationSpeed(int time);
    int animationSpeed() const;

    void setDragDropEnabled(bool enabled);
    bool dragDropEnabled() const;

    void setLocation(PopupLocation location);
    PopupLocation location() const;

Q_SIGNALS:
    void aboutToShow();
    void aboutToHide();

public slots:
    void closePopup();

protected slots:
    void showDelayTimer();
    void collapsingTimer();
    void expandingTimer();

public:
    virtual QSize sizeHint() const;

protected:
    virtual bool event(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void enterEvent(QEvent* event);
    virtual void leaveEvent(QEvent* event);
private:
    QTN_DECLARE_PRIVATE(OfficePopupWindow)
    Q_DISABLE_COPY(OfficePopupWindow)
};

QTITAN_END_NAMESPACE

#endif // QTN_OFFICEPOPUPWINDOW_H
