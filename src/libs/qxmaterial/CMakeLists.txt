add_qtc_library(qxmaterial
  VERSION 0.1.2
  DEFINES QX_MATERIAL_LIBRARY
  PUBLIC_DEPENDS qxcontrols Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxmaterial_global.h
    materialtokens.h materialtokens.cpp
    materialswitch.h materialswitch.cpp
    materialslider.h materialslider.cpp
    materialbadge.h materialbadge.cpp
    materialbutton.h materialbutton.cpp
    materialsegmentedbutton.h materialsegmentedbutton.cpp
    materialselect.h materialselect.cpp
    materialchip.h materialchip.cpp
    materialcard.h materialcard.cpp
    materialcheckbox.h materialcheckbox.cpp
    materialdialog.h materialdialog.cpp
    materialradio.h materialradio.cpp
    materialtextfield.h materialtextfield.cpp
    materialfab.h materialfab.cpp
    materialprogress.h materialprogress.cpp
    materialprogressring.h materialprogressring.cpp
    materialsnackbar.h materialsnackbar.cpp
    materialskeleton.h materialskeleton.cpp
    materialripple.h materialripple.cpp
    materialnavigationrail.h materialnavigationrail.cpp
    materialtopappbar.h materialtopappbar.cpp
)
