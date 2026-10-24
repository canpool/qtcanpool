/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALAVATAR_H
#define MATERIALAVATAR_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>
#include <QtGui/QPixmap>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialAvatarPrivate;

/*!
 * Material's avatar: the small face that stands for a person or a thing.
 *
 * It shows one of three sources, the first it was given winning: a picture,
 * an icon, or the name it was given - and a name is written as one letter,
 * its first, in capitals, which is what an avatar with no picture of its own
 * has to say.
 *
 * It comes in three sizes - twenty-four, forty and fifty-six - and three
 * shapes: the circle, the rounded square, and the plain square. The stand is
 * the family's primary, the accent its commands wear, or the page's own
 * variant when the quieter tone is asked for; what stands on it is white,
 * the way the tick of a chosen box is.
 *
 * An avatar is a face, not a control: it answers no press, keeps no focus
 * ring and takes no wash, so an icon of a person in a list does not look
 * like something to click.
 */
class QX_MATERIAL_EXPORT MaterialAvatar : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QPixmap picture READ picture WRITE setPicture NOTIFY pictureChanged)
    Q_PROPERTY(Size avatarSize READ avatarSize WRITE setAvatarSize NOTIFY avatarSizeChanged)
    Q_PROPERTY(Shape shape READ shape WRITE setShape NOTIFY shapeChanged)
    Q_PROPERTY(Tone tone READ tone WRITE setTone NOTIFY toneChanged)
public:
    /*! The three sizes an avatar comes in. */
    enum class Size {
        Small,  /*!< 24 - beside a line of text that is telling the story. */
        Medium, /*!< 40 - the stand-alone one. */
        Large   /*!< 56 - the one that is the page's subject. */
    };
    Q_ENUM(Size)

    /*! The three shapes an avatar comes in. */
    enum class Shape {
        Circle,  /*!< Fully round. */
        Rounded, /*!< A square with the smallest corner the family has. */
        Square   /*!< No corner at all. */
    };
    Q_ENUM(Shape)

    /*! Which of the family's two surfaces the avatar stands on. */
    enum class Tone {
        Primary, /*!< The accent the commands wear. */
        Neutral  /*!< The page's own variant - the quiet one. */
    };
    Q_ENUM(Tone)

    explicit MaterialAvatar(QWidget *parent = Q_NULLPTR);
    ~MaterialAvatar() override;

    /*! The name the avatar stands for; its first letter is what it writes. */
    QString name() const;
    /*! The letter a name comes out as: the first one, in capitals. */
    QString initials() const;
    /*! The icon shown when there is no picture. */
    QIcon icon() const;
    /*! The picture shown over everything else. */
    QPixmap picture() const;
    /*! How big the avatar is. It is not the widget's own measure: an avatar
     *  keeps that square, and this is which square. */
    Size avatarSize() const;
    /*! How the avatar's edge is cut. */
    Shape shape() const;
    /*! Which surface the avatar stands on. */
    Tone tone() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setName(const QString &name);
    void setIcon(const QIcon &icon);
    void setPicture(const QPixmap &picture);
    void setAvatarSize(Size avatarSize);
    void setShape(Shape shape);
    void setTone(Tone tone);
Q_SIGNALS:
    void nameChanged(const QString &name);
    void iconChanged(const QIcon &icon);
    void pictureChanged(const QPixmap &picture);
    void avatarSizeChanged(Size avatarSize);
    void shapeChanged(Shape shape);
    void toneChanged(Tone tone);
protected:
    qreal cornerRadius() const override;
    QRectF chromeRect() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    QAccessible::Role accessibleRole() const override;
    void fillAccessibleState(QAccessible::State *state) const override;
private:
    Q_DISABLE_COPY(MaterialAvatar)
    QX_DECLARE_PRIVATE(MaterialAvatar)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALAVATAR_H
