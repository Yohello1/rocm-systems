#include <sys/ioctl.h>
#include <stdio.h>

int main() {
#ifdef IOCPARM_MASK
    printf("IOCPARM_MASK: %d\n", IOCPARM_MASK);
#endif
#ifdef _IOC_SIZEMASK
    printf("_IOC_SIZEMASK: %d\n", _IOC_SIZEMASK);
#endif
    return 0;
}
