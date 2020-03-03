#include "system.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef __APPLE__

#include <sys/socket.h>
#include <ifaddrs.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <net/if.h>

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

        if (*addr) {
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

#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

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

int get_first_hwaddr(unsigned char *out, char *name, size_t len) {
  int ret = -1;

  struct ifreq ifr;
  struct ifconf ifc;
  char buf[1024];
  int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

  if (fd != -1) {
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;

    if (ioctl(fd, SIOCGIFCONF, &ifc) == 0) {
      struct ifreq *it = ifc.ifc_req;
      const struct ifreq *end = it + (ifc.ifc_len / sizeof(struct ifreq));

      for (; it != end; ++it) {
        strcpy(ifr.ifr_name, it->ifr_name);

        if (ioctl(fd, SIOCGIFFLAGS, &ifr) == 0) {
          if (!(ifr.ifr_flags & IFF_LOOPBACK)) {
            if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
              if (ifr.ifr_hwaddr.sa_family == ARPHRD_ETHER) {
                if (name) {
                  size_t ifr_name_len = strlen(ifr.ifr_name);
                  size_t n = ifr_name_len >= len ? len - 1 : ifr_name_len;
                  strncpy(name, ifr.ifr_name, n);
                  name[n] = '\0';
                }

                memcpy(out, ifr.ifr_hwaddr.sa_data, 6);
                ret = 0;
                break;
              }
            }
          }
        }
      }
    }

    close(fd);
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

int get_first_ipv4(uint32_t *out, char *name, size_t len) {
  int ret = -1;

  struct ifreq ifr;
  struct ifconf ifc;
  char buf[1024];
  int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

  if (fd != -1) {
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;

    if (ioctl(fd, SIOCGIFCONF, &ifc) == 0) {
      struct ifreq *it = ifc.ifc_req;
      const struct ifreq *end = it + (ifc.ifc_len / sizeof(struct ifreq));

      for (; it != end; ++it) {
        strcpy(ifr.ifr_name, it->ifr_name);

        if (ioctl(fd, SIOCGIFFLAGS, &ifr) == 0) {
          if (!(ifr.ifr_flags & IFF_LOOPBACK)) {
            if (ioctl(fd, SIOCGIFADDR, &ifr) == 0) {
              if (name) {
                size_t ifr_name_len = strlen(ifr.ifr_name);
                size_t n = ifr_name_len >= len ? len - 1 : ifr_name_len;
                strncpy(name, ifr.ifr_name, n);
                name[n] = '\0';
              }

              *out = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
              ret = 0;
              break;
            }
          }
        }
      }
    }

    close(fd);
  }

  return ret;
}

#endif
