/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALSEGMENTEDBUTTON_H
#define MATERIALSEGMENTEDBUTTON_H

#include "qxmaterial_global.h"

#include <QtWidgets/QWidget>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialSegmentedButtonPrivate;

/*!
 * Material's segmented button: a row of choices, one of which is in force.
 *
 * Qt has radio buttons, and a radio group answers this for a form - which is
 * not where a segmented button lives: it is a row inside a toolbar or a
 * header, where a group of radio buttons would arrive with a frame, spacing
 * and a shape of its own. This is one widget with one outline round the whole
 * row, divided into equal parts, and the part in force is the one that is
 * filled.
 *
 * Material keeps the outline round the row and fills only the segment in
 * force, which is what says "one of these" - the fill is the answer, not a
 * marker travelling between the labels the way another family draws it.
 */
class QX_MATERIAL_EXPORT MaterialSegmentedButton : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString currentText READ currentText)
public:
    explicit MaterialSegmentedButton(QWidget *parent = Q_NULLPTR);
    ~MaterialSegmentedButton() override;

    /*! Replace the choices; the one in force falls back to the first. */
    void setItems(const QStringList &items);
    QStringList items() const;
    int count() const;

    int currentIndex() const;
    /*! The text of the segment in force; empty while there is none. */
    QString currentText() const;
    /*! Where segment \a index stands, in the row's own coordinates. Widths are
     *  the words' own with the spare room shared out, so a caller that puts
     *  something on a segment - a count, a mark - knows where to put it. */
    QRect segmentRect(int index) const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setCurrentIndex(int index);

Q_SIGNALS:
    /*! Emitted when the segment in force changed, whoever changed it. */
    void currentIndexChanged(int index);
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;
private:
    Q_DISABLE_COPY(MaterialSegmentedButton)
    QX_DECLARE_PRIVATE(MaterialSegmentedButton)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALSEGMENTEDBUTTON_H
