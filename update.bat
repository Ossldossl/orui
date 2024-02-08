@echo off
pushd dreid
git pull
call build_lib.bat
copy dreid.lib ../dreid.lib
popd