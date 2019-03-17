#ifndef __IOCTL_HPP__ 
#define __IOCTL_HPP__ 

#include <sal.h>
#include <winioctl.h>

#define IOCTL_DEVICE_TYPE   56789

#define FIRST_IOCTL_CODE    CTL_CODE(IOCTL_DEVICE_TYPE, 0x850, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SECOND_IOCTL_CODE   CTL_CODE(IOCTL_DEVICE_TYPE, 0x851, METHOD_BUFFERED, FILE_ANY_ACCESS)


void
IoctlSendIoctl(
    _In_ DWORD IoctlCode
);

#endif //__IOCTL_HPP__