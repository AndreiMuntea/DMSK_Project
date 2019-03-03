@echo off

ECHO "Usage : %0 ^<TMF directory^> [etl location] [result path]"

if NOT [%1]==[] goto collect_logs
    @echo Usage: %0 
goto :eof

:collect_logs

SET etl_path=".\\log.etl"
if NOT [%2]==[] (
    SET etl_path=%2
)

SET result_path=".\\log"
if NOT [%3]==[] (
    SET etl_path=%3
)

ECHO "Searching etl file in %etl_path%"
ECHO "Searching tmf file in %1"
ECHO "Writing results in %result_path%"

tracelog.exe -stop ConsoleApplicationTrace 
tracefmt.exe %etl_path% -p %1 -o %result_path%