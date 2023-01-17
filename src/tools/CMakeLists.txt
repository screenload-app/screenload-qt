target_sources(screenload PRIVATE arrow/arrowtool.h arrow/arrowtool.cpp)
target_sources(screenload PRIVATE pixelate/pixelatetool.h pixelate/pixelatetool.cpp)
target_sources(screenload PRIVATE circle/circletool.h circle/circletool.cpp)
target_sources(screenload PRIVATE circlecount/circlecounttool.h circlecount/circlecounttool.cpp)
target_sources(screenload PRIVATE copy/copytool.h copy/copytool.cpp)
target_sources(screenload PRIVATE exit/exittool.h exit/exittool.cpp)
target_sources(screenload PRIVATE sizeincrease/sizeincreasetool.h sizeincrease/sizeincreasetool.cpp)
target_sources(screenload PRIVATE sizedecrease/sizedecreasetool.h sizedecrease/sizedecreasetool.cpp)
target_sources(
  screenload
        PRIVATE imgupload/storages/dwnldr/dwnldrauthhttpserver.h
        imgupload/storages/dwnldr/dwnldrauthhttpserver.cpp
        imgupload/storages/dwnldr/dwnldruploader.h
        imgupload/storages/dwnldr/dwnldruploader.cpp
        imgupload/storages/imgur/imguruploader.h
        imgupload/storages/imgur/imguruploader.cpp
        imgupload/storages/imguploaderbase.h
        imgupload/storages/imguploaderbase.cpp
        imgupload/imguploadertool.h
        imgupload/imguploadertool.cpp
        imgupload/imguploadermanager.h
        imgupload/imguploadermanager.cpp
)
target_sources(
  screenload
  PRIVATE launcher/applaunchertool.h
          launcher/applauncherwidget.h
          launcher/launcheritemdelegate.h
          launcher/terminallauncher.h
          launcher/applaunchertool.cpp
          launcher/applauncherwidget.cpp
          launcher/launcheritemdelegate.cpp
          launcher/openwithprogram.cpp
          launcher/terminallauncher.cpp)
target_sources(screenload PRIVATE line/linetool.h line/linetool.cpp)
target_sources(screenload PRIVATE marker/markertool.h marker/markertool.cpp)
target_sources(screenload PRIVATE move/movetool.h move/movetool.cpp)
target_sources(screenload PRIVATE pencil/penciltool.h pencil/penciltool.cpp)
target_sources(
  screenload
  PRIVATE pin/pintool.h
          pin/pinwidget.h
          pin/pintool.cpp
          pin/pinwidget.cpp)
target_sources(screenload PRIVATE rectangle/rectangletool.h rectangle/rectangletool.cpp)
target_sources(screenload PRIVATE redo/redotool.h redo/redotool.cpp)
target_sources(screenload PRIVATE save/savetool.h save/savetool.cpp)
target_sources(screenload PRIVATE accept/accepttool.h accept/accepttool.cpp)
target_sources(screenload PRIVATE invert/inverttool.h invert/inverttool.cpp)
target_sources(screenload PRIVATE selection/selectiontool.h selection/selectiontool.cpp)
target_sources(screenload PRIVATE sizeindicator/sizeindicatortool.h sizeindicator/sizeindicatortool.cpp)
target_sources(
  screenload
  PRIVATE text/textconfig.h
          text/texttool.h
          text/textwidget.h
          text/textconfig.cpp
          text/texttool.cpp
          text/textwidget.cpp)
target_sources(screenload PRIVATE undo/undotool.h undo/undotool.cpp)

target_sources(
  screenload
  PRIVATE abstractactiontool.cpp
          abstractpathtool.cpp
          abstracttwopointtool.cpp
          capturecontext.cpp
          toolfactory.cpp
          abstractactiontool.h
          abstractpathtool.h
          abstracttwopointtool.h
          capturetool.h
          toolfactory.h)
