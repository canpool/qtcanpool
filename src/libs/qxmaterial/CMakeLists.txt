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
    materialtooltip.h materialtooltip.cpp
    materialdivider.h materialdivider.cpp
    materialmenu.h materialmenu.cpp
    materiallist.h materiallist.cpp
    materiallistitem.h materiallistitem.cpp
    materialavatar.h materialavatar.cpp
    materialstepper.h materialstepper.cpp
    materialtabs.h materialtabs.cpp
    materialaccordion.h materialaccordion.cpp
    materialdatepicker.h materialdatepicker.cpp
    materialtimepicker.h materialtimepicker.cpp
    materialtable.h materialtable.cpp
    materialbottomnavigation.h materialbottomnavigation.cpp
    materialdrawer.h materialdrawer.cpp
    materialautocomplete.h materialautocomplete.cpp
    materialtogglebuttongroup.h materialtogglebuttongroup.cpp
    materialpagination.h materialpagination.cpp
    materialrating.h materialrating.cpp
    materialspeeddial.h materialspeeddial.cpp
    materialprogressring.h materialprogressring.cpp
    materialsnackbar.h materialsnackbar.cpp
    materialskeleton.h materialskeleton.cpp
    materialripple.h materialripple.cpp
    materialnavigationrail.h materialnavigationrail.cpp
    materialtopappbar.h materialtopappbar.cpp
)
