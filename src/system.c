#include "system.h"

#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef __APPLE__

#include <net/if_dl.h>

int get_ifaddr(const char *name, unsigned char *out) {
  int ret = -1;
  struct ifaddrs *ifap;

  if (getifaddrs(&ifap) == 0) {
    for (struct ifaddrs *ptr = ifap; ptr; ptr = ptr->ifa_next) {
      if (ptr->ifa_addr->sa_family == AF_LINK) {
        if (strcmp(name, ptr->ifa_name) == 0) {
          uint8_t *addr = (uint8_t *)LLADDR((struct sockaddr_dl *)ptr->ifa_addr);
          memcpy(out, addr, 6);
          ret = 0;
          break;
        }
      }
    }

    freeifaddrs(ifap);
  }

  return ret;
}

int get_first_ifaddr(unsigned char *out, char *name, size_t len) {
  int ret = -1;
  struct ifaddrs *ifap;

  if (getifaddrs(&ifap) == 0) {
    for (struct ifaddrs *ptr = ifap; ptr; ptr = ptr->ifa_next) {
      if (ptr->ifa_addr->sa_family == AF_LINK &&
        (ptr->ifa_flags & (IFF_UP | IFF_RUNNING | IFF_LOOPBACK)) == (IFF_UP | IFF_RUNNING)
      ) {
        uint8_t *addr = (uint8_t *)LLADDR((struct sockaddr_dl *)ptr->ifa_addr);
        memcpy(out, addr, 6);

        if (name) {
          size_t name_len = strlen(ptr->ifa_name);
          size_t n = name_len >= len ? len - 1 : name_len;
          strncpy(name, ptr->ifa_name, n);
          name[n] = '\0';
        }

        ret = 0;
        break;
      }
    }

    freeifaddrs(ifap);
  }

  return ret;
}

int get_ipv4(const char *name, uint32_t *out) {
  int ret = -1;
  struct ifaddrs *ifap;

  if (getifaddrs(&ifap) == 0) {
    for (struct ifaddrs *ptr = ifap; ptr; ptr = ptr->ifa_next) {
      if (ptr->ifa_addr && ptr->ifa_addr->sa_family == AF_INET) {
        if (strcmp(name, ptr->ifa_name) == 0) {
          *out = ((struct sockaddr_in *)ptr->ifa_addr)->sin_addr.s_addr;
          ret = 0;
          break;
        }
      }
    }

    freeifaddrs(ifap);
  }

  return ret;
}

int get_first_ipv4(uint32_t *out, char *name, size_t len) {
  int ret = -1;
  struct ifaddrs *ifap;

  if (getifaddrs(&ifap) == 0) {
    for (struct ifaddrs *ptr = ifap; ptr; ptr = ptr->ifa_next) {
      if (ptr->ifa_addr->sa_family == AF_INET &&
        (ptr->ifa_flags & (IFF_UP | IFF_RUNNING | IFF_LOOPBACK)) == (IFF_UP | IFF_RUNNING)
      ) {
        *out = ((struct sockaddr_in *)ptr->ifa_addr)->sin_addr.s_addr;

        if (name) {
          size_t name_len = strlen(ptr->ifa_name);
          size_t n = name_len >= len ? len - 1 : name_len;
          strncpy(name, ptr->ifa_name, n);
          name[n] = '\0';
        }

        ret = 0;
        break;
      }
    }

    freeifaddrs(ifap);
  }

  return ret;
}

#else

#include <linux/if_packet.h>

int get_ifaddr(const char *name, unsigned char *out) {
  int ret = -1;
  struct ifaddrs *ifap;

  if (getifaddrs(&ifap) == 0) {
    for (struct ifaddrs *ptr = ifap; ptr; ptr = ptr->ifa_next) {
      if (ptr->ifa_addr->sa_family == AF_PACKET) {
        if (strcmp(name, ptr->ifa_name) == 0) {
          memcpy(out, ((struct sockaddr_ll *)ptr->ifa_addr)->sll_addr, 6);
          ret = 0;
          break;
        }
      }
    }

    freeifaddrs(ifap);
  }

  return ret;
}

int get_first_ifaddr(unsigned char *out, char *name, size_t len) {
  int ret = -1;
  struct ifaddrs *ifap;

  if (getifaddrs(&ifap) == 0) {
    for (struct ifaddrs *ptr = ifap; ptr; ptr = ptr->ifa_next) {
      if (ptr->ifa_addr->sa_family == AF_PACKET &&
        (ptr->ifa_flags & (IFF_UP | IFF_RUNNING | IFF_LOOPBACK)) == (IFF_UP | IFF_RUNNING)
      ) {
        memcpy(out, ((struct sockaddr_ll *)ptr->ifa_addr)->sll_addr, 6);

        if (name) {
          size_t name_len = strlen(ptr->ifa_name);
          size_t n = name_len >= len ? len - 1 : name_len;
          strncpy(name, ptr->ifa_name, n);
          name[n] = '\0';
        }

        ret = 0;
        break;
      }
    }

    freeifaddrs(ifap);
  }

  return ret;
}

int get_ipv4(const char *name, uint32_t *out) {
  int ret = -1;
  struct ifaddrs *ifap;

  if (getifaddrs(&ifap) == 0) {
    for (struct ifaddrs *ptr = ifap; ptr; ptr = ptr->ifa_next) {
      if (ptr->ifa_addr && ptr->ifa_addr->sa_family == AF_INET) {
        if (strcmp(name, ptr->ifa_name) == 0) {
          *out = ((struct sockaddr_in *)ptr->ifa_addr)->sin_addr.s_addr;
          ret = 0;
          break;
        }
      }
    }

    freeifaddrs(ifap);
  }

  return ret;
}

int get_first_ipv4(uint32_t *out, char *name, size_t len) {
  int ret = -1;
  struct ifaddrs *ifap;

  if (getifaddrs(&ifap) == 0) {
    for (struct ifaddrs *ptr = ifap; ptr; ptr = ptr->ifa_next) {
      if (ptr->ifa_addr->sa_family == AF_INET &&
        (ptr->ifa_flags & (IFF_UP | IFF_RUNNING | IFF_LOOPBACK)) == (IFF_UP | IFF_RUNNING)
      ) {
        *out = ((struct sockaddr_in *)ptr->ifa_addr)->sin_addr.s_addr;

        if (name) {
          size_t name_len = strlen(ptr->ifa_name);
          size_t n = name_len >= len ? len - 1 : name_len;
          strncpy(name, ptr->ifa_name, n);
          name[n] = '\0';
        }

        ret = 0;
        break;
      }
    }

    freeifaddrs(ifap);
  }

  return ret;
}

#endif
