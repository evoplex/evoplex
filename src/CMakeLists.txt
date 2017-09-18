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
    core/edge.h

    core/mainapp.cpp
    core/mainapp.h

    core/filemgr.cpp
    core/filemgr.h
    core/experiment.cpp
    core/experiment.h
    core/experimentsmgr.cpp
    core/experimentsmgr.h
    core/project.cpp
    core/project.h
)
ADD_LIBRARY(evoplex_core_lib SHARED ${evoplex_core_src} ${evoplex_utils})
TARGET_LINK_LIBRARIES(evoplex_core_lib Qt5::Core Qt5::Concurrent Qt5::Widgets)


#### GUI ####
SET(evoplex_gui_src
    gui/maingui.cpp
    gui/maingui.h
    gui/attributeswidget.cpp
    gui/attributeswidget.h
    gui/experimentwidget.cpp
    gui/experimentwidget.h
    gui/graphwidget.cpp
    gui/graphwidget.h
    gui/projectwidget.cpp
    gui/projectwidget.h
    gui/projectswindow.cpp
    gui/projectswindow.h
    gui/queuewidget.cpp
    gui/queuewidget.h
    gui/tablewidget.cpp
    gui/tablewidget.h
    gui/welcomewidget.cpp
    gui/welcomewidget.h

    #gui/contextmenutable.cpp
    #gui/contextmenutable.h
)
SET(evoplex_ui
    gui/forms/attributeswidget.ui
    gui/forms/queuewidget.ui
    gui/forms/projectwidget.ui
    gui/forms/welcomewidget.ui
)
QT5_WRAP_UI(evoplex_ui_h ${evoplex_ui})
ADD_LIBRARY(evoplex_gui_lib SHARED ${evoplex_gui_src} ${evoplex_ui_h})
TARGET_LINK_LIBRARIES(evoplex_gui_lib evoplex_core_lib Qt5::Core Qt5::Widgets)

#### EXECUTABLE ####
ADD_EXECUTABLE(evoplex main.cpp gui/icons/icons.qrc)
TARGET_LINK_LIBRARIES(evoplex evoplex_core_lib evoplex_gui_lib)
