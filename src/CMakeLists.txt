#### UTILS ####
SET(evoplex_utils
    utils/constants.h
    utils/singleton.h
    utils/prg.h
    utils/utils.h
)
#### CORE ####
SET(evoplex_core_src
    core/abstractagent.h
    core/abstractgraph.h
    core/abstractmodel.h

    core/mainapp.cpp
    core/mainapp.h

    core/filemgr.cpp
    core/filemgr.h
    core/experiment.cpp
    core/experiment.h
    core/project.cpp
    core/project.h
    core/trial.cpp
    core/trial.h
    core/trialsmgr.cpp
    core/trialsmgr.h
)
ADD_LIBRARY(evoplex_core_lib SHARED ${evoplex_core_src} ${evoplex_utils})
TARGET_LINK_LIBRARIES(evoplex_core_lib Qt5::Core Qt5::Widgets)


#### GUI ####
SET(evoplex_gui_src
    gui/maingui.cpp
    gui/maingui.h
    gui/contextmenutable.cpp
    gui/contextmenutable.h
    gui/experimentview.cpp
    gui/experimentview.h
    gui/widgetgraph.cpp
    gui/widgetgraph.h
    gui/widgetproject.cpp
    gui/widgetproject.h
    gui/wizardnewproject.cpp
    gui/wizardnewproject.h
)
SET(evoplex_ui
    gui/forms/maingui.ui
    gui/forms/experimentview.ui
    gui/forms/widgetproject.ui
    gui/forms/wizardnewproject.ui
)
QT5_WRAP_UI(evoplex_ui_h ${evoplex_ui})
ADD_LIBRARY(evoplex_gui_lib SHARED ${evoplex_gui_src} ${evoplex_ui_h})
TARGET_LINK_LIBRARIES(evoplex_gui_lib evoplex_core_lib Qt5::Core Qt5::Widgets)


#### EXECUTABLE ####
ADD_EXECUTABLE(evoplex main.cpp)
TARGET_LINK_LIBRARIES(evoplex evoplex_core_lib evoplex_gui_lib)
