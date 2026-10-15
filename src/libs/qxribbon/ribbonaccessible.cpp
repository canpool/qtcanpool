/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/

#include "ribbonbar_p.h"
#include "ribbongroup.h"
#include "ribbonpage.h"

#include <QAccessibleWidget>

QX_RIBBON_BEGIN_NAMESPACE

namespace
{

/*!
 * \brief Accessible interface for a ribbon page body.
 *
 * RibbonPage is a painted QWidget with no text of its own, so without an explicit
 * role the default accessibility reports a generic Client. Report Pane instead so
 * assistive tools announce the page as a named section, and let the default child
 * enumeration expose the groups it contains.
 */
class RibbonPageAccessible : public QAccessibleWidget
{
public:
    explicit RibbonPageAccessible(RibbonPage *page)
        : QAccessibleWidget(page, QAccessible::Pane)
    {
    }
};

/*!
 * \brief Accessible interface for a ribbon group.
 */
class RibbonGroupAccessible : public QAccessibleWidget
{
public:
    explicit RibbonGroupAccessible(RibbonGroup *group)
        : QAccessibleWidget(group, QAccessible::Grouping)
    {
    }
};

/*!
 * \brief Creates the ribbon-specific interfaces; returns null for anything else so
 *        the built-in factories keep handling the remaining widgets.
 */
QAccessibleInterface *ribbonAccessibleFactory(const QString &className, QObject *object)
{
    Q_UNUSED(className)
    if (auto *page = qobject_cast<RibbonPage *>(object)) {
        return new RibbonPageAccessible(page);
    }
    if (auto *group = qobject_cast<RibbonGroup *>(object)) {
        return new RibbonGroupAccessible(group);
    }
    return Q_NULLPTR;
}

}   // namespace

void installRibbonAccessibility()
{
    static bool installed = false;
    if (installed) {
        return;
    }
    installed = true;
    QAccessible::installFactory(&ribbonAccessibleFactory);
}

QX_RIBBON_END_NAMESPACE
