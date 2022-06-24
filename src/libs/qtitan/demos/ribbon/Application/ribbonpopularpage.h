#ifndef RIBBONPOPULARPAGE_H
#define RIBBONPOPULARPAGE_H

#include <QWidget>

#include <QtitanRibbon.h>

#include "mainwindow.h"
#include "ui_ribbonpopularpage.h"
#include "QtitanDef.h"


/* RibbonPopularPage */
class PreviewColor : public QWidget
{
public:
    PreviewColor(QWidget* parent);
    virtual ~PreviewColor();

public:
    const QColor& color() const;
    void setColor(const QColor& color);

protected:
    virtual void paintEvent(QPaintEvent* event);

protected:
    QColor m_color;
};

/* RibbonPopularPage */
class RibbonPopularPage : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonPopularPage(MainWindow* mainWindow, QWidget* parent = Q_NULL);
    virtual ~RibbonPopularPage();

protected:
    void setupPage();
    void updateEnabledWidgets();

public Q_SLOTS:
    void currentThemeChanged(int);
    void currentBackgroundChanged(int);
    void showColorDialog();
    void accepted();

protected:
    virtual void showEvent(QShowEvent* event);

private:
    Ui::pageCustomizePopular m_pageRes;
    PreviewColor* m_previewColor;

    Qtitan::RibbonStyle::Theme m_currentThemeId;
    QColor m_accentColor;
    QString m_curRibbonBackground;
    MainWindow* m_mainWindow;
    bool m_wasDisplayed;

private:
    Q_DISABLE_COPY(RibbonPopularPage)

};


#endif // RIBBONPOPULARPAGE_H
