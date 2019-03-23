sc stop IOCTLFirstDRIVER
sc delete IOCTLFirstDRIVER 

ping ::1 -n 10

sc stop IOCTLSecondDRIVER
sc delete IOCTLSecondDRIVER 

tracelog.exe -stop ConsoleApp
tracelog.exe -stop IoctlFirstDriver
tracelog.exe -stop IoctlSecondDriver