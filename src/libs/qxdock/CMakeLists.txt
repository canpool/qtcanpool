file(GLOB DOCK_SOURCES "*.cpp")
file(GLOB DOCK_HEADERS "*.h")
file(GLOB DOCK_QRCFILES "*.qrc")

add_qtc_library(qxdock
  VERSION 0.1.0
  DEFINES QX_DOCK_LIBRARY
  DEPENDS Qt5::Core Qt5::Widgets
  SOURCES
    ${DOCK_SOURCES}
    ${DOCK_HEADERS}
    ${DOCK_QRCFILES}
)
