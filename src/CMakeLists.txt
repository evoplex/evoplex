#### UTILS ####
SET(evoplex_utils
    utils/singleton.h
    utils/utils.h
)

#### GUI ####
SET(evoplex_gui_src
    gui/maingui.cpp
    gui/maingui.h
    gui/widgetproject.cpp
    gui/widgetproject.h
    gui/wizardnewproject.cpp
    gui/wizardnewproject.h
)
SET(evoplex_ui
    gui/forms/maingui.ui
    gui/forms/widgetproject.ui
    gui/forms/wizardnewproject.ui
)
QT5_WRAP_UI(evoplex_ui_h ${evoplex_ui})
SET(evoplex_gui ${evoplex_gui_src} ${evoplex_ui_h})

#### CORE ####
SET(evoplex_core_src
    core/interfaces.h
    core/mainapp.cpp
    core/mainapp.h

    core/graph.cpp
    core/graph.h
    core/processes.cpp
    core/processes.h
    core/project.cpp
    core/project.h
    core/simulation.cpp
    core/simulation.h
)

#INSTALL(FILES core/interfaces.h DESTINATION core/interfaces)

#### link src, gui and qt ####
ADD_LIBRARY(evoplex_lib SHARED ${evoplex_core_src} ${evoplex_gui} ${evoplex_utils})
TARGET_LINK_LIBRARIES(evoplex_lib Qt5::Core Qt5::Widgets)

#### EXECUTABLE ####
ADD_EXECUTABLE(evoplex main.cpp)
TARGET_LINK_LIBRARIES(evoplex evoplex_lib)
