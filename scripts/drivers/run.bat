tracelog.exe -start IoctlFirstDriver -guid #86FB450A-DB03-44C6-863E-C4FB9E6741A8 -level 7 -flag 0xffff -rt -kd

sc create IOCTLFirstDRIVER binpath="c:\work\IoctlFirstDriver\IOCTLFirstDriver.sys" type=kernel
sc start IOCTLFirstDRIVER 