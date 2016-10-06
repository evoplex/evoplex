#### UTILS ####
SET(evoplex_util
    util/singleton.h
    util/util.h
)

#### GUI ####
SET(evoplex_gui_src
    gui/maingui.cpp
    gui/maingui.h
)
SET(evoplex_ui
    gui/forms/maingui.ui
)
QT5_WRAP_UI(evoplex_ui_h ${evoplex_ui})
SET(evoplex_gui ${evoplex_gui_src} ${evoplex_ui_h})

#### CORE ####
SET(evoplex_core
    core/mainapp.cpp
    core/mainapp.h
)

#### link src, gui and qt ####
ADD_LIBRARY(evoplex_lib SHARED ${evoplex_core} ${evoplex_gui} ${evoplex_util})
TARGET_LINK_LIBRARIES(evoplex_lib Qt5::Core Qt5::Widgets)

#### EXECUTABLE ####
ADD_EXECUTABLE(evoplex main.cpp)
TARGET_LINK_LIBRARIES(evoplex evoplex_lib)
