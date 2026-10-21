/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef GALLERYPAGE_H
#define GALLERYPAGE_H

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QWidget>

/*!
 * One style's page in the gallery: a column of rows, one control in each.
 *
 * The gallery exists because a control nobody ever looked at is a control with
 * no visual regression: qxdock paid for that with a whole round of catch-up in
 * 4.6. A page is therefore deliberately boring - caption on the left, control
 * on the right, nothing arranged for effect - so that a change in what a
 * control paints shows up as a change in the picture and not as a change in the
 * arrangement.
 *
 * What a row carries beyond the control is the id, and it is not decoration:
 * it is the object name the --inspect switch prints, and the name the visual
 * regression test crops the capture to. It is spelled once here, by the caller,
 * and everything downstream reads it back rather than guessing it from the
 * caption - a caption says what a thing is called in Fluent, an id says which
 * widget in this window it is, and the two are not the same string.
 */
class GalleryPage : public QWidget
{
    Q_OBJECT
public:
    explicit GalleryPage(const QString &styleId, QWidget *parent = Q_NULLPTR);

    /*! The id this page answers to on the --style switch: "fluent" or "material". */
    QString styleId() const;

    /*!
     * Adds a heading spanning the page; the rows that follow belong to it.
     *
     * A gallery of two dozen controls is unreadable without them, and they are
     * grouped the way the control sets themselves are - by what a control is
     * for, not by the order the classes happen to be in.
     */
    void addSection(const QString &text);

    /*!
     * Adds \a control as a row captioned \a caption and named \a id.
     *
     * The control is returned typed, so a caller configures the control it just
     * asked for without a cast and without a local of its own:
     * \code
     * page->addControl<FluentSwitch>("fluentSwitch", "FluentSwitch", new FluentSwitch)->setOn(true);
     * \endcode
     */
    template <typename Control> Control *addControl(const QString &id, const QString &caption, Control *control)
    {
        control->setObjectName(id);
        m_controls.append(control);
        m_form->addRow(caption, control);
        return control;
    }

    /*! Every control added, in the order they were added. */
    QList<QWidget *> controls() const;
private:
    QString m_styleId;
    QFormLayout *m_form = Q_NULLPTR;
    QList<QWidget *> m_controls;
};

#endif   // GALLERYPAGE_H
