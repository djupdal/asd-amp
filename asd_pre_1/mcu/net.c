#include "asd_pre_1.h"

#include "lwip/dhcp.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "lwip/tcp_impl.h"
#include "lwip/ip_frag.h"
#include "lwip/netif.h"
#include "lwip/init.h"
#include "lwip/stats.h"
#include "netif/etharp.h"
#include "enc424j600_eth.h"
#include "lwip/timers.h"

#define SERVER_PORT 80

struct netif netif;
struct netconn *conn;

#define NETCMD_VOL    1
#define NETCMD_SOURCE 2

extern int16_t volume;
extern int source;
void setVol(int16_t newVolume);
void setSource(int newSource);

///////////////////////////////////////////////////////////////////////////////

void netIRQ() {
  enc424j600_spi_poll(&netif);
}

static err_t server_poll(void *arg, struct tcp_pcb *pcb) {
  tcp_arg(pcb, NULL);
  tcp_recv(pcb, NULL);
  tcp_err(pcb, NULL);
  tcp_poll(pcb, NULL, 0);
  tcp_sent(pcb, NULL);

  err_t err = tcp_close(pcb);
  if (err != ERR_OK) {
    tcp_poll(pcb, server_poll, 4);
  }
  return err;
}

static err_t server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p,
                         err_t err) {
  if(err != ERR_OK) {
    server_poll(arg, pcb);
  } else {
    if(p) {
      char sendbuf[80];
      char recbuf[p->len+1];
      memcpy(recbuf, p->payload, p->len);
      recbuf[p->len] = 0;

      char *cmd = strtok(recbuf, " \t\n\r");
      char *arg = strtok(NULL, " \t\n\r");

      if(!strcmp(cmd, "SET_VOL")) {
        setVol(strtol(arg, NULL, 0));
        sprintf(sendbuf, "OK\n");

      } else if(!strcmp(cmd, "GET_VOL")) {
        sprintf(sendbuf, "VOL %d\n", volume);

      } else if(!strcmp(cmd, "GET_VOL_MAX")) {
        sprintf(sendbuf, "VOL_MAX %d\n", VOL_MIN);

      } else if(!strcmp(cmd, "GET_VOL_MIN")) {
        sprintf(sendbuf, "VOL_MIN %d\n", VOL_MAX);

      } else if(!strcmp(cmd, "SET_SOURCE")) {
        setSource(strtol(arg, NULL, 0));
        sprintf(sendbuf, "OK\n");

      } else if(!strcmp(cmd, "GET_SOURCE")) {
        sprintf(sendbuf, "SOURCE %d\n", source);

      } else if(!strcmp(cmd, "GET_SOURCES")) {
        sprintf(sendbuf, "SOURCES %d\n", SOURCES);
      }

      tcp_write(pcb, sendbuf, strlen(sendbuf), TCP_WRITE_FLAG_COPY);
    }
  }

  if(p) {
    tcp_recved(pcb, p->tot_len);
    pbuf_free(p);
  }    

  return ERR_OK;
}

static err_t server_sent(void *arg, struct tcp_pcb *pcb, u16_t len) {
  return ERR_OK;
}

static void server_err(void *arg, err_t err) {
  printf("TCP error: %s\n", lwip_strerr(err));
}

static err_t server_accept(void *arg, struct tcp_pcb *pcb, err_t err) {
  struct tcp_pcb_listen *lpcb = (struct tcp_pcb_listen*)arg;
  tcp_accepted(lpcb);

  tcp_arg(pcb, lpcb);

  // register callbacks
  tcp_recv(pcb, server_recv);
  tcp_sent(pcb, server_sent);
  tcp_err(pcb, server_err);
  tcp_poll(pcb, server_poll, 4);

  return ERR_OK;
}

void status_callback(struct netif *netif) {
  if(netif->flags & NETIF_FLAG_UP) {
    uint32_t ip_addr = netif->ip_addr.addr;
    uint32_t netmask = netif->netmask.addr;
    uint32_t gw = netif->gw.addr;

    printf("Link is up\n");
    printf("Address: %d.%d.%d.%d\n",
           (uint8_t)(ip_addr&0xff), (uint8_t)((ip_addr>>8)&0xff),
           (uint8_t)((ip_addr>>16)&0xff), (uint8_t)(ip_addr>>24));
    printf("Mask: %d.%d.%d.%d\n",
           (uint8_t)(netmask&0xff), (uint8_t)((netmask>>8)&0xff),
           (uint8_t)((netmask>>16)&0xff), (uint8_t)(netmask>>24));
    printf("GW: %d.%d.%d.%d\n",
           (uint8_t)(gw&0xff), (uint8_t)((gw>>8)&0xff),
           (uint8_t)((gw>>16)&0xff), (uint8_t)(gw>>24));

    // start server
    struct tcp_pcb *pcb = tcp_new();
    if(pcb) {
      err_t err = tcp_bind(pcb, IP_ADDR_ANY, SERVER_PORT);
      if(err == ERR_OK) {
        pcb = tcp_listen(pcb);
        if(pcb) {
          tcp_arg(pcb, pcb);
          tcp_accept(pcb, server_accept);
          printf("TCP server started\n");
        }
      }
    }
  }
}

bool netInit(void) {
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;

  GPIO_PinModeSet(nINT_ETH_PORT, nINT_ETH_BIT, gpioModeInput, 1);
  GPIO_IntConfig(nINT_ETH_PORT, nINT_ETH_BIT, true, true, true);

  // does not matter, we use DHCP
  IP4_ADDR(&gw, 192,168,1,100);
  IP4_ADDR(&ipaddr, 192,168,1,10);
  IP4_ADDR(&netmask, 255,255,255,0);

  lwip_init();

  netif_add(&netif, &ipaddr, &netmask, &gw, NULL,
            enc424j600_spi_init, ethernet_input);
  netif_set_default(&netif);

  netif_set_status_callback(&netif, status_callback);

  dhcp_start(&netif);

  return true;
}
