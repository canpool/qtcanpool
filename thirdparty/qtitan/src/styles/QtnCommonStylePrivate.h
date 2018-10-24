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
#ifndef QTN_COMMONSTYLEPRIVATE_H
#define QTN_COMMONSTYLEPRIVATE_H

#include <QTime>
#include <QEvent>
#include <QWidget>
#include <QColor>

class QPainter;
class QSettings;
class QStyleOption;


namespace Qtitan
{
    /* StyleHelper */
    class StyleHelper : public QObject
    {
    public:
        StyleHelper(QObject* pParent);
        virtual ~StyleHelper();

    public:
        void openIniFile(const QString& fileName);

    public:
        QString getValue(const QString& prefix, const QString& key) const;
        QColor getColor(const QString& prefix, const QString& key, QColor color = QColor()) const;
        int getInteger(const QString& prefix, const QString& key, int val) const;

    public:
        static QPixmap alphaBlend(const QPixmap& src);

    protected:
        QSettings* m_pSettings;
    };

    /* QtitanAnimation */ 
    class QtitanAnimation : public QObject
    {
    public :
        QtitanAnimation() : _widget(0), _running(true){}
        virtual ~QtitanAnimation();
    public :
        QWidget* widget() const { return _widget; }
        bool running() const { return _running; }
        const QTime& startTime() const { return _startTime; }
        void setRunning(bool val) { _running = val; }
        void setWidget(QWidget* widget);
        void setStartTime(const QTime& startTime) { _startTime = startTime; }
    public :
        virtual void paint(QPainter* painter, const QStyleOption* option);

    protected:
        void drawBlendedImage(QPainter* painter, QRect rect, float value);
        virtual bool eventFilter(QObject* obj, QEvent* event);

    protected:
        QTime _startTime;
        QWidget* _widget;
        QImage _primaryImage;
        QImage _secondaryImage;
        QImage _tempImage;
        bool _running;
    };

    /* QtitanTransition */
    class QtitanTransition : public QtitanAnimation
    {
    public :
        QtitanTransition() : QtitanAnimation() {}
        virtual ~QtitanTransition() {}
    public :
        void setDuration(int duration) { _duration = duration; }
        void setStartImage(const QImage& image) { _primaryImage = image; }
        void setEndImage(const QImage& image) { _secondaryImage = image; }
        virtual void paint(QPainter* painter, const QStyleOption* option);
        int duration() const { return _duration; }
    public :
        int _duration;
    };
}; //namespace Qtitan

#endif // QTN_COMMONSTYLEPRIVATE_H
