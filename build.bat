set LD_LIBRARY_PATH=./raylib/lib/
REM	error = execute(f'{compiler} /nologo /MP /std:c++17 /wd"4530" {arg_defines} /Zi {dll_stuff} {INCLUDES} /Fe:"{APP_NAME}" {arg_c_files} R:/rose/.build/bin/Release/raylib.lib R:/rose/.build/bin/Release/imgui.lib /link /incremental /PDB:"{PDB_NAME}" > {TMP}/clout.txt')
REM g++ ./main.cpp -std=c++17 -o ./raycraft.out -L./raylib/lib/ -lraylib
REM /MP
CL /nologo /MD /std:c++latest /Fe:raycraft.exe ./main.cpp /link .\raylib\lib\raylib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib Winmm.lib
raycraft.exe