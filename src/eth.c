#include "eth.h"
#include <port.h>
#include <lwip/etharp.h>
#include <lwip/init.h>
#include <lwip/ip4.h>
#include <lwip/netif.h>

static struct netif eth_netif;

err_t netif_eth_init(struct netif *netif);
void netif_eth_low_level_input(struct netif *netif);
err_t netif_eth_low_level_output(struct netif *netif, struct pbuf *p);

int eth_init(void) {
  lwip_init();

  ip4_addr_t eth_ipaddr, eth_netmask, eth_gw;
  IP4_ADDR(&eth_ipaddr, 172, 18, 2, 3);
  IP4_ADDR(&eth_netmask, 255, 255, 0, 0);
  IP4_ADDR(&eth_gw, 172, 18, 0, 1);

  netif_add(&eth_netif, &eth_ipaddr, &eth_netmask, &eth_gw, NULL,
            netif_eth_init, netif_input);
  netif_set_default(&eth_netif);

  netif_set_link_up(&eth_netif);
  netif_set_up(&eth_netif);

  return 0;
}

int eth_cycle(void) { netif_eth_low_level_input(&eth_netif); }

u32_t sys_now(void) { return get_tick_ms(); }

#if LWIP_IPV4
#if !LWIP_ARP
err_t netif_eth_output_ipv4(struct netif *netif, struct pbuf *p,
                            const ip4_addr_t *addr) {
  return netif_eth_low_level_output(netif, p);
}
#endif
#endif

#if LWIP_IPV6
err_t netif_eth_output_ipv6(struct netif *netif, struct pbuf *p,
                            const ip6_addr_t *addr) {
  return netif_eth_low_level_output(netif, p);
}
#endif

err_t netif_eth_init(struct netif *netif) {
#if LWIP_NETIF_HOSTNAME
  netif->hostname = ETH_HOSTNAME;
#endif
  netif->name[0] = 'e';
  netif->name[1] = 't';
#if LWIP_IPV4
#if LWIP_ARP
  netif->output = etharp_output;
#else
  netif->output = netif_eth_output_ipv4;
#endif
  netif->linkoutput = netif_eth_low_level_output;
#endif
#if LWIP_IPV6
  netif->output_ip6 = netif_loop_output_ipv6;
#endif
  // multicast
  //  netif_set_flags(netif, NETIF_FLAG_IGMP);

  // checksum
  //  NETIF_SET_CHECKSUM_CTRL(netif, NETIF_CHECKSUM_DISABLE_ALL);
  return ERR_OK;
}

void netif_eth_low_level_input(struct netif *netif) {
  struct pbuf *p = 0;

  //...

  if (p) {
    netif->input(p, netif);
  }
}

err_t netif_eth_low_level_output(struct netif *netif, struct pbuf *p) {
  return ERR_OK;
}
