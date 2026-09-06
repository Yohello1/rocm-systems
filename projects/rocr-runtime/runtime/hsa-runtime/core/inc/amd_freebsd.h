#ifndef ElfW
  #if defined(__LP64__) || defined(_LP64)
    #define ElfW(type) Elf64_##type
  #else
    #define ElfW(type) Elf32_##type
  #endif
#endif

#ifndef PT_LOAD
  #define PT_LOAD 1
#endif

#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif
#ifndef _BSD_SOURCE
  #define _BSD_SOURCE 1
#endif

#include <sys/types.h>
#include <link.h>
#include <elf.h>

#ifndef ElfW
  #if defined(__LP64__) || defined(_LP64)
    #define ElfW(type) Elf64_##type
  #else
    #define ElfW(type) Elf32_##type
  #endif
#endif

#ifndef Elf64_Addr
  typedef uint64_t Elf64_Addr;
#endif
#ifndef Elf32_Addr
  typedef uint32_t Elf32_Addr;
#endif

#ifndef PT_LOAD
  #define PT_LOAD 1
#endif

#include <stdatomic.h>
#include <atomic>
#include <elf.h>
#include <sys/types.h>
#include <link.h>
