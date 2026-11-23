// Block reduction using __shared__ memory + __syncthreads, and a shared-mem transpose.
#include "common.h"
#define BS 256
__global__ void reduce(const int* in, unsigned long long* out, int n) {
  __shared__ int buf[BS];
  int t = threadIdx.x, i = blockIdx.x * BS + t;
  buf[t] = (i < n) ? in[i] : 0;
  __syncthreads();
  for (int s = BS / 2; s > 0; s >>= 1) {
    if (t < s) buf[t] += buf[t + s];
    __syncthreads();
  }
  if (t == 0) atomicAdd(out, (unsigned long long)buf[0]);
}
#define TD 16
__global__ void transpose(const float* in, float* out, int w, int h) {
  __shared__ float tile[TD][TD + 1];
  int x = blockIdx.x * TD + threadIdx.x, y = blockIdx.y * TD + threadIdx.y;
  if (x < w && y < h) tile[threadIdx.y][threadIdx.x] = in[y * w + x];
  __syncthreads();
  x = blockIdx.y * TD + threadIdx.x; y = blockIdx.x * TD + threadIdx.y;
  if (x < h && y < w) out[y * h + x] = tile[threadIdx.x][threadIdx.y];
}
int main() {
  const int n = 1 << 24;
  int* h = (int*)malloc(n * sizeof(int));
  unsigned long long expect = 0;
  for (int i = 0; i < n; i++) { h[i] = i % 977; expect += h[i]; }
  int* d; unsigned long long* dsum; unsigned long long sum = 0;
  CHECK(hipMalloc(&d, n * sizeof(int)));
  CHECK(hipMalloc(&dsum, sizeof(sum)));
  CHECK(hipMemcpy(d, h, n * sizeof(int), hipMemcpyHostToDevice));
  CHECK(hipMemset(dsum, 0, sizeof(sum)));
  reduce<<<(n + BS - 1) / BS, BS>>>(d, dsum, n);
  CHECK(hipGetLastError());
  CHECK(hipMemcpy(&sum, dsum, sizeof(sum), hipMemcpyDeviceToHost));
  bool pass = sum == expect;
  printf("reduce: got %llu expected %llu\n", sum, expect);

  const int w = 1000, ht = 777;
  float *hi = (float*)malloc(w * ht * sizeof(float)), *ho = (float*)malloc(w * ht * sizeof(float));
  for (int i = 0; i < w * ht; i++) hi[i] = i;
  float *di, *dout;
  CHECK(hipMalloc(&di, w * ht * sizeof(float)));
  CHECK(hipMalloc(&dout, w * ht * sizeof(float)));
  CHECK(hipMemcpy(di, hi, w * ht * sizeof(float), hipMemcpyHostToDevice));
  transpose<<<dim3((w + TD - 1) / TD, (ht + TD - 1) / TD), dim3(TD, TD)>>>(di, dout, w, ht);
  CHECK(hipGetLastError());
  CHECK(hipMemcpy(ho, dout, w * ht * sizeof(float), hipMemcpyDeviceToHost));
  int ok = 0;
  for (int y = 0; y < ht; y++) for (int x = 0; x < w; x++) ok += (ho[x * ht + y] == hi[y * w + x]);
  printf("transpose: %d/%d correct\n", ok, w * ht);
  pass &= ok == w * ht;
  CHECK(hipFree(d)); CHECK(hipFree(dsum)); CHECK(hipFree(di)); CHECK(hipFree(dout));
  free(h); free(hi); free(ho);
  printf("%s\n", pass ? "PASS" : "FAIL");
  return pass ? 0 : 1;
}
