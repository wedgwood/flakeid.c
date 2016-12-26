#include "system.h"

#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int get_mac(const char *if_name, unsigned char *out) {
  int ret = -1;
  struct ifreq ifr;
  size_t if_name_len = strlen(if_name);

  if (if_name_len < sizeof(ifr.ifr_name)) {
    memcpy(ifr.ifr_name, if_name, if_name_len);
    ifr.ifr_name[if_name_len] = '\0';

    int fd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (fd != -1) {
      if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
        if (ifr.ifr_hwaddr.sa_family == ARPHRD_ETHER) {
          memcpy(out, ifr.ifr_hwaddr.sa_data, 6);
          ret = 0;
        }
      }

      close(fd);
    }
  }

  return ret;
}

int get_ipv4(const char *if_name, uint32_t *out) {
  int ret = -1;
  struct ifreq ifr;
  size_t if_name_len = strlen(if_name);

  if (if_name_len < sizeof(ifr.ifr_name)) {
    memcpy(ifr.ifr_name, if_name, if_name_len);
    ifr.ifr_name[if_name_len] = '\0';

    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd != -1) {
      if (ioctl(fd, SIOCGIFADDR, &ifr) == 0) {
        *out = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
        ret = 0;
      }

      close(fd);
    }
  }

  return ret;
}
