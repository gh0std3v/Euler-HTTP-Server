#ifndef __debug__
#define __debug__

#include <stdio.h>
#include <errno.h>

#define log_error(err) fprintf(stderr, "[ERROR] %s:%d (function %s): %s (error code: %d)\n", __FILE__, __LINE__, __FUNCTION__, strerror(err), err);
#define log_debug(M, ...) fprintf(stderr, "[DEBUG] %s:%d (function %s): " M, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);
#define log_warn(M, ...) fprintf(stderr, "[WARN] %s:%d (function, %s): " M, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);
#define log_info(M, ...) printf("[INFO] %s:%d (function %s): " M, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);

#endif
