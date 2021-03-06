#ifndef __FLAKEID_SYSTEM_H__
#define __FLAKEID_SYSTEM_H__

#include <stdint.h>
#include <sys/types.h>

int get_ifaddr(const char *name, unsigned char *out);
int get_first_ifaddr(unsigned char *out, char *name, size_t len);
int get_ipv4(const char *name, uint32_t *out);
int get_first_ipv4(uint32_t *out, char *if_name, size_t len);

#endif //!__FLAKEID_SYSTEM_H__
