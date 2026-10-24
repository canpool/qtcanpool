/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTPASSWORDBOX_H
#define FLUENTPASSWORDBOX_H

#include "qxfluent_global.h"

#include "fluenttextbox.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentPasswordBoxPrivate;

/*!
 * Fluent's password box: the text box keeping a secret - the characters it
 * holds are drawn as the echo until the reader asks for them by name.
 *
 * Everything it is, it is as a text box: the face, the prompt, the accent
 * along the foot. What it adds is the answer in the room at the trailing
 * edge, where the text box keeps its clear: an eye that says "hold to
 * reveal". While the pointer holds it down, the characters stand as
 * themselves; the moment it lets go, they are the echo again. A secret
 * returns to hiding on its own - that is the whole of what a reveal button
 * is for.
 */
class QX_FLUENT_EXPORT FluentPasswordBox : public FluentTextBox
{
    Q_OBJECT
public:
    explicit FluentPasswordBox(QWidget *parent = Q_NULLPTR);
    ~FluentPasswordBox() override;

    /*! True while the reveal is held down and the characters stand as
     *  themselves. */
    bool isRevealed() const;
protected:
    void paintContent(QPainter *painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    bool clearWanted() const override;
private:
    Q_DISABLE_COPY(FluentPasswordBox)
    QX_DECLARE_PRIVATE(FluentPasswordBox)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTPASSWORDBOX_H
