#ifndef __FLAKEID_SYSTEM_H__
#define __FLAKEID_SYSTEM_H__

#include <stdint.h>

int get_mac(const char *if_name, unsigned char *out);
int get_ipv4(const char *if_name, uint32_t *out);

#endif //!__FLAKEID_SYSTEM_H__
