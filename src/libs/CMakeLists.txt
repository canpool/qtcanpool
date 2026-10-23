option(WITH_QXCORE "Build qxcore library" ON)
option(WITH_QXPLUGIN "Build qxplugin library" ON)
option(WITH_QXTHEME "Build qxtheme library" ON)
option(WITH_QXCONTROLS "Build qxcontrols library" ON)
option(WITH_QXFLUENT "Build qxfluent library" ON)
option(WITH_QXMATERIAL "Build qxmaterial library" ON)
option(WITH_QXCUPERTINO "Build qxcupertino library" ON)
option(WITH_QXHARMONY "Build qxharmony library" ON)
option(WITH_QXRIBBON "Build qxribbon library" ON)
option(WITH_QXDOCK   "Build qxdock library" ON)
option(WITH_QXWINDOW "Build qxwindow library" ON)
option(WITH_QXAPP "Build qxapp library" ON)

# qxcore is the infrastructure layer: it depends on Qt Core only, and every
# other library may build on top of it.
if (WITH_QXCORE)
  add_subdirectory(qxcore)
endif()

# qxplugin is the plugin runtime: the contract (QxPlugin, QxPluginContext) and
# the loader (QxPluginManager). It builds on qxcore for the settings object the
# host context exposes, and on nothing above it, so a plugin links only qxplugin
# and never the application framework.
if (WITH_QXPLUGIN AND WITH_QXCORE)
  add_subdirectory(qxplugin)
endif()

# qxtheme builds on qxcore (settings) and needs Qt Widgets (palette, style
# sheet). It is deliberately independent from qxribbon: the built-in style
# sheets are resolved from the qxribbon resource bundle at run time, so the
# theme engine stays usable in applications that do not use the ribbon.
if (WITH_QXTHEME AND WITH_QXCORE)
  add_subdirectory(qxtheme)
endif()

# qxcontrols is the shared base the control libraries draw on: state layers,
# focus rings, shadows - the mechanisms, not any family's form. It reads the
# token vocabularies from qxtheme and nothing above it, so the dependency
# direction stays one-way: qxtheme <- qxcontrols <- qxfluent / qxmaterial.
if (WITH_QXCONTROLS AND WITH_QXTHEME AND WITH_QXCORE)
  add_subdirectory(qxcontrols)
endif()

# qxfluent is one of the two control libraries: Fluent's own shapes, ramp,
# shadows and durations, drawn on the base qxcontrols supplies. It knows
# qxcontrols and qxtheme and nothing else - the two control libraries do not
# know about each other. The one exception is window material (plan 4.7 C3),
# which is a wrapper over qxwindow's Mica / Acrylic / Mica Alt and is off
# entirely when qxwindow is not built - see the comment in qxfluent's own file.
if (WITH_QXFLUENT AND WITH_QXCONTROLS AND WITH_QXTHEME AND WITH_QXCORE)
  add_subdirectory(qxfluent)
endif()

# qxmaterial is the other control library, and it is qxfluent's peer rather
# than its sibling: same base, same rules, no knowledge of it. The two answer
# the same questions differently, so nothing they both use is a form - only a
# mechanism, and those live in qxcontrols.
if (WITH_QXMATERIAL AND WITH_QXCONTROLS AND WITH_QXTHEME AND WITH_QXCORE)
  add_subdirectory(qxmaterial)
endif()

# qxcupertino is a third control library on the same base and with the same
# rules: it knows qxcontrols and qxtheme, and it knows nothing of the other
# families - not their names, not their tokens, not their shapes. What it adds
# is a different answer to the same questions, which is why nothing two
# families both use is a form; the forms stay in each family and only the
# mechanisms live in qxcontrols.
if (WITH_QXCUPERTINO AND WITH_QXCONTROLS AND WITH_QXTHEME AND WITH_QXCORE)
  add_subdirectory(qxcupertino)
endif()

# qxharmony is the fourth, on the same base and under the same rule: it reads
# qxcontrols and qxtheme and knows no other family. Four families answering the
# same questions four ways is the point - what they share is a mechanism, and
# the mechanisms are in qxcontrols; every form stays home.
if (WITH_QXHARMONY AND WITH_QXCONTROLS AND WITH_QXTHEME AND WITH_QXCORE)
  add_subdirectory(qxharmony)
endif()

if (WITH_QXRIBBON)
  add_subdirectory(qxribbon)
endif()

if (WITH_QXDOCK)
  add_subdirectory(qxdock)
endif()

if (WITH_QXWINDOW)
  add_subdirectory(qxwindow)
endif()

# qxapp is the application framework: it carries the ribbon window and the
# AppShell, which combines the navigation rail, the dock system, the theme
# engine and the settings, so it needs every layer underneath it.
if (WITH_QXAPP AND WITH_QXRIBBON AND WITH_QXWINDOW AND WITH_QXDOCK AND WITH_QXTHEME AND WITH_QXCORE)
  add_subdirectory(qxapp)
endif()
