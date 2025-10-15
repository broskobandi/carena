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
