#include "flakeid.h"
#include "system.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>

int main() {
  /* flakeid64_ctx_t *ctx = flakeid64_ctx_create(0); */
  flakeid64_ctx_t *ctx = flakeid64_ctx_create_with_spoof();
  int i = 0;

  for (; i < 10000; ++i) {
    int64_t id;
    flakeid64_get(ctx, &id);
    printf("=> %ld\n", id);
    uint64_t time;
    uint16_t seq;
    unsigned int machine;
    flakeid64_extract(id, &time, &machine, &seq);
    printf("=> %ld: %lu-%lu-%u\n", id, time, machine, seq);
    unsigned char hex[33] = {'\0'};
    flakeid64_hexdump(id, hex);
    printf("%s\n", hex);
  }

  flakeid64_ctx_destroy(ctx);
  printf("my_pid=%d\n", getpid());

  return 0;
}
