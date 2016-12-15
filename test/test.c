#include "flakeid.h"
#include "system.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  unsigned char mac[6];
  int rc = get_mac("eth0", mac);

  if (rc) {
    printf("no mac address found for eth0\n");
  } else {
    printf("mac=%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  }


  /* flakeid_ctx_t *ctx = flakeid_ctx_create(mac, 6); */
  flakeid_ctx_t *ctx = flakeid_ctx_create_with_spoof();
  int i = 0;

  for (; i < 10000; ++i) {
    unsigned char id[16];
    flakeid_get(ctx, id);
    unsigned char hex[36] = {'\0'};
    flakeid_hexdump(id, 0, hex);
    printf("=> %s\n", hex);
    uint64_t time;
    uint16_t seq;
    unsigned char mac[6];
    uint16_t pid;
    flakeid_extract(id, &time, mac, &pid, &seq);
    printf("=> %s: %lu-%02x:%02x:%02x:%02x:%02x:%02x-%u-%u\n", hex, time, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], pid, seq);
  }

  flakeid_ctx_destroy(ctx);
  printf("my_pid=%d\n", getpid());

  return 0;
}
