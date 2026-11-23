#pragma once
#include <hip/hip_runtime.h>
#include <cstdio>
#include <cstdlib>
#define CHECK(x) do { hipError_t e_ = (x); if (e_ != hipSuccess) { \
  fprintf(stderr, "%s:%d %s failed: %s\n", __FILE__, __LINE__, #x, hipGetErrorString(e_)); exit(1); } } while (0)
