// Multiple hipStreams with async copies + kernels, hipEvent timing.
#include "common.h"
__global__ void saxpy(const float* x, float* y, int n, float a) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) y[i] = a * x[i] + y[i];
}
int main() {
  const int nstreams = 4, n = 1 << 25, chunk = n / nstreams;
  float *hx, *hy, *dx, *dy;
  CHECK(hipHostMalloc(&hx, n * sizeof(float)));
  CHECK(hipHostMalloc(&hy, n * sizeof(float)));
  CHECK(hipMalloc(&dx, n * sizeof(float)));
  CHECK(hipMalloc(&dy, n * sizeof(float)));
  for (int i = 0; i < n; i++) { hx[i] = i % 1000; hy[i] = 1.0f; }
  hipStream_t s[nstreams];
  for (int k = 0; k < nstreams; k++) CHECK(hipStreamCreate(&s[k]));
  hipEvent_t start, stop;
  CHECK(hipEventCreate(&start));
  CHECK(hipEventCreate(&stop));
  CHECK(hipEventRecord(start, 0));
  for (int k = 0; k < nstreams; k++) {
    size_t off = (size_t)k * chunk;
    CHECK(hipMemcpyAsync(dx + off, hx + off, chunk * sizeof(float), hipMemcpyHostToDevice, s[k]));
    CHECK(hipMemcpyAsync(dy + off, hy + off, chunk * sizeof(float), hipMemcpyHostToDevice, s[k]));
    hipLaunchKernelGGL(saxpy, dim3((chunk + 255) / 256), dim3(256), 0, s[k], dx + off, dy + off, chunk, 3.0f);
    CHECK(hipGetLastError());
    CHECK(hipMemcpyAsync(hy + off, dy + off, chunk * sizeof(float), hipMemcpyDeviceToHost, s[k]));
  }
  for (int k = 0; k < nstreams; k++) CHECK(hipStreamSynchronize(s[k]));
  CHECK(hipEventRecord(stop, 0));
  CHECK(hipEventSynchronize(stop));
  float ms = 0;
  CHECK(hipEventElapsedTime(&ms, start, stop));
  int ok = 0;
  for (int i = 0; i < n; i++) ok += (hy[i] == 3.0f * (i % 1000) + 1.0f);
  // kernel-only timing on one stream
  hipEvent_t k0, k1;
  CHECK(hipEventCreate(&k0));
  CHECK(hipEventCreate(&k1));
  CHECK(hipEventRecord(k0, s[0]));
  for (int it = 0; it < 10; it++)
    hipLaunchKernelGGL(saxpy, dim3((n + 255) / 256), dim3(256), 0, s[0], dx, dy, n, 1.0f);
  CHECK(hipEventRecord(k1, s[0]));
  CHECK(hipEventSynchronize(k1));
  float kms = 0;
  CHECK(hipEventElapsedTime(&kms, k0, k1));
  printf("streams=%d n=%d: %d/%d correct, total %.3f ms; 10x saxpy %.3f ms (%.1f GB/s)\n",
         nstreams, n, ok, n, ms, kms, 10.0 * 3 * n * sizeof(float) / (kms * 1e6));
  // verify the timed kernels really ran: y = (3x+1) + 10x
  CHECK(hipMemcpy(hy, dy, n * sizeof(float), hipMemcpyDeviceToHost));
  int ok2 = 0;
  for (int i = 0; i < n; i++) ok2 += (hy[i] == 13.0f * (i % 1000) + 1.0f);
  printf("after timed kernels: %d/%d correct\n", ok2, n);
  bool pass = ok == n && ok2 == n && ms > 0 && kms > 0;
  for (int k = 0; k < nstreams; k++) CHECK(hipStreamDestroy(s[k]));
  CHECK(hipEventDestroy(start)); CHECK(hipEventDestroy(stop));
  CHECK(hipEventDestroy(k0)); CHECK(hipEventDestroy(k1));
  CHECK(hipFree(dx)); CHECK(hipFree(dy));
  CHECK(hipHostFree(hx)); CHECK(hipHostFree(hy));
  printf("%s\n", pass ? "PASS" : "FAIL");
  return pass ? 0 : 1;
}
