@echo off
set /p f="Enter file: "
cd \
cd C:\Users\Anjalee\Documents\GitHub\5225A-2018-2019\src
gcc -E %f%.c -o ..\debugging\%f%_out.c