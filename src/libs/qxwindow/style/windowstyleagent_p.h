#ifndef STYLEAGENTPRIVATE_H
#define STYLEAGENTPRIVATE_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QxWindow API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

#include "windowstyleagent.h"
#include <QtCore/QHash>

QX_WINDOW_BEGIN_NAMESPACE

class WindowStyleAgentPrivate : public QObject
{
    Q_DECLARE_PUBLIC(WindowStyleAgent)
public:
    WindowStyleAgentPrivate();
    ~WindowStyleAgentPrivate() override;

    void init();

    WindowStyleAgent *q_ptr;

    WindowStyleAgent::SystemTheme systemTheme = WindowStyleAgent::Unknown;

    void setupSystemThemeHook();
    void removeSystemThemeHook();

    void notifyThemeChanged(WindowStyleAgent::SystemTheme theme);
};

QX_WINDOW_END_NAMESPACE

#endif   // STYLEAGENTPRIVATE_H