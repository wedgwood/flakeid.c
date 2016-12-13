#ifndef __FLAKEID_H__
#define __FLAKEID_H__

#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>

typedef struct flakeid_ctx_s flakeid_ctx_t;

flakeid_ctx_t *flakeid_ctx_create(const unsigned char *machine, size_t len);
void flakeid_ctx_destroy(flakeid_ctx_t *ctx);

int flakeid_updatetime(flakeid_ctx_t *ctx, struct timeval *tv);
void flakeid_generate(flakeid_ctx_t *ctx, unsigned char *out);
int flakeid_get(flakeid_ctx_t *ctx, unsigned char *out);
void flakeid_hexdump(const unsigned char *id, char delimiter, unsigned char *out);
void flakeid_extract(const unsigned char *id, uint64_t *time, uint16_t *seq, unsigned char *mac, uint16_t *pid);

#endif //!__FLAKEID_H__
