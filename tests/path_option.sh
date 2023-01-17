#!/usr/bin/env sh

# Before running the script make sure a screenload daemon with a matching version
# is running

# The first argument to this script is a path to the screenload executable
[ -n "$1" ] && screenload="$1" || screenload='screenload'

# TODO Before proper stderr logging is implemented, you will have to look at the
# system notifications

rm -rf /tmp/screenload_path_test 2>/dev/null
mkdir -p /tmp/screenload_path_test
cd /tmp/screenload_path_test

echo ">> Nonexistent directory. This command should give an invalid path error."
"$screenload" screen -p blah/blah

sleep 2
echo ">> The output file is specified relative to PWD"
"$screenload" screen -p relative.png

sleep 2
echo ">> Absolute paths work too"
"$screenload" screen -p /tmp/screenload_path_test/absolute.png

sleep 2
mkdir subdir
echo ">> Redundancy in the path will be removed"
"$screenload" screen -p /tmp/screenload_path_test/subdir/..///redundancy_removed.png

sleep 2
echo ">> If the destination is a directory, the file name is generated from strf from the config"
"$screenload" screen -p ./

sleep 2
echo ">> If the output file has no suffix, it will be added (png)"
"$screenload" screen -p /tmp/screenload_path_test/without_suffix

sleep 2
echo ">> Other suffixes are supported, and the image format will match it"
"$screenload" screen -p /tmp/screenload_path_test/jpg_suffix.jpg

sleep 2
echo ">> If the destination path exists, it will have _NUM appended to the base name"
"$screenload" screen -p /tmp/screenload_path_test/absolute.png

sleep 2
echo ">> Same thing again but without specifying a suffix"
"$screenload" screen -p /tmp/screenload_path_test/absolute

sleep 2
