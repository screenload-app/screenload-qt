#compdef screenload
# ------------------------------------------------------------------------------
# Description
# -----------
#
#  Completion script for the screenload command line interface
#  (https://github.com/screenload-org/screenload).
#
# ------------------------------------------------------------------------------
# How to use
# -------
#
# Copy this file to /usr/share/zsh/site-functions/_screenload
#


# gui

_screenload_gui_opts=(
    {-p,--path}'[Existing directory or new file to save to]':dir:_files
    {-c,--clipboard}'[Save the capture to the clipboard]'
    {-d,--delay}'[Delay time in milliseconds]'
    "--region[Screenshot region to select <WxH+X+Y or string>]"
    {-r,--raw}'[Print raw PNG capture]'
    {-g,--print-geometry}'[Print geometry of the selection in the format W H X Y. Does nothing if raw is specified]'
    {-u,--upload}'[Upload screenshot]'
    "--pin[Pin the capture to the screen]"
    {-s,--accept-on-select}'[Accept capture as soon as a selection is made]'
)

_screenload_gui() {
    _arguments -s : \
    "$_screenload_gui_opts[@]"
}


# screen

_screenload_screen_opts=(
    {-n,--number}'[Define the screen to capture (starting from 0). Default: screen containing the cursor]'
    {-p,--path}'[Existing directory or new file to save to]':dir:_files
    {-c,--clipboard}'[Save the capture to the clipboard]'
    {-d,--delay}'[Delay time in milliseconds]'
    "--region[Screenshot region to select <WxH+X+Y or string>]"
    {-r,--raw}'[Print raw PNG capture]'
    {-u,--upload}'[Upload screenshot]'
    "--pin[Pin the capture to the screen]"
)

_screenload_screen() {
    _arguments -s : \
    "$_screenload_screen_opts[@]"
}


# full

_screenload_full_opts=(
    {-p,--path}'[Existing directory or new file to save to]':dir:_files
    {-c,--clipboard}'[Save the capture to the clipboard]'
    {-d,--delay}'[Delay time in milliseconds]'
    "--region[Screenshot region to select <WxH+X+Y or string>]"
    {-r,--raw}'[Print raw PNG capture]'
    {-u,--upload}'[Upload screenshot]'
)

_screenload_full() {
    _arguments -s : \
    "$_screenload_full_opts[@]"
}


# config

_screenload_config_opts=(
    {-a,--autostart}'[Enable or disable run at startup]'
    {-f,--filename}'[Set the filename pattern]'
    {-t,--trayicon}'[Enable or disable the tray icon]'
    {-s,--showhelp}'[Define the main UI color]'
    {-m,--maincolor}'[Show the help message in the capture mode]'
    {-k,--contrastcolor}'[Define the contrast UI color]'
    "--check[Check the configuration for errors]"
)

_screenload_config() {
    _arguments -s : \
    "$_screenload_config_opts[@]"
}


# Main handle
_screenload() {
    local curcontext="$curcontext" ret=1
    local -a state line commands

    commands=(
        "gui:Start a manual capture in GUI mode"
        "screen:Capture a single screen (one monitor)"
        "full:Capture the entire desktop (all monitors)"
        "launcher:Open the capture launcher"
        "config:Configure ScreenLoad"
    )

    _arguments -C -s -S -n \
        '(- 1 *)'{-v,--version}"[display version information]: :->full" \
        '(- 1 *)'{-h,--help}'[[display usage information]: :->full' \
        '1:cmd:->cmds' \
        '*:: :->args' && ret=0

    case "$state" in
        (cmds)
            _describe -t commands 'commands' commands
        ;;
        (args)
        local cmd
        cmd=$words[1]
        case "$cmd" in
            (gui)
                _screenload_gui && ret=0
            ;;
            (screen)
                _screenload_screen && ret=0
            ;;
            (full)
                _screenload_full && ret=0
            ;;
            (config)
                _screenload_config && ret=0
            ;;
            (*)
                _default && ret=0
            ;;
        esac
        ;;
        (*)
        ;;
    esac

    return ret
}

_screenload

#
# Editor modelines  -  https://www.wireshark.org/tools/modelines.html
#
# Local variables:
# mode: sh
# c-basic-offset: 4
# tab-width: 4
# indent-tabs-mode: nil
# End:
#
# vi: set filetype=zsh shiftwidth=4 tabstop=4 expandtab:
# :indentSize=4:tabSize=4:noTabs=true:
#
