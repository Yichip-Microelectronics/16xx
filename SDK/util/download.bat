@echo off

e p >nul 2>&1
e 9f08 11
e 8050 1
e p >nul 2>&1
e 9f08 11
e 8050 1
e p 
e 9f08 11
e 8050 1

::flash io config
e 830c 02
e 8310 03
e 830f 04
e 830d 05
e 830b 06
e 830e 07


e fa
e fs 0
e fe 0 2000 
e fr 1000 10
e k >nul 2>&1

e p >nul 2>&1
e 9f08 11
e 8050 1
e p >nul 2>&1
e 9f08 11
e 8050 1
e p
e 9f08 11
e 8050 1

e h fast_download.rom c000 >nul 2>&1
e r dpc 1000080
e r mstatus 00
e c >nul 2>&1
set baud=30
e fp uart_polling.dat 1000 f
e fw 0 001003 >nul 2>&1
e fr 1000 10 
e k