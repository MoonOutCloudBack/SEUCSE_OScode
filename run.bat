start /D "E:\OpenOCD" cmd /K openocd-nexys4.bat
start "gdb" cmd /K "E:\MipsMingw\bin\mips-mti-elf-gdb.exe" -x startup.txt 