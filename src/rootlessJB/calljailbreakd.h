#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
 

#ifndef PAYLOADS_COMMON_H
#define PAYLOADS_COMMON_H

#include <inttypes.h>

#define JAILBREAKD_COMMAND_FIXUP_DYLIB 8

void openjailbreakdsocket(void);
void calljailbreakd(const char *dylib);
void closejailbreakfd(void);

#endif  // PAYLOADS_COMMON_H

struct __attribute__((__packed__)) JAILBREAKD_FIXUP_DYLIB {
    uint8_t Command;
    char dylib[1024];
};
