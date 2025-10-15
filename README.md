# carena
## Arena allocator written in C.
The goal of the project is to provide fast and convenient tools for allocating,
accessing, and deallocating objects in a global static buffer in a 
thread-safe manner.
## Features
### Thread-safe
Each thread uses its own unique arena which prevents race conditions without 
the relying on mutex locks which aids performance.
### Adjustable
If the default arena size (128KB) is too small, it is possible to extend it 
by recompiling the library with ...
```bash
cd carena &&
make clean &&
make install EXTRA_CPPFLAGS=-DCARENA_SIZE_MULTIPLIER=<N>
```
... where <N> is the number of times the arena size should be multiplied.
### Reusable
The library is based on a complex free list system that ensures that deallocated 
memory regions are reusable for new allocations which is particularly useful in
long running applications.
## Dependencies
### For error messages
- [cerror](https://gitlab.com/broskobandi/cerror.git)
### For compiling
- gcc
### For testing
- clang
- [ctest](https://gitlab.com/broskobandi/ctest.git)
### For the documentation
- doxygen
## Install
```bash
git clone https://gitlab.com/broskobandi/carena.git &&
cd carena &&
make &&
sudo make install
```
## Uninstall
```bash
cd carena &&
sudo make uninstall
```
## Testing
```bash
cd carena &&
make clean &&
make test &&
make clean
```
## Documentation
```bash
cd carena &&
make doc
```
## Usage
```c
/* Include the library. */
#include <carena.h>

int main(void) {
	/* Allocate memory. */
	void *data = carena_alloc(32);

	/* Resize memory. */
	carena_realloc(data, 64);

	/* Free memory */
	carena_free(data);

	return 0;
}
```
Compile the program using this library with the -L/usr/local/lib -lcarena flags.
## Todo
- The realloc function might need a slightly more elegant and performant rework.
- Add malloc as a fallback mechanism?
