@echo off
set files=example.c lib/orui.c lib/platform.c lib/allocators.c lib/console.c lib/misc.c
set flags=-fsanitize=address -O0 -gfull -g3 -Wall -Wno-switch -Wno-microsoft-enum-forward-reference -Wno-unused-variable -Wno-unused-function -DUI_BACKEND_WIN32_SOFTWARE= 
set libs=-l shell32.lib -l gdi32.lib -l user32.lib 
clang %files% -o out/example.exe %flags% %libs%
@echo on
