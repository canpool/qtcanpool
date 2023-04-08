/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "commonstyle.h"
#include <QBasicTimer>
#include <QColor>
#include <QEvent>
#include <QTime>
#include <QWidget>

class QPainter;
class QSettings;
class QStyleOption;

QRIBBON_BEGIN_NAMESPACE

#define _qtc_Index      "Index"
#define _qtc_Wrap       "Wrap"
#define _qtc_Wrap_Group "WrapGroup"

#define _qtc_Begin_Group "BeginToolBarGroup"
#define _qtc_Group       "ToolBarGroup"
#define _qtc_End_Group   "EndToolBarGroup"

#define _qtc_Index_Group        "IndexGroup"
#define _qtc_Switch_Count       "SwitchCount"
#define _qtc_TitleGroupsVisible "TitleGroupsVisible"

#define _qtc_KeyTip "QRibbon::KeyTip"

#define _qtc_PopupButtonGallery      "PopupButtonGallery"
#define _qtc_ScrollUpButtonGallery   "ScrollUpButtonGallery"
#define _qtc_ScrollDownButtonGallery "ScrollDownButtonGallery"

#define _qtc_PopupBar         "PopupBar"
#define _qtc_PopupLable       "PopupLable_"
#define _qtc_SplitActionPopup "SplitActionPopup_"
#define _qtc_WidgetGallery    "WidgetGallery"
#define _qtc_StyleName        "StyleName"

/* StyleHelper */
class StyleHelper : public QObject
{
public:
    StyleHelper(QObject *pParent);
    virtual ~StyleHelper();
public:
    void openIniFile(const QString &fileName);
public:
    QString getValue(const QString &prefix, const QString &key) const;
    QColor getColor(const QString &prefix, const QString &key, QColor color = QColor()) const;
    int getInteger(const QString &prefix, const QString &key, int val) const;
public:
    static QPixmap alphaBlend(const QPixmap &src);
protected:
    QSettings *m_pSettings;
};

/* CommonAnimation */
class CommonAnimation : public QObject
{
public:
    CommonAnimation() : m_widget(0), m_running(true) {}
    virtual ~CommonAnimation();
public:
    QWidget *widget() const { return m_widget; }
    bool running() const { return m_running; }
    const QTime &startTime() const { return m_startTime; }
    void setRunning(bool val) { m_running = val; }
    void setWidget(QWidget *widget);
    void setStartTime(const QTime &startTime) { m_startTime = startTime; }
public:
    virtual void paint(QPainter *painter, const QStyleOption *option);
protected:
    void drawBlendedImage(QPainter *painter, QRect rect, float value);
    virtual bool eventFilter(QObject *obj, QEvent *event);
protected:
    QTime m_startTime;
    QWidget *m_widget;
    QImage m_primaryImage;
    QImage m_secondaryImage;
    QImage m_tempImage;
    bool m_running;
};

/* CommonTransition */
class CommonTransition : public CommonAnimation
{
public:
    CommonTransition();
    virtual ~CommonTransition();
public:
    void setDuration(int duration) { m_duration = duration; }
    void setStartImage(const QImage &image) { m_primaryImage = image; }
    void setEndImage(const QImage &image) { m_secondaryImage = image; }
    virtual void paint(QPainter *painter, const QStyleOption *option);
    int duration() const { return m_duration; }
public:
    int m_duration;
};

/* CommonPaintManager */
class CommonPaintManager : public QObject
{
public:
    CommonPaintManager(CommonStyle *baseStyle);
    virtual ~CommonPaintManager();
public:
    CommonStyle *baseStyle() const;
    StyleHelper &helper() const;
    QPixmap cached(const QString &img) const;
    QPixmap cachedPath(const QString &img) const;
    QRect sourceRectImage(QRect rcSrc, int state = 0, int count = 1) const;

    void drawImage(const QPixmap &soSrc, QPainter &p, const QRect &rcDest, const QRect &rcSrc, QRect rcSizingMargins,
                   QColor clrTransparent, bool alphaBlend = false) const;
    void drawImage(const QPixmap &dcSrc, QPainter &p, const QRect &rcDest, const QRect &rcSrc,
                   QRect rcSizingMargins = QRect(QPoint(0, 0), QPoint(0, 0)), bool alphaBlend = false) const;
    void drawPixmap(const QPixmap &dcSrc, QPainter &p, const QRect &rcDest, const QRect &rcSrc, bool alpha,
                    QRect rcSizingMargins = QRect(QPoint(0, 0), QPoint(0, 0)), bool alphaBlend = false) const;

    virtual void modifyColors() = 0;
protected:
    CommonStyle *m_base;
private:
    Q_DISABLE_COPY(CommonPaintManager)
};

/* CommonStylePrivate */
class CommonStylePrivate : public QObject
{
public:
    QTC_DECLARE_PUBLIC(CommonStyle)
public:
    explicit CommonStylePrivate();
    virtual ~CommonStylePrivate();
public:
    void initialization();

    void doTimer();
    void startAnimation(CommonAnimation *);
    void stopAnimation(const QWidget *);
    CommonAnimation *widgetAnimation(const QWidget *widget) const;

    void setPalette(QWidget *widget);
    void unsetPalette(QWidget *widget);

    void setAutoFillBackground(QWidget *widget);
    void unsetAutoFillBackground(QWidget *widget);

    void setPaintManager(CommonPaintManager &paintManager);
    virtual void makePaintManager() = 0;
public:
    CommonPaintManager *m_paintManager;
    QStyle *m_defaultStyle;
    StyleHelper *m_helper;
    QList<CommonAnimation *> m_animations;
    QBasicTimer m_animationTimer;
    QHash<const QWidget *, QPalette> m_customPaletteWidgets;   // widgets whose palette we tampered
    QHash<const QWidget *, bool> m_customAutoFillBackground;
};

#define QTC_D_STYLE(Class) Class##Private *d = static_cast<Class *>(baseStyle())->qtc_d();

QRIBBON_END_NAMESPACE
