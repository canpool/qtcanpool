/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "officestyle.h"
#include "ribbon_global.h"
#include <QTimer>
#include <QWidget>

QRIBBON_BEGIN_NAMESPACE

enum PopupLocation {
#ifdef Q_OS_WIN
    PopupLocationNearTaskBar,
#endif   // Q_OS_WIN
    PopupLocationBottomRight,
    PopupLocationCenter
};

enum PopupAnimation {
    PopupAnimationNone,
    PopupAnimationFade,
    PopupAnimationSlide,
    PopupAnimationUnfold
};

class OfficePopupWindowPrivate;

class QRIBBON_EXPORT OfficePopupWindow : public QWidget
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
    OfficePopupWindow(QWidget *parent);
    virtual ~OfficePopupWindow();

    static void showPopup(QWidget *parent, const QIcon &titleIcon, const QString &titleText, const QString &bodyText,
                          bool closeButtonVisible = true,
                          OfficeStyle::OfficePopupDecoration decoration = OfficeStyle::PopupSystemDecoration);
public:
    bool showPopup();
    bool showPopup(const QPoint &pos);

    void setCentralWidget(QWidget *widget);
    QWidget *centralWidget() const;

    void setBodyText(const QString &text);
    const QString &bodyText() const;

    void setTitleIcon(const QIcon &icon);
    const QIcon &titleIcon() const;

    void setTitleText(const QString &text);
    const QString &titleText() const;

    void setCloseButtonPixmap(const QPixmap &pm);
    const QPixmap &closeButtonPixmap() const;

    void setTitleCloseButtonVisible(bool visible = true);
    bool isTitleCloseButtonVisible() const;

    QPoint getPosition() const;
    void setPosition(const QPoint &pos);

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

public Q_SLOTS:
    void closePopup();

protected Q_SLOTS:
    void showDelayTimer();
    void collapsingTimer();
    void expandingTimer();
public:
    virtual QSize sizeHint() const;
protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
private:
    QRN_DECLARE_PRIVATE(OfficePopupWindow)
    Q_DISABLE_COPY(OfficePopupWindow)
};

QRIBBON_END_NAMESPACE
