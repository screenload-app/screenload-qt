target_sources(screenload PRIVATE
    capturerequest.h
    screenload.h
    screenloaddaemon.h
    screenloaddbusadapter.h
    qguiappcurrentscreen.h
)

target_sources(screenload PRIVATE
    capturerequest.cpp
    screenload.cpp
    screenloaddaemon.cpp
    screenloaddbusadapter.cpp
    qguiappcurrentscreen.cpp
)

IF (WIN32)
    target_sources(screenload PRIVATE globalshortcutfilter.h globalshortcutfilter.cpp)
ENDIF ()
