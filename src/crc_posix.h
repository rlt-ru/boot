#ifndef CRC_POSIX
#define CRC_POSIX

#include <stddef.h>

unsigned long memcrc(const unsigned char *b, size_t n);
unsigned long streamcrc(unsigned long s, char b);
unsigned long streamcrc_end(unsigned long s, size_t n);

#endif //CRC_POSIX
