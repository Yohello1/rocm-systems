// hipHostMalloc (pinned, host-visible) + hipMallocManaged, kernel reads/writes directly.
#include "common.h"
__global__ void scale(float* a, int n, float f) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) a[i] = a[i] * f + 1.0f;
}
static int verify(const char* what, float* a, int n) {
  int ok = 0;
  for (int i = 0; i < n; i++) ok += (a[i] == i * 2.0f + 1.0f);
  printf("%s: %d/%d correct\n", what, ok, n);
  return ok == n;
}
int main() {
  const int n = 1 << 20;
  int pass = 1;
  float* h;
  CHECK(hipHostMalloc(&h, n * sizeof(float), hipHostMallocDefault));
  for (int i = 0; i < n; i++) h[i] = i;
  scale<<<(n + 255) / 256, 256>>>(h, n, 2.0f);
  CHECK(hipGetLastError());
  CHECK(hipDeviceSynchronize());
  pass &= verify("hipHostMalloc", h, n);
  CHECK(hipHostFree(h));

  float* m;
  CHECK(hipMallocManaged(&m, n * sizeof(float)));
  for (int i = 0; i < n; i++) m[i] = i;
  scale<<<(n + 255) / 256, 256>>>(m, n, 2.0f);
  CHECK(hipGetLastError());
  CHECK(hipDeviceSynchronize());
  pass &= verify("hipMallocManaged", m, n);
  CHECK(hipFree(m));

  // plain malloc'ed memory registered with hipHostRegister
  float* r = (float*)aligned_alloc(4096, n * sizeof(float));
  for (int i = 0; i < n; i++) r[i] = i;
  CHECK(hipHostRegister(r, n * sizeof(float), hipHostRegisterDefault));
  float* dr;
  CHECK(hipHostGetDevicePointer((void**)&dr, r, 0));
  scale<<<(n + 255) / 256, 256>>>(dr, n, 2.0f);
  CHECK(hipGetLastError());
  CHECK(hipDeviceSynchronize());
  pass &= verify("hipHostRegister", r, n);
  CHECK(hipHostUnregister(r));
  free(r);

  printf("%s\n", pass ? "PASS" : "FAIL");
  return pass ? 0 : 1;
}
