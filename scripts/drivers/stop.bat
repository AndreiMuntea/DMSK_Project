sc stop IOCTLFirstDRIVER
sc delete IOCTLFirstDRIVER 

sc stop IOCTLSecondDRIVER
sc delete IOCTLSecondDRIVER 

tracelog.exe -stop ConsoleApp
tracelog.exe -stop IoctlFirstDriver
tracelog.exe -stop IoctlSecondDriver