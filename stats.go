package stats

// Package stats provides high-performance statistical computing functions
// with SIMD optimization and Go bindings to the underlying C library.
//
// Build modes:
//   - Source mode (default): go build
//     Compiles C sources directly with cgo
//   - Prebuilt mode: go build -tags lib
//     Links against prebuilt static library
//
// The cgo configuration is in cgo_source.go and cgo_prebuilt.go
//
// Key features:
// - Numerically stable algorithms (Welford's method)
// - SIMD acceleration (AVX2, SSE4.2)
// - Batch processing for multiple datasets
// - Zero-copy C interop via cgo
//
// Performance targets:
// - 5000 groups × 250 elements: < 0.3ms
// - 5x+ faster than pure Go implementations

/*
#include <stdlib.h>

// Forward declarations for fc_init/fc_cleanup (no public header)
int fc_init(void);
void fc_cleanup(void);
*/
import "C"

import (
	"fmt"
	"runtime"
	"sync"
)

// Config holds library configuration options
type Config struct {
	NumThreads    int
	SIMDLevel     string
	EnableAVX2    bool
	MemoryLimitMB uint64
	Verbose       bool
}

// DefaultConfig returns default configuration
func DefaultConfig() Config {
	return Config{
		NumThreads:    runtime.NumCPU(),
		SIMDLevel:     "auto",
		EnableAVX2:    true,
		MemoryLimitMB: 0,
		Verbose:       false,
	}
}

type libState struct {
	refCount int
	mu       sync.Mutex
}

var state = &libState{}

// Init initializes the library with default configuration
func Init() error {
	return InitWithConfig(DefaultConfig())
}

// InitWithConfig initializes the library with custom configuration
func InitWithConfig(cfg Config) error {
	state.mu.Lock()
	defer state.mu.Unlock()

	// Allow multiple Init calls (reference counting)
	if state.refCount > 0 {
		state.refCount++
		return nil
	}

	status := C.fc_init()
	if status != 0 {
		return fmt.Errorf("initialization failed with status %d", status)
	}

	state.refCount = 1
	return nil
}

// Cleanup releases library resources
func Cleanup() {
	state.mu.Lock()
	defer state.mu.Unlock()

	if state.refCount <= 0 {
		return
	}

	state.refCount--
	if state.refCount == 0 {
		C.fc_cleanup()
	}
}

// IsInitialized returns whether the library is initialized
func IsInitialized() bool {
	state.mu.Lock()
	defer state.mu.Unlock()
	return state.refCount > 0
}

