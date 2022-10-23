#ifndef CRC_POSIX
#define CRC_POSIX

#include <stddef.h>

unsigned long memcrc(const unsigned char *b, size_t n);
unsigned long streamcrc_acc(unsigned long acc, char b);
unsigned long streamcrc_end(unsigned long acc, size_t n);

#endif //CRC_POSIX
