//go:build !lib

// Source mode: compile C sources directly with CGO.
// Usage: go build (default)
// Requires: CGO_CFLAGS_ALLOW="-mavx.*|-msse.*|-mfma|-mavx512.*"
package stats

/*
#cgo CFLAGS: -I${SRCDIR}/include -I${SRCDIR}/modules/platform/include -I${SRCDIR}/src -I${SRCDIR}/modules/platform/src -O2 -Wall -std=c11 -D_POSIX_C_SOURCE=200112L -mavx2 -mfma -msse4.2
#cgo LDFLAGS: -lm

#include "stats.h"
#include "mean_var.h"

// Platform sources (dependency)
#include "modules/platform/src/simd_detect.c"
#include "modules/platform/src/mem_aligned.c"
#include "modules/platform/src/error.c"
#include "modules/platform/src/fc_init.c"

// Platform-specific sources
#if defined(__linux__)
  #include "modules/platform/src/platform_linux.c"
#elif defined(__APPLE__)
  #include "modules/platform/src/platform_macos.c"
#elif defined(_WIN32)
  #include "modules/platform/src/platform_win.c"
#endif

// Stats sources
#include "mean_var.c"

// Forward declarations for fc_init/fc_cleanup (no public header)
int fc_init(void);
void fc_cleanup(void);
*/
import "C"
