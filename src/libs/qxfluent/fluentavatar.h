/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTAVATAR_H
#define FLUENTAVATAR_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>
#include <QtGui/QPixmap>

QX_FLUENT_BEGIN_NAMESPACE

class FluentAvatarPrivate;

/*!
 * Fluent's avatar - what WinUI calls a person picture: a face for a name, round
 * and always round, in one of three sizes.
 *
 * It shows the picture when there is one and the name's initials when there is
 * not, and the name's initials are the first letters of its first two words -
 * "Ada Lovelace" is AL, which is how a person is shortened when the face is
 * small. The stand is the page's ink at a tenth over the page, the initials the
 * page's ink: a quiet face that does not compete with the row it stands beside.
 *
 * An avatar is a face and not a control: it answers no press, keeps no focus
 * ring and takes no wash. An icon of a person in a list must not look like
 * something to click.
 *
 * \sa FluentListView
 */
class QX_FLUENT_EXPORT FluentAvatar : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QPixmap picture READ picture WRITE setPicture NOTIFY pictureChanged)
    Q_PROPERTY(Size avatarSize READ avatarSize WRITE setAvatarSize NOTIFY avatarSizeChanged)
public:
    /*! The three sizes an avatar comes in. */
    enum class Size {
        Small,  /*!< 24 - beside a line of text that is telling the story. */
        Medium, /*!< 40 - the stand-alone one. */
        Large   /*!< 56 - the one that is the page's subject. */
    };
    Q_ENUM(Size)

    explicit FluentAvatar(QWidget *parent = Q_NULLPTR);
    ~FluentAvatar() override;

    /*! The name the avatar stands for. */
    QString name() const;
    /*! What the name comes out as: the first letters of its first two words,
     *  in capitals - and the first letter alone when the name is one word. */
    QString initials() const;
    /*! The picture shown over everything else. */
    QPixmap picture() const;
    /*! How big the avatar is. It is not the widget's own measure: an avatar
     *  keeps that square, and this is which square. */
    Size avatarSize() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setName(const QString &name);
    void setPicture(const QPixmap &picture);
    void setAvatarSize(Size size);
Q_SIGNALS:
    void nameChanged();
    void pictureChanged();
    void avatarSizeChanged();
protected:
    /*! Nothing: a face takes no wash. */
    QRectF chromeRect() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    QAccessible::Role accessibleRole() const override;
    void fillAccessibleState(QAccessible::State *state) const override;
private:
    Q_DISABLE_COPY(FluentAvatar)
    QX_DECLARE_PRIVATE(FluentAvatar)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTAVATAR_H
