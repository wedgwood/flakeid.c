#include "flakeid.h"

#include <endian.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

struct flakeid_ctx_s {
  uint64_t time;
  struct {
    unsigned char machine[6];
    uint16_t pid;
  } worker;
  uint16_t seq;
};

flakeid_ctx_t *flakeid_ctx_create(const unsigned char *machine, size_t len) {
  printf("mac=%02x:%02x:%02x:%02x:%02x:%02x\n", machine[0], machine[1], machine[2], machine[3], machine[4], machine[5]);
  flakeid_ctx_t *ret = NULL;
  flakeid_ctx_t *ctx = (flakeid_ctx_t *)calloc(sizeof(flakeid_ctx_t), 1);

  if (ctx) {
    ctx->worker.pid = htobe16(0xFFFF & getpid());
    ret = ctx;
  }

  if (machine) {
    memcpy(ctx->worker.machine, machine, len > 6 ? 6 : len);
  }

  return ret;
}

void flakeid_ctx_destroy(flakeid_ctx_t *ctx) {
  free(ctx);
}

int flakeid_updatetime(flakeid_ctx_t *ctx, struct timeval *tv) {
  int ret = -1;

  if (!tv) {
    struct timeval now;

    if (gettimeofday(&now, NULL) == 0) {
      tv = &now;
    }
  }

  if (tv) {
    ctx->time = tv->tv_sec * 1000 + tv->tv_usec / 1000;
    ret = 0;
  }

  return ret;
}

void flakeid_generate(flakeid_ctx_t *ctx, unsigned char *out) {
  uint64_t time_be = htobe64(ctx->time);
  uint16_t seq_be = htobe16(ctx->seq++);

  /* [48 bits | Timestamp, in milliseconds since the epoch] */
  /* [16 bits | a per-process counter, reset each millisecond] */
  /* [48 bits | a host identifier] */
  /* [16 bits | the process ID] */
  memcpy(out, (char *)&time_be + 2, 6);
  memcpy(out + 6, &seq_be, 2);
  memcpy(out + 8, ctx->worker.machine, 6);
  memcpy(out + 14, &ctx->worker.pid, 2);
}

int flakeid_get(flakeid_ctx_t *ctx, unsigned char *out) {
  int ret = -1;

  if (!flakeid_updatetime(ctx, NULL)) {
    flakeid_generate(ctx, out);
    ret = 0;
  }

  return ret;
}

void flakeid_hexdump(const unsigned char *id, char delimiter, unsigned char *out) {
  if (!delimiter) {
    delimiter = '-';
  }

  snprintf(
    (char *)out,
    35,
    "%02x%02x%02x%02x%02x%02x%c%02x%02x%c%02x%02x%02x%02x%02x%02x%c%02x%02x",
    id[0], id[1], id[2], id[3], id[4], id[5], delimiter,
    id[6], id[7], delimiter,
    id[8], id[9], id[10], id[11], id[12], id[13], delimiter,
    id[14], id[15]
  );
}

void flakeid_extract(const unsigned char *id, uint64_t *time, uint16_t *seq, unsigned char *mac, uint16_t *pid) {
  if (time) {
    uint64_t time_be = 0;
    memcpy((char *)&time_be + 2, id, 6);
    *time = be64toh(time_be);
  }

  if (seq) {
    uint16_t seq_be = *(uint16_t *)(id + 6);
    *seq = be16toh(seq_be);
  }

  if (mac) {
    memcpy(mac, id + 8, 6);
  }

  if (pid) {
    *pid = be16toh(*(uint16_t *)(id + 14));
  }
}
