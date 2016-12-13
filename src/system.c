#include "system.h"

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>

int get_mac(char *if_name, unsigned char *out) {
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
    }
  }

  return ret;
}
