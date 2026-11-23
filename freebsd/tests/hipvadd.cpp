// Minimal HIP test: device query, H2D/D2H copies, kernel launch, verify.
#include <hip/hip_runtime.h>
#include <cstdio>
#include <vector>

#define CHECK(x) do { hipError_t e_ = (x); if (e_ != hipSuccess) { \
	fprintf(stderr, "%s:%d %s: %s\n", __FILE__, __LINE__, #x, hipGetErrorString(e_)); \
	return 1; } } while (0)

__global__ void vadd(const int *a, const int *b, int *c, int n)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < n)
		c[i] = a[i] + b[i];
}

int main()
{
	const int n = 1 << 20;
	int ndev = 0;
	CHECK(hipGetDeviceCount(&ndev));
	hipDeviceProp_t p;
	CHECK(hipGetDeviceProperties(&p, 0));
	printf("devices=%d name=%s arch=%s\n", ndev, p.name, p.gcnArchName);

	std::vector<int> a(n), b(n), c(n, -1);
	for (int i = 0; i < n; i++) { a[i] = i; b[i] = 2 * i; }
	int *da, *db, *dc;
	CHECK(hipMalloc(&da, n * sizeof(int)));
	CHECK(hipMalloc(&db, n * sizeof(int)));
	CHECK(hipMalloc(&dc, n * sizeof(int)));
	CHECK(hipMemcpy(da, a.data(), n * sizeof(int), hipMemcpyHostToDevice));
	CHECK(hipMemcpy(db, b.data(), n * sizeof(int), hipMemcpyHostToDevice));
	vadd<<<(n + 255) / 256, 256>>>(da, db, dc, n);
	CHECK(hipGetLastError());
	CHECK(hipDeviceSynchronize());
	CHECK(hipMemcpy(c.data(), dc, n * sizeof(int), hipMemcpyDeviceToHost));
	int bad = 0;
	for (int i = 0; i < n; i++)
		if (c[i] != 3 * i && bad++ < 5)
			fprintf(stderr, "c[%d]=%d want %d\n", i, c[i], 3 * i);
	printf("%s: %d/%d correct\n", bad ? "FAIL" : "PASS", n - bad, n);
	CHECK(hipFree(da)); CHECK(hipFree(db)); CHECK(hipFree(dc));
	return bad ? 1 : 0;
}
