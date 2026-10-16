/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"

#include <QWidget>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonSliderPanePrivate;

/**
 * @brief The Office zoom control: a minus button, a slider track and a plus button.
 *
 * RibbonSliderPane is the compact zoom control a ribbon application puts at the
 * right end of its status bar: two scroll buttons around a slider track, wired so
 * that the buttons move the position by a single step and the track reports every
 * move. It is a plain widget, so a host that wants it next to its view switches
 * adds it with QStatusBar::addPermanentWidget(), where a message never pushes it
 * out.
 *
 * The value and the position are two views of one thing, as for QSlider: the
 * position moves continuously while the user drags, the value is what the control
 * settles on. A host that only cares about the zoom level connects to
 * @ref valueChanged; one that wants every intermediate step connects to
 * @ref sliderMoved.
 *
 * Nothing here paints: the control is built from stock widgets that carry the
 * library's object names, so a theme styles it the way it styles the rest of the
 * ribbon. The scroll buttons draw their glyphs from the palette, so they read on
 * a dark theme without a theme carrying icons for them.
 */
class QX_RIBBON_EXPORT RibbonSliderPane : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int singleStep READ singleStep WRITE setSingleStep)
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged USER true)
    Q_PROPERTY(int sliderPosition READ sliderPosition WRITE setSliderPosition NOTIFY sliderMoved)
public:
    explicit RibbonSliderPane(QWidget *parent = nullptr);
    ~RibbonSliderPane();

    /// Sets the range the slider track and the scroll buttons work in.
    void setRange(int min, int max);

    /**
     * @brief Whether the minus and plus buttons are shown
     *
     * The default is true. A host that only wants the track turns them off.
     */
    void setScrollButtons(bool on);

    void setSliderPosition(int position);
    int sliderPosition() const;

    void setSingleStep(int step);
    int singleStep() const;

    int value() const;
public Q_SLOTS:
    /// Sets the value; clamped to the range. Emits valueChanged() when it moves.
    void setValue(int value);
Q_SIGNALS:
    /// Emitted when the value has settled on a new level.
    void valueChanged(int value);
    /// Emitted for every step the position takes while the user drags.
    void sliderMoved(int position);
protected Q_SLOTS:
    /// Moves the position one single step down; wired to the minus button.
    void decrement();
    /// Moves the position one single step up; wired to the plus button.
    void increment();
protected:
    void changeEvent(QEvent *event) override;
private:
    friend class RibbonSliderPanePrivate;

    QX_DECLARE_PRIVATE(RibbonSliderPane)
};

QX_RIBBON_END_NAMESPACE
