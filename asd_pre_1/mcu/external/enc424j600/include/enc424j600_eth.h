#ifndef CYGONCE_DEVS_ETH_ENC424J600_ETH_H_
#define CYGONCE_DEVS_ETH_ENC424J600_ETH_H_

//==========================================================================
//
//      enc424j600_eth.h
//
//      Microchip enc424j600 Ethernet chip
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2010, 2012 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later
// version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with eCos; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// As a special exception, if other files instantiate templates or use
// macros or inline functions from this file, or you compile this file
// and link it with other works to produce a work based on this file,
// this file does not by itself cause the resulting work to be covered by
// the GNU General Public License. However the source code for this file
// must still be made available in accordance with section (3) of the GNU
// General Public License v2.
//
// This exception does not invalidate any other reasons why a work based
// on this file might be covered by the GNU General Public License.
// -------------------------------------------
// ####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Ilija Stanislevik
// Contributors:
// Date:         2010-11-23
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <enc424j600_spi.h>
#include "lwip/netif.h"

#define ETHER_ADDR_LEN 6

// Ethernet flow control options
enum enc424j600_flow_control_e
{
    ENC424J600_NO_FC,            // No flow control
    ENC424J600_ONCHIP_AUTO_FC    // On-chip automatic flow control. Not tested yet.
};

// enc424j600 clock out frequencies
enum enc424j600_clkout_e
{
    ENC424J600_CLKO_0,    // DC
    ENC424J600_CLKO_33M33, ENC424J600_CLKO_25M00, ENC424J600_CLKO_20M00,
    ENC424J600_CLKO_16M67, ENC424J600_CLKO_12M50, ENC424J600_CLKO_10M00,
    ENC424J600_CLKO_8M333, ENC424J600_CLKO_8M000, ENC424J600_CLKO_6M250,
    ENC424J600_CLKO_5M000, ENC424J600_CLKO_4M000, ENC424J600_CLKO_3M125,
    ENC424J600_CLKO_0M0,  // DC
    ENC424J600_CLKO_100K, ENC424J600_CLKO_50K
};

struct enc424j600_priv_data_s;

typedef struct enc424j600_priv_data_s
{
    uint16_t                   NextPacketPointer;
    enum { ENC424J600_LINK_OFF, ENC424J600_LINK_ON } link_status;
    bool                       txbusy;
    unsigned long              txkey;
    u8_t                       hwaddr[ETHER_ADDR_LEN];
    struct netif               *netif;
  
} enc424j600_priv_data_t;

struct eth_drv_sg {
    void *buf;
    uint32_t len;
};

err_t enc424j600_spi_init(struct netif *netif);
void enc424j600_spi_poll(struct netif *netif);

#endif /* CYGONCE_DEVS_ETH_ENC424J600_ETH_H_ */

// End of enc424j600_eth.h
