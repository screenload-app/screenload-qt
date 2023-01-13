# Required to generate MOC
target_sources(
  flameshot
  PRIVATE abstractlogger.h
            filenamehandler.h
        screengrabber.h
        confighandler.h
        systemnotification.h
        valuehandler.h
        screenshotsaver.h
        globalvalues.h
        desktopfileparse.h
        desktopinfo.h
        pathinfo.h
        colorutils.h
        history.h
        strfparse.h
        request.h
)

target_sources(
  flameshot
  PRIVATE abstractlogger.cpp
        filenamehandler.cpp
        screengrabber.cpp
        confighandler.cpp
        systemnotification.cpp
        valuehandler.cpp
        screenshotsaver.cpp
        globalvalues.cpp
        desktopfileparse.cpp
        desktopinfo.cpp
        pathinfo.cpp
        colorutils.cpp
        history.cpp
        strfparse.cpp
        request.cpp
)
