# HIP on FreeBSD (Strix Halo, gfx1151) — build notes

Tested on FreeBSD 16-CURRENT with ROCr/libhsakmt/comgr/LLVM already in /opt/rocm.

## CLR (libamdhip64)
    pkg install libglvnd libX11 libdrm cmake ninja python312
    python3.12 -m pip install CppHeaderParser
    cmake -G Ninja projects/clr -DCMAKE_BUILD_TYPE=Release -DCLR_BUILD_HIP=ON -DCLR_BUILD_OCL=OFF \
      -DHIP_COMMON_DIR=$PWD/projects/hip -DHIP_PLATFORM=amd -DCMAKE_INSTALL_PREFIX=/opt/rocm \
      -DROCM_PATH=/opt/rocm -DCMAKE_PREFIX_PATH=/opt/rocm -DHIP_ENABLE_ROCPROFILER_REGISTER=OFF \
      -DCMAKE_C_COMPILER=cc -DCMAKE_CXX_COMPILER=c++ -DPython3_EXECUTABLE=/usr/local/bin/python3.12
    ninja && ninja install    # install needs /opt/rocm/bin/hipcc to exist (build hipcc first)

rocprofiler-register and OpenCL are disabled; roctracer headers are not used.
NUMA queries are stubbed (single domain).

## hipcc + clang-linker-wrapper (ROCm/llvm-project, amd-staging)
Apply `llvm-project-hipcc-freebsd.patch`, then build `amd/hipcc` with
cmake (-DCMAKE_INSTALL_PREFIX=/opt/rocm) and rebuild `clang-linker-wrapper`.
- clang-linker-wrapper: host triple passed to clang-offload-bundler must have
  four components (x86_64-unknown-freebsd16.0 was rejected), otherwise
  `--offload-new-driver` (default) fails. Workaround without the fix:
  `--no-offload-new-driver`.
- hipcc adds `-L/opt/rocm/lib -Wl,-rpath,/opt/rocm/lib -lamdhip64` since
  clang's FreeBSD toolchain does not add the HIP runtime for `--hip-link`.

## Install fixups
- hipcc looks for clang in $ROCM_PATH/lib/llvm/bin:
  `mkdir -p /opt/rocm/lib/llvm; ln -s ../../bin /opt/rocm/lib/llvm/bin; ln -s ../../lib /opt/rocm/lib/llvm/lib; ln -s lib/llvm /opt/rocm/llvm`
- libamd_comgr needs a runpath: `patchelf --set-rpath '$ORIGIN' /opt/rocm/lib/libamd_comgr.so.3*`
  (FreeBSD rtld applies DT_RUNPATH only to the object's own deps; CLR now installs with `$ORIGIN`).

## Kernel side
Needs drm-kmod-rocm `loop/kfd-thread-fixes` (KFD thread/cwsr/userptr fixes) and the
LinuxKPI idr + exit-signal fixes (freebsd-src-rocm `loop/linuxkpi-*`).
