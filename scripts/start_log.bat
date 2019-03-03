@echo off

ECHO "Usage : %0 [etl location]"

SET etl_path=".\\log.etl"
if NOT [%1]==[] (
    SET etl_path=%1
)

ECHO "Writing etl file in %etl_path%"

tracelog.exe -start ConsoleApplicationTrace -f %etl_path%
tracelog.exe -enable ConsoleApplicationTrace -guid #84bdb2e9-829e-41b3-b891-02f454bc2bd7 -flags 0x00000001 -level 7 