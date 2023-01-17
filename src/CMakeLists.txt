find_package(
        Qt5
        CONFIG
        REQUIRED
        Core
        Gui
        Widgets
        Network
        Svg
        DBus
        LinguistTools)

if (USE_WAYLAND_CLIPBOARD)
    find_package(KF5GuiAddons)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# set application icon
if (APPLE)
    # generate iconset
    execute_process(
            COMMAND bash "-c" "mkdir -p screenload.iconset"
    )
    execute_process(
            COMMAND bash "-c" "sips -z 16 16     \"${CMAKE_SOURCE_DIR}\"/data/img/app/screenload.monochrome.png --out screenload.iconset/icon_16x16.png"
            COMMAND bash "-c" "sips -z 32 32     \"${CMAKE_SOURCE_DIR}\"/data/img/app/screenload.monochrome.png --out screenload.iconset/icon_16x16@2x.png"
            COMMAND bash "-c" "sips -z 32 32     \"${CMAKE_SOURCE_DIR}\"/data/img/app/screenload.monochrome.png --out screenload.iconset/icon_32x32.png"
            COMMAND bash "-c" "sips -z 64 64     \"${CMAKE_SOURCE_DIR}\"/data/img/app/screenload.monochrome.png --out screenload.iconset/icon_32x32@2x.png"
            COMMAND bash "-c" "sips -z 64 64     \"${CMAKE_SOURCE_DIR}\"/data/img/app/screenload.monochrome.png --out screenload.iconset/icon_64x64x.png"
            COMMAND bash "-c" "sips -z 128 128   \"${CMAKE_SOURCE_DIR}\"/data/img/app/screenload.monochrome.png --out screenload.iconset/icon_64x64@2.png"
            COMMAND bash "-c" "sips -z 128 128   \"${CMAKE_SOURCE_DIR}\"/data/img/app/screenload.monochrome-1024.png --out screenload.iconset/icon_128x128.png"
            COMMAND bash "-c" "sips -z 256 256   \"${CMAKE_SOURCE_DIR}\"/data/img/app/screenload.monochrome-1024.png --out screenload.iconset/icon_128x128@2x.png"
            COMMAND bash "-c" "sips -z 256 256   \"${CMAKE_SOURCE_DIR}\"/data/img/app/screenload.monochrome-1024.png --out screenload.iconset/icon_256x256.png"
            COMMAND bash "-c" "sips -z 512 512   \"${CMAKE_SOURCE_DIR}\"/data/img/app/screenload.monochrome-1024.png --out screenload.iconset/icon_256x256@2x.png"
            COMMAND bash "-c" "sips -z 512 512   \"${CMAKE_SOURCE_DIR}\"/data/img/app/screenload.monochrome-1024.png --out screenload.iconset/icon_512x512.png"
            COMMAND bash "-c" "sips -z 1024 1024 \"${CMAKE_SOURCE_DIR}\"/data/img/app/screenload.monochrome-1024.png --out screenload.iconset/icon_512x512@2x.png"


            COMMAND bash "-c" "iconutil -c icns screenload.iconset"
    )

    execute_process(
            COMMAND bash "-c" "rm -R screenload.iconset"
    )

    execute_process(
            # copy icon from cache generated on the localhost if generation on CI failed
            COMMAND bash "-c" "[[ -r 'screenload.icns' ]] || cp \"${CMAKE_SOURCE_DIR}\"/packaging/macos/screenload.icns ./"
    )

    # Set application icon
    set(MACOSX_BUNDLE_ICON_FILE screenload.icns)

    # And this part tells CMake where to find and install the file itself
    set(APP_ICON_MACOSX ${CMAKE_BINARY_DIR}/screenload.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    add_executable(screenload MACOSX_BUNDLE main.cpp ${APP_ICON_MACOSX})
else ()
    add_executable(screenload)
endif ()

add_executable(ScreenLoad::screenload ALIAS screenload)

if(SCREENLOAD_ICON)
  target_compile_definitions(screenload PUBLIC SCREENLOAD_ICON="${SCREENLOAD_ICON}")
endif()

if (WIN32)
    set_property(TARGET screenload PROPERTY WIN32_EXECUTABLE true)
    if (MSVC)
        target_compile_options(screenload PRIVATE /source-charset:utf-8)
    endif ()
endif ()

add_subdirectory(cli)
add_subdirectory(config)
add_subdirectory(core)
add_subdirectory(utils)
add_subdirectory(widgets)
add_subdirectory(tools)

set(SCREENLOAD_TS_FILES
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_bg.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_ca.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_cs.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_de_DE.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_el.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_es.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_eu.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_fa.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_fi.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_fr.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_gl.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_grc.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_he.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_hu.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_id.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_it_IT.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_ja.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_ka.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_ko.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_nb_NO.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_nl_NL.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_nl.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_pl.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_pt_BR.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_ru.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_sk.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_sr_SP.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_sv_SE.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_tr.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_uk.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_vi.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_zh_CN.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_zh_HK.ts
      ${CMAKE_SOURCE_DIR}/data/translations/Internationalization_zh_TW.ts
         )

if (GENERATE_TS)
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${SCREENLOAD_TS_FILES})
else ()
    qt5_add_translation(QM_FILES ${SCREENLOAD_TS_FILES})
endif ()

target_sources(
        screenload
        PRIVATE
        # ${CMAKE_CURRENT_SOURCE_DIR}/../external/Qt-Color-Widgets/src/QtColorWidgets/color_utils.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/../external/Qt-Color-Widgets/src/QtColorWidgets/color_wheel.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/../external/Qt-Color-Widgets/include/QtColorWidgets/color_wheel.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../data/graphics.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/../data/files.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/../data/screenload.rc # windows binary icon resource file
        ${QM_FILES}
        main.cpp)

target_include_directories(
  screenload
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external/Qt-Color-Widgets/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../dbus/>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cli>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/config>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/arrow>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/pixelate>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/circle>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/circlecount>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/copy>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/exit>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/imgur>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/launcher>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/line>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/marker>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/move>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/pencil>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/pin>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/rectangle>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/redo>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/save>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/invert>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/selection>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/sizeindicator>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/text>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/undo>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/sizeincrease>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/sizedecrease>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/utils>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/widgets>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/widgets/panel>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/widgets/capture>
         $<INSTALL_INTERFACE:include/mylib>)
target_include_directories(
    screenload
    PUBLIC
         $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/>
         )


if (USE_EXTERNAL_SINGLEAPPLICATION)
    add_compile_definitions(USE_EXTERNAL_SINGLEAPPLICATION=1)
else ()
    target_include_directories(
        screenload
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external/singleapplication>)
endif()

target_link_libraries(
        screenload
        project_warnings
        project_options
        Qt5::Svg
        Qt5::DBus
        Qt5::Network
        Qt5::Widgets
        ${QTSINGLEAPPLICATION_LIBRARY}
        QtColorWidgets

)

if (USE_WAYLAND_CLIPBOARD)
  target_compile_definitions(screenload PRIVATE USE_WAYLAND_CLIPBOARD=1)
  target_link_libraries(screenload KF5::GuiAddons)
endif()

if (APPLE)
    set(MACOSX_BUNDLE_IDENTIFIER "ru.screenload")
    set_target_properties(
            screenload
            PROPERTIES
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${MACOSX_BUNDLE_IDENTIFIER}
    )
    target_link_libraries(
            screenload
            qhotkey
    )
endif ()

if (WIN32)
    set(USE_OPENSSL FALSE)
    if (ENABLE_OPENSSL)
        find_package(OpenSSL)
        if (OPENSSL_FOUND)
            message(STATUS "OpenSSL support enabled.")
            set(USE_OPENSSL TRUE)
        endif ()
    else ()
        mark_as_advanced(CLEAR OPENSSL_LIBRARIES OPENSSL_INCLUDE_DIR)
    endif ()

    if (NOT USE_OPENSSL)
        message(WARNING "OpenSSL is required to upload screenshots")
    endif ()
endif ()

# Choose default color palette (small or large)
if($ENV{SCREENLOAD_PREDEFINED_COLOR_PALETTE_LARGE})
    set(SCREENLOAD_PREDEFINED_COLOR_PALETTE_LARGE true)
else()
    set(SCREENLOAD_PREDEFINED_COLOR_PALETTE_LARGE false)
endif()
message("ScreenLoad predefined color palette large: " ${SCREENLOAD_PREDEFINED_COLOR_PALETTE_LARGE})
target_compile_definitions(screenload PRIVATE PREDEFINED_COLOR_PALETTE_LARGE=${SCREENLOAD_PREDEFINED_COLOR_PALETTE_LARGE})

find_package (Git)
if (GIT_FOUND)
    message("git found: ${GIT_EXECUTABLE} in version     ${GIT_VERSION_STRING}")
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD OUTPUT_VARIABLE SCREENLOAD_GIT_HASH)
    string(REGEX REPLACE "\r*\n$" "" SCREENLOAD_GIT_HASH "${SCREENLOAD_GIT_HASH}")
    target_compile_definitions(screenload PRIVATE SCREENLOAD_GIT_HASH="${SCREENLOAD_GIT_HASH}")
    message("SCREENLOAD_GIT_HASH: ${SCREENLOAD_GIT_HASH}")
else()
    target_compile_definitions(screenload PRIVATE SCREENLOAD_GIT_HASH="-")
    message("git command is not found")
endif ()
target_compile_definitions(screenload PRIVATE APP_PREFIX="${CMAKE_INSTALL_PREFIX}")
target_compile_definitions(screenload PRIVATE APP_VERSION="v${PROJECT_VERSION}")
#target_compile_definitions(screenload PRIVATE QAPPLICATION_CLASS=QApplication)
target_compile_definitions(screenload PRIVATE SCREENLOAD_APP_VERSION_URL="https://solution.s3.amazonaws.com/screenload-qt/Version.json")
# Enable easier debugging of screenshot capture mode
if (SCREENLOAD_DEBUG_CAPTURE)
    target_compile_definitions(screenload PRIVATE SCREENLOAD_DEBUG_CAPTURE)
endif ()

if (USE_MONOCHROME_ICON)
    target_compile_definitions(screenload PRIVATE USE_MONOCHROME_ICON)
endif ()

foreach (FILE ${QM_FILES})
    get_filename_component(F_NAME ${FILE} NAME)
    add_custom_command(
            TARGET screenload
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${F_NAME}
            ${CMAKE_CURRENT_BINARY_DIR}/translations/${F_NAME})
endforeach ()

# ######################################################################################################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/ScreenLoad)

if(USE_LAUNCHER_ABSOLUTE_PATH)
    set(LAUNCHER_EXECUTABLE "${CMAKE_INSTALL_FULL_BINDIR}/screenload")
else()
    set(LAUNCHER_EXECUTABLE "screenload")
endif()

# Install binary
install(TARGETS screenload
        EXPORT screenload-targets
        BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if (UNIX)
    # Install desktop files, completion and dbus files
    configure_file(${CMAKE_SOURCE_DIR}/data/desktopEntry/package/ru.screenload.ScreenLoad.desktop
            ${CMAKE_CURRENT_BINARY_DIR}/share/applications/ru.screenload.ScreenLoad.desktop @ONLY)

    configure_file(${CMAKE_SOURCE_DIR}/data/appdata/ru.screenload.ScreenLoad.metainfo.xml
            ${CMAKE_CURRENT_BINARY_DIR}/share/metainfo/ru.screenload.ScreenLoad.metainfo.xml COPYONLY)

    configure_file(${CMAKE_SOURCE_DIR}/data/shell-completion/screenload.bash
            ${CMAKE_CURRENT_BINARY_DIR}/share/bash-completion/completions/screenload COPYONLY)

    configure_file(${CMAKE_SOURCE_DIR}/data/shell-completion/screenload.zsh
            ${CMAKE_CURRENT_BINARY_DIR}/share/zsh/site-functions/_screenload COPYONLY)

    configure_file(${CMAKE_SOURCE_DIR}/data/shell-completion/screenload.fish
            ${CMAKE_CURRENT_BINARY_DIR}/share/fish/vendor_completions.d/screenload.fish COPYONLY)

    configure_file(${CMAKE_SOURCE_DIR}/data/dbus/ru.screenload.ScreenLoad.xml
            ${CMAKE_CURRENT_BINARY_DIR}/share/dbus-1/interfaces/ru.screenload.ScreenLoad.xml COPYONLY)

    configure_file(${CMAKE_SOURCE_DIR}/data/dbus/ru.screenload.ScreenLoad.service.in
            ${CMAKE_CURRENT_BINARY_DIR}/share/dbus-1/services/ru.screenload.ScreenLoad.service)

    # Install man pages
    configure_file(${CMAKE_SOURCE_DIR}/data/man/man1/screenload.1
            ${CMAKE_CURRENT_BINARY_DIR}/share/man/man1/screenload.1 COPYONLY)

    # Install Icons
    configure_file(${CMAKE_SOURCE_DIR}/data/img/hicolor/48x48/apps/ru.screenload.ScreenLoad.png
            ${CMAKE_CURRENT_BINARY_DIR}/share/icons/hicolor/48x48/apps/ru.screenload.ScreenLoad.png COPYONLY)

    configure_file(${CMAKE_SOURCE_DIR}/data/img/hicolor/128x128/apps/ru.screenload.ScreenLoad.png
            ${CMAKE_CURRENT_BINARY_DIR}/share/icons/hicolor/128x128/apps/ru.screenload.ScreenLoad.png COPYONLY)

    configure_file(${CMAKE_SOURCE_DIR}/data/img/hicolor/scalable/apps/ru.screenload.ScreenLoad.svg
            ${CMAKE_CURRENT_BINARY_DIR}/share/icons/hicolor/scalable/apps/ru.screenload.ScreenLoad.svg COPYONLY)

    # Install icon with both names
    configure_file(${CMAKE_SOURCE_DIR}/data/img/hicolor/48x48/apps/screenload.png
            ${CMAKE_CURRENT_BINARY_DIR}/share/icons/hicolor/48x48/apps/screenload.png COPYONLY)

    configure_file(${CMAKE_SOURCE_DIR}/data/img/hicolor/128x128/apps/screenload.png
            ${CMAKE_CURRENT_BINARY_DIR}/share/icons/hicolor/128x128/apps/screenload.png COPYONLY)

    configure_file(${CMAKE_SOURCE_DIR}/data/img/hicolor/scalable/apps/screenload.svg
            ${CMAKE_CURRENT_BINARY_DIR}/share/icons/hicolor/scalable/apps/screenload.svg COPYONLY)

    # Install assets
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/share/ DESTINATION ${CMAKE_INSTALL_DATAROOTDIR})

    # Install Translations
    install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/screenload/translations)
endif ()

# windeployqt
if (WIN32)
    if (EXISTS $ENV{QTDIR}/bin/windeployqt.exe)
        if (CMAKE_BUILD_TYPE MATCHES Release)
            set(BINARIES_TYPE --release)
        else ()
            set(BINARIES_TYPE --debug)
        endif ()

        add_custom_command(
                TARGET screenload
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/windeployqt_stuff
                COMMAND
                $ENV{QTDIR}/bin/windeployqt.exe ${BINARIES_TYPE} --no-translations --compiler-runtime --no-system-d3d-compiler
                --no-angle --no-webkit2 --no-quick-import --dir ${CMAKE_BINARY_DIR}/windeployqt_stuff $<TARGET_FILE:screenload>
                # copy translations manually QM_FILES
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/windeployqt_stuff/translations
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/src/translations
                ${CMAKE_BINARY_DIR}/windeployqt_stuff/translations)

        install(DIRECTORY ${CMAKE_BINARY_DIR}/windeployqt_stuff/ DESTINATION bin)


        STRING(REGEX REPLACE "\\\\" "/" OPENSSL_ROOT_DIR "$ENV{OPENSSL_ROOT_DIR}" )

        if (ENABLE_OPENSSL)
            if (EXISTS ${OPENSSL_ROOT_DIR}/bin)

                install(
                        DIRECTORY ${OPENSSL_ROOT_DIR}/bin/
                        DESTINATION bin
                        FILES_MATCHING
                        PATTERN "*.dll")
            else ()
                message(WARNING "Unable to find OpenSSL dlls.")
            endif ()
        endif ()

    else ()
        message("Unable to find executable QTDIR/bin/windeployqt.")
    endif ()
endif ()

# macdeployqt
if (APPLE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
    execute_process(COMMAND brew --prefix qt5 OUTPUT_VARIABLE QTDIR)
    string(REGEX REPLACE "\n$" "" QTDIR "${QTDIR}")
    set(MAC_DEPLOY_QT ${QTDIR}/bin/macdeployqt)
    if (EXISTS ${MAC_DEPLOY_QT})
        set_source_files_properties(resources/icon.icns PROPERTIES
                MACOSX_PACKAGE_LOCATION Resources)

        set_target_properties(${target} PROPERTIES
                MACOSX_BUNDLE TRUE
                )
    else ()
        message("Unable to find executable ${MAC_DEPLOY_QT}.")
    endif ()
endif ()
