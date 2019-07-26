#include "flakeid.h"
#include "system.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>

void print_mac(unsigned char *mac) {
  printf("%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void print_ipv4(uint32_t ip) {
  printf("%u.%u.%u.%u", 0x000000FF & ip, (0x0000FF00 & ip) >> 8, (0x00FF0000 & ip) >> 16, (0xFF000000 & ip) >> 24);
}

int main() {
  unsigned char mac[6];
  int rc = get_mac("eth0", mac);

  if (rc) {
    printf("no mac address found for eth0\n");
  } else {
    printf("mac=%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  }

  flakeid_ctx_t *ctx = flakeid_ctx_create(mac, 6);
  /* flakeid_ctx_t *ctx = flakeid_ctx_create_with_spoof(NULL); */
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

  char name[1024];
  get_first_hwaddr(mac, name, sizeof(name));
  printf("name: %s, mac: ", name);
  print_mac(mac);
  printf("\n");

  uint32_t ip = 0;

  get_first_ipv4(&ip, name, sizeof(name));
  printf("name: %s:%d, ", name, ip);
  print_ipv4(ip);
  printf("\n");

  return 0;
}
