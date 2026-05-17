// Not sure what liscense yet
// Sourojeet Adhikari
// FreeBSD Foundation


#ifndef _THUNK_UDMABUF_H
#define _THUNK_UDMABUF_H

#include <sys/types.h>
#include <sys/ioctl.h>

#define UDMABUF_FLAGS_CLOEXEC 0x01

struct udmabuf_create {
        __uint32_t memfd;
        __uint32_t flags;
        __uint64_t offset;
        __uint64_t size;
};

struct udmabuf_create_item {
        __uint32_t memfd;
        __uint32_t __pad;
        __uint64_t offset;
        __uint64_t size;
};

struct udmabuf_create_list {
        __uint32_t flags;
        __uint32_t count;
        struct udmabuf_create_item list[];
};

#define UDMABUF_CREATE       _IOW('u', 0x42, struct udmabuf_create)
#define UDMABUF_CREATE_LIST  _IOW('u', 0x43, struct udmabuf_create_list)

#endif // _THUNK_UDMABUF_H
