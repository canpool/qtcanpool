/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALRATING_H
#define MATERIALRATING_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialRatingPrivate;

/*!
 * Material's rating: a row of stars, filled up to the one that was chosen.
 *
 * It asks the same two questions the fluent rating asks - how much is it now,
 * and what would it be if I clicked here - and Material answers the second one
 * in its own ink: the stars a click would fill are drawn in the accent thinned,
 * where fluent thins the page's ink. The other difference is the star itself.
 * Fluent draws a star as a line, so an empty one and a full one are the same
 * outline with a different inside; Material's is a solid, so a star that is on
 * has no outline of its own at all - the accent is both its edge and its inside.
 * An empty star is the only one that is drawn as a line, in the ink Material
 * keeps for what is not filled in.
 *
 * Not here: MUI's \c precision. A rating that counts in halves is a different
 * answer to a different question - the value stops being a count of stars, the
 * pointer has to be read per half, and an assistive tool has to say "three and a
 * half" - and both families here have only ever asked how many stars.
 *
 * \c readOnly is here, because a rating is not always the reader's to change: a
 * score shown on a review is the same picture with nothing to press.
 */
class QX_MATERIAL_EXPORT MaterialRating : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int count READ count WRITE setCount)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
public:
    explicit MaterialRating(QWidget *parent = Q_NULLPTR);
    ~MaterialRating() override;

    /*! How many stars are filled; 0 is "not rated". */
    int value() const;
    /*! How many stars there are. */
    int count() const;
    /*! True when the rating is a score to read rather than one to give. */
    bool isReadOnly() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setValue(int value);
    void setCount(int count);
    void setReadOnly(bool readOnly);

Q_SIGNALS:
    /*! Emitted whenever the rating changes, whatever changed it. */
    void valueChanged(int value);
protected:
    void paintContent(QPainter *painter) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    QAccessibleInterface *accessibleInterface() override;
private:
    Q_DISABLE_COPY(MaterialRating)
    QX_DECLARE_PRIVATE(MaterialRating)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALRATING_H
