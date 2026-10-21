/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentwindowmaterial.h"

#if defined(QX_FLUENT_HAS_WINDOW)
#include "qxwindow/windowagentwidget.h"
#endif

#include <QtWidgets/QWidget>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// The keys the window system knows these by, which is qxwindow's vocabulary
// rather than Fluent's - the whole content of this wrapper is the spelling
// between the two.
const auto kMica = QStringLiteral("mica");
const auto kMicaAlt = QStringLiteral("mica-alt");
const auto kAcrylic = QStringLiteral("acrylic-material");

QString keyFor(FluentWindowMaterial::Material material)
{
    switch (material) {
    case FluentWindowMaterial::Material::Mica:
        return kMica;
    case FluentWindowMaterial::Material::MicaAlt:
        return kMicaAlt;
    case FluentWindowMaterial::Material::Acrylic:
        return kAcrylic;
    default:
        return QString();
    }
}
}   // namespace

class FluentWindowMaterialPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentWindowMaterial)

    QWidget *window = Q_NULLPTR;
    FluentWindowMaterial::Material material = FluentWindowMaterial::Material::None;
    bool applied = false;
#if defined(QX_FLUENT_HAS_WINDOW)
    QxWindow::WindowAgentWidget *agent = Q_NULLPTR;
    bool setup = false;
#endif

    /*! The agent is made when the first material is asked for and not before:
     *  an application that never asks for one should not be carrying a window
     *  agent it has no use for. */
    bool ensureAgent()
    {
#if !defined(QX_FLUENT_HAS_WINDOW)
        return false;
#else
        Q_Q(FluentWindowMaterial);
        if (!window) {
            return false;
        }
        if (!agent) {
            agent = new QxWindow::WindowAgentWidget(q);
        }
        if (!setup) {
            setup = agent->setup(window);
        }
        return setup;
#endif
    }
};

FluentWindowMaterial::FluentWindowMaterial(QWidget *window)
    : QObject(window)
    , d_ptr(new FluentWindowMaterialPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->window = window;
}

FluentWindowMaterial::~FluentWindowMaterial(){QX_FINI_PRIVATE()}

QWidget *FluentWindowMaterial::window() const
{
    return d_ptr->window;
}

FluentWindowMaterial::Material FluentWindowMaterial::material() const
{
    return d_ptr->material;
}

bool FluentWindowMaterial::isApplied() const
{
    return d_ptr->applied;
}

bool FluentWindowMaterial::apply(Material material)
{
    d_ptr->material = material;
    if (!d_ptr->ensureAgent()) {
        d_ptr->applied = false;
        return false;
    }

    const QString wanted = keyFor(material);
#if defined(QX_FLUENT_HAS_WINDOW)
    QxWindow::WindowAgentWidget *agent = d_ptr->agent;
    // One slot, not three switches: whatever else was on has to go, and only
    // what is actually on is touched - a platform that has never heard of a
    // material is not asked to turn it off.
    for (const QString &each : {kMica, kMicaAlt, kAcrylic}) {
        if (each != wanted && agent->windowAttribute(each).toBool()) {
            agent->setWindowAttribute(each, false);
        }
    }
    if (wanted.isEmpty()) {
        // Asking for nothing needs nothing from the window system, and always
        // succeeds - which is what makes it safe to call on any platform.
        d_ptr->applied = true;
        return true;
    }
    d_ptr->applied = agent->setWindowAttribute(wanted, true);
    return d_ptr->applied;
#else
    Q_UNUSED(wanted)
    d_ptr->applied = false;
    return false;
#endif
}

bool applyWindowMaterial(QWidget *window, FluentWindowMaterial::Material material)
{
    if (!window) {
        return false;
    }
    // A window has one material, so the thing that asks for it is the window's
    // own: found again rather than made again.
    FluentWindowMaterial *holder = window->findChild<FluentWindowMaterial *>();
    if (!holder) {
        holder = new FluentWindowMaterial(window);
    }
    return holder->apply(material);
}

QX_FLUENT_END_NAMESPACE
