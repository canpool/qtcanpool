/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include <QEvent>
#include <QIcon>
#include <QLabel>
#include <QTimer>

QRIBBON_BEGIN_NAMESPACE

class RibbonToolTipPrivate;
class QRIBBON_EXPORT RibbonToolTip : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(int margin READ margin WRITE setMargin)
public:
    RibbonToolTip(const QString &title, const QString &text, const QIcon &icon = QIcon(), QWidget *parent = Q_NULL);
    virtual ~RibbonToolTip();
    static RibbonToolTip *instance();
public:
    static void showToolTip(const QPoint &pos, const QString &title, const QString &text, const QIcon &icon = QIcon(),
                            QWidget *parent = Q_NULL);
    static void hideToolTip();
    static bool isToolTipVisible();
    // static QString textToolTip();

    enum WrapMode {
        NoWrap = 0,
        NativeWrap = NoWrap,
        AutoWrap,
    };

    static void setWrapMode(WrapMode mode);
    static WrapMode wordWrap();

    static void setIconVisible(bool visible);
    static bool isIconVisible();
public:
    int margin() const;
    void setMargin(int);

    const QString &title() const;
    const QString &text() const;

    void setIcon(const QIcon &icon);
    const QIcon &icon() const;
public:
    void reuseTip(const QString &textTitle, const QString &text);
    void hideTip();
    void closeToolTip();
    void setTipRect(QWidget *w, const QRect &r);
    void restartExpireTimer();
    bool tipChanged(const QPoint &pos, const QString &text, const QString &textTitle, QObject *o);
    void placeTip(const QPoint &pos, QWidget *w);

    static int getTipScreen(const QPoint &pos, QWidget *w);

public Q_SLOTS:
    void setTitle(const QString &);
    void setText(const QString &);
public:
    virtual QSize sizeHint() const;
protected:
    virtual bool eventFilter(QObject *, QEvent *event);
    virtual void timerEvent(QTimerEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
public:
    bool fadingOut() const;
private:
    QTC_DECLARE_PRIVATE(RibbonToolTip)
    Q_DISABLE_COPY(RibbonToolTip)
};

class RibbonKeyTipPrivate;
/* KeyTip */
class RibbonKeyTip : public QFrame
{
    Q_OBJECT
public:
    enum AlignTip {
        AlignTipTop = 0x00000000,
        AlignTipLeft = 0x00000000,
        AlignTipCenter = 0x00000001,
        AlignTipRight = 0x00000002,
        AlignTipVCenter = 0x00000004,
        AlignTipBottom = 0x00000008,
    };
public:
    RibbonKeyTip(QWidget *bars, QWidget *owner, const QString &caption, const QPoint &pt, uint align, bool enabled,
                 QAction *action = Q_NULL);
    virtual ~RibbonKeyTip();
public:
    QPoint posTip() const;
    void setVisibleTip(bool visible);
    bool isVisibleTip() const;
    bool isEnabledTip() const;

    QWidget *getBars() const;
    uint getAlign() const;

    void setExplicit(bool exp);
    bool isExplicit() const;

    QString getStringTip() const;
    void setStringTip(const QString &str);

    QString getCaption() const;
    void setCaption(const QString &str);

    QString getPrefix() const;
    void setPrefix(const QString &pref);

    QWidget *getOwner() const;
    QAction *getAction() const;
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
private:
    QTC_DECLARE_PRIVATE(RibbonKeyTip)
    Q_DISABLE_COPY(RibbonKeyTip)
};

QRIBBON_END_NAMESPACE
