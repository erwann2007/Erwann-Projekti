@echo off

set RH="C:\Users\Asentaja\Documents\secret-directory\raylib\include"
set RS="C:\Users\Asentaja\Documents\secret-directory\raylib\lib"

gcc src-tetris.c -o tetris.exe -I%RH% -L%RS% -lraylib -lgdi32 -lwinmm -mwindows
