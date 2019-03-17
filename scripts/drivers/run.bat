tracelog.exe -start IoctlFirstDriver -guid #86FB450A-DB03-44C6-863E-C4FB9E6741A8 -level 7 -flag 0xffff -rt -kd
tracelog.exe -start IoctlSecondDriver -guid #9567F86C-BCDB-4F31-9119-1AE1089366D4 -level 7 -flag 0xffff -rt -kd
tracelog.exe -start ConsoleApp -guid #84bdb2e9-829e-41b3-b891-02f454bc2bd7 -level 7 -flag 0xffff -rt -kd

sc create IOCTLSecondDRIVER binpath="c:\work\IoctlFirstDriver\IOCTLSecondDriver.sys" type=kernel
sc start IOCTLSecondDRIVER 

ping ::1 -n 2

sc create IOCTLFirstDRIVER binpath="c:\work\IoctlFirstDriver\IOCTLFirstDriver.sys" type=kernel
sc start IOCTLFirstDRIVER 
