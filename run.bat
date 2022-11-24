start /D "E:\OpenOCD" cmd /K openocd-nexys4.bat
start "gdb" cmd /K "E:\MIPS\Toolchains\mips-mti-elf\2017.10-05\bin\mips-mti-elf-gdb.exe" -x startup.txt 