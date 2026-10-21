/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentwindowmaterial.h"

#include <QWidget>

QX_FLUENT_USE_NAMESPACE

class tst_FluentWindowMaterial : public QObject
{
    Q_OBJECT
private slots:
    void whatWasAskedForIsWhatItIs();
    void aWindowThatIsNotAWindowIsNothing();
    void oneWindowOneMaterial();
    void askingForNothingIsAlwaysGiven();
    void theMaterialsAreOneSlot();
};

/*! A material is asked for by name, and what was asked for is what it is -
 *  whether or not the window system gave it, because a Windows 10 cannot give
 *  Mica and a Linux has none of them. */
void tst_FluentWindowMaterial::whatWasAskedForIsWhatItIs()
{
    QWidget window;
    FluentWindowMaterial material(&window);
    QCOMPARE(material.window(), &window);
    QCOMPARE(material.material(), FluentWindowMaterial::Material::None);

    const bool took = material.apply(FluentWindowMaterial::Material::Mica);
    QCOMPARE(material.material(), FluentWindowMaterial::Material::Mica);
    QCOMPARE(material.isApplied(), took);
}

/*! A window material belongs to a window; with no window there is nothing to
 *  ask and nothing is given. */
void tst_FluentWindowMaterial::aWindowThatIsNotAWindowIsNothing()
{
    FluentWindowMaterial material(Q_NULLPTR);
    QVERIFY(!material.apply(FluentWindowMaterial::Material::Mica));
    QVERIFY(!material.isApplied());
    QVERIFY(!applyWindowMaterial(Q_NULLPTR, FluentWindowMaterial::Material::Acrylic));
}

/*! A window has one material, so asking twice does not mean two of the things
 *  that ask for it: the one the window already had is found again. */
void tst_FluentWindowMaterial::oneWindowOneMaterial()
{
    QWidget window;
    const bool first = applyWindowMaterial(&window, FluentWindowMaterial::Material::Mica);
    const bool second = applyWindowMaterial(&window, FluentWindowMaterial::Material::Mica);
    QCOMPARE(second, first);
    QCOMPARE(window.findChildren<FluentWindowMaterial *>().count(), 1);
}

/*! Asking for no material needs nothing from the window system, so it is
 *  always given - which is what makes it safe to ask on any platform. */
void tst_FluentWindowMaterial::askingForNothingIsAlwaysGiven()
{
    QWidget window;
    FluentWindowMaterial material(&window);
    material.apply(FluentWindowMaterial::Material::Mica);

    QVERIFY(material.apply(FluentWindowMaterial::Material::None));
    QCOMPARE(material.material(), FluentWindowMaterial::Material::None);
    QVERIFY(material.isApplied());
}

/*! The three are one slot on a window, not three switches: what is asked for
 *  last is what it is, and asking does not stack. */
void tst_FluentWindowMaterial::theMaterialsAreOneSlot()
{
    QWidget window;
    FluentWindowMaterial material(&window);
    material.apply(FluentWindowMaterial::Material::Mica);
    material.apply(FluentWindowMaterial::Material::Acrylic);

    QCOMPARE(material.material(), FluentWindowMaterial::Material::Acrylic);

    material.apply(FluentWindowMaterial::Material::MicaAlt);
    QCOMPARE(material.material(), FluentWindowMaterial::Material::MicaAlt);
}

TEST_ADD(tst_FluentWindowMaterial)

#include "tst_fluentwindowmaterial.moc"
