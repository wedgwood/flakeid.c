#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <net/if_dl.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  struct ifaddrs *ifap;

  if (getifaddrs(&ifap) == 0) {
    for (struct ifaddrs *ptr = ifap; ptr; ptr = ptr->ifa_next) {
      if (ptr->ifa_addr->sa_family == AF_LINK) {
        uint8_t *mac = (uint8_t *)LLADDR((struct sockaddr_dl *)ptr->ifa_addr);
        printf("%s: %02x:%02x:%02x:%02x:%02x:%02x\n", ptr->ifa_name, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      }
    }
  }
}
