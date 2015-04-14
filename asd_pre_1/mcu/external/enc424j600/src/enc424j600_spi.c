//==========================================================================
//
//      enc424j600_spi.c
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

///////////////////////////////////////////////////////////////////////////////
//
// Modified to fit asd_pre_1 (EFM32GG and NOSYS lwip)
// Asbjørn Djupdal 2014
//
///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdio.h>
#include <em_usart.h>

#include "asd_pre_1.h"

#include "enc424j600_spi.h"
#include "enc424j600_eth.h"

#include "netif/etharp.h"

// Set ENC424J600_DEBUG to:
// 0 to suppress all printout
// 1 to print error reports
// 2 to print interrupt tracing
// 4 printout _init() progress
// 8 to print progress tracing data
// 0x10  for test with dummy stack
// 0x20  to printout status registers
#define ENC424J600_DEBUG (0)

#define ENC424J600_DONT_DROP_CS    (0)
#define ENC424J600_DO_DROP_CS      (1)
#define ENC424J600_TXBUF_START     (0)

#define ENC424J600_RXBUF_START (ENC424J600_TXBUF_START + TXBUF_SIZE)

static enc424j600_priv_data_t eth0_enc424j600_priv_data;

// Local service functions

static void enc424j600_spi_recv(enc424j600_priv_data_t *dpd, int len);
static void spiInit(void);
static void spi_transfer(uint32_t count, const uint8_t *tx_data,
                         uint8_t *rx_data, int drop);

// Interface for enc424j600 "single byte instruction".
static void
simple_operation(enc424j600_priv_data_t *dpd, enum enc424j600_spi_opcode_1_e opcode)
{
    spi_transfer(
            1,
            & opcode,
            NULL,           // not interested in answer
            ENC424J600_DO_DROP_CS);

    return;
}

// Interface for enc424j600 "banked N-byte instructions".
// Banked read, write, bit set or bit clear operation with control register
static int
banked_register_operation(enc424j600_priv_data_t *cpd,
    enum enc424j600_spi_opcode_e opcode,
    uint8_t address,
    uint16_t length,
    uint8_t *buffer)
{
    if ((NULL == buffer) || (0 == length))
    {
#if ENC424J600_DEBUG & 1
        printf("Wrong parm buffer=0x%x or length=%d.\n", (int)buffer, length);
#endif
        return -1;
    }

    if ((ENC424J600_ECON1H < address) || (opcode & ENC424J600_ECON1H))
    {
#if ENC424J600_DEBUG & 1
        printf("Wrong address=0x%02x or opcode=0x%02x for banked operation.\n",
            address, opcode);
#endif
        return -1;
    }

    address |= opcode;  // combine them in single byte
    // Send the opcode and address,
    spi_transfer(
        1,
        & address,
        NULL,           // not interested in answer
        ENC424J600_DONT_DROP_CS);
    // then the data.
    spi_transfer(
        length,
        (ENC424J600_READCR == opcode) ? NULL : buffer,
        (ENC424J600_READCR == opcode) ? buffer : NULL,
        ENC424J600_DO_DROP_CS);

    return 0;
}

// Interface for enc424j600 "unbanked N-byte instructions".
// Unbanked read, write, bit set or bit clear operation with control register
static int
unbanked_register_operation(enc424j600_priv_data_t *cpd,
    enum enc424j600_spi_opcode_u_e opcode,
    uint8_t address,
    uint16_t length,
    uint8_t *buffer)
{
    if ((NULL == buffer) || (0 == length))
    {
#if ENC424J600_DEBUG & 1
        printf("Wrong parm buffer=0x%x or length=%d.\n", (int)buffer, length);
#endif
        return -1;
    }

    // Send the opcode,
    spi_transfer(
        1,
        & opcode,
        NULL,           // not interested in answer
        ENC424J600_DONT_DROP_CS);
    // then the address
    spi_transfer(
        1,
        & address,
        NULL,           // not interested in answer
        ENC424J600_DONT_DROP_CS);
    // and finally the data.
    spi_transfer(
        length,
        (ENC424J600_READCRU == opcode) ? NULL : buffer,
        (ENC424J600_READCRU == opcode) ? buffer : NULL,
        ENC424J600_DO_DROP_CS);

    return 0;
}

// Get ESA from the Ethernet chip, unbanked
static int
read_esa_u(enc424j600_priv_data_t *cpd, uint8_t *esa_buf)
{
    int retval = -1;
    uint8_t response_msg[ETHER_ADDR_LEN];

#if ENC424J600_DEBUG & 8
    printf("read_esa_u()...\n");
#endif
    if (NULL == esa_buf)
    {
        return -1;
    }

    retval = unbanked_register_operation(cpd, ENC424J600_READCRU, ENC424J600_MAADR3_U, ETHER_ADDR_LEN,
                                         &response_msg[0]);
    if (0 != retval)
    {
        return retval;
    }
    *esa_buf++ = response_msg[4];
    *esa_buf++ = response_msg[5];
    *esa_buf++ = response_msg[2];
    *esa_buf++ = response_msg[3];
    *esa_buf++ = response_msg[0];
    *esa_buf   = response_msg[1];

    return 0;
}

/* // Set ESA into Ethernet chip, unbanked */
/* static int */
/* write_esa_u(enc424j600_priv_data_t *dpd, uint8_t *esa_buf) */
/* { */
/*     int retval = -1; */
/*     uint8_t msg[8]; */

/* #if ENC424J600_DEBUG & 8 */
/*     printf("write_esa_u()...1...\n"); */
/* #endif */
/*     if (NULL == esa_buf) */
/*     { */
/*         return -1; */
/*     } */

/*     msg[4] = *esa_buf++; */
/*     msg[5] = *esa_buf++; */
/*     msg[2] = *esa_buf++; */
/*     msg[3] = *esa_buf++; */
/*     msg[0] = *esa_buf++; */
/*     msg[1] = *esa_buf; */

/*     retval = unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MAADR3_U, 6, &msg[0]); */
/*     if (0 != retval) */
/*     { */
/* #if ENC424J600_DEBUG & 1 */
/*     printf("Writing ESA into chip failed.\n"); */
/* #endif */
/*         return retval; */
/*     } */

/*     return 0; */
/* } */

#if ENC424J600_DEBUG & 0x20
// Read data from PHY register
static void
read_phy(enc424j600_priv_data_t *dpd,
    const enum enc424j600_pr_address_e pr_address,
    uint16_t *retval_p)
{
    uint8_t aux[2] = {pr_address, 0x01};
    uint8_t status;

    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MIREGADR_U, 2, &aux[0]);
    unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MICMD_U, 2, &aux[0]);
    aux[0] |= ENC424J600_MIIRD;
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MICMD_U, 2, &aux[0]);
    usleep(26);
    do
    {   // Check and busy wait for read operation to complete
        unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MISTAT_U, 1, &status);
    } while(0 != (status & ENC424J600_MISTAT_BUSY));

    aux[0] &= !ENC424J600_MIIRD; // Clear ENC424J600_MIIRD bit
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MICMD_U, 2, &aux[0]);

    unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MIRD_U, 2, &aux[0]);
    *retval_p = aux[0] + 256 * aux[1];

    return;
}
#endif

// Write data to PHY register
static void
write_phy(enc424j600_priv_data_t *dpd,
    const enum enc424j600_pr_address_e pr_address,
    const uint16_t value)
{
    uint8_t aux[2] = {pr_address, 0x01};

    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MIREGADR_U, 2, &aux[0]);
    aux[0] = value & 0xff;
    aux[1] = (value >> 8) & 0xff;
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MIWR_U, 2, &aux[0]);

    return;
    // There must be no other MIIM operation in the following 25.6 us!
}

// Write value for on-chip pointer.
static void
set_pointer(enc424j600_priv_data_t *dpd,
    enum enc424j600_spi_bufferp_e oc_pointer,
    uint16_t value
    )
{
    spi_transfer(
        1,
        & oc_pointer,
        NULL,           // not interested in answer
        ENC424J600_DONT_DROP_CS);
    spi_transfer(
        2,
        (uint8_t *) & value,
        NULL,           // not interested in answer
        ENC424J600_DO_DROP_CS);
    return;
}

static inline void
write_to_tx_buffer(enc424j600_priv_data_t *dpd,
    uint16_t length,
    uint8_t *buffer)
{
    uint8_t oc_pointer = ENC424J600_WGPDATA; // We use General Purpose Buffer

    spi_transfer(
        1,
        & oc_pointer,
        NULL,           // not interested in answer
        ENC424J600_DONT_DROP_CS);
    spi_transfer(
        length,
        buffer,
        NULL,
        ENC424J600_DO_DROP_CS);

    return;
}

static int
read_from_rx_buffer(enc424j600_priv_data_t *dpd,
    uint16_t length,
    uint8_t *buffer)
{
    uint8_t oc_pointer = ENC424J600_RRXDATA;

//    if ((0 == length) || (NULL == buffer))
    if (0 == length)
    {
        return -1;
    }
    spi_transfer(
        1,
        & oc_pointer,
        NULL,           // not interested in answer
        ENC424J600_DONT_DROP_CS);
    spi_transfer(
        length,
        NULL,
        buffer,
        ENC424J600_DO_DROP_CS);

    return 0;
}

static void
set_MAC(enc424j600_priv_data_t *dpd,
    uint8_t ethernet_status   // As read from upper byte of ESTAT register
    )
{
    uint8_t aux[2];

    if (0 != (ethernet_status & ENC424J600_PHYDPX))
    {   // Full duplex
#if ENC424J600_DEBUG & 0x20
        printf("Full duplex\n");
#endif
        unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MACON2_U, 2, &aux[0]);
        aux[0] |= ENC424J600_FULDPX;    // Set MAC to full duplex
        unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MACON2_U, 2, &aux[0]);

        aux[1] = 0;
        aux[0] = 0x15;  // Inter-packet gap
        unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MABBIPG_U, 2, &aux[0]);
    }
    else
    {   // Half duplex
#if ENC424J600_DEBUG & 0x20
        printf("Half duplex\n");
#endif
        unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MACON2_U, 2, &aux[0]);
        aux[0] &= !ENC424J600_FULDPX; // Set MAC to half duplex
        unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MACON2_U, 2, &aux[0]);

        aux[1] = 0;
        aux[0] = 0x12;  // Inter-packet gap
        unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MABBIPG_U, 2, &aux[0]);
    }

    return;
}

static void
enc424j600_spi_LINKevent(enc424j600_priv_data_t *dpd)
{
    uint8_t iflag = ENC424J600_LINKIF;
    uint8_t aux[2];

#if ENC424J600_DEBUG & 8
    printf("enc424j600_spi_LINKevent()....\n");
#endif

    banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_ESTATL, 2, &aux[0]);
#if ENC424J600_DEBUG & 0x20
    printf("ESTAT %02x%02x Link ", aux[1], aux[0]);
#endif
    if (0 != (aux[1] & ENC424J600_PHYLNK))
    {
        dpd->link_status = ENC424J600_LINK_ON;

        // Link is just established. Set up the MAC
        set_MAC(dpd, aux[1]);

        printf("ETH link is up\n");
        netif_set_link_up(dpd->netif);
    }
    else
    {
        dpd->link_status = ENC424J600_LINK_OFF;
        dpd->netif->flags &= ~NETIF_FLAG_LINK_UP;

        printf("ETH link is down\n");
        netif_set_link_down(dpd->netif);
    }
#if ENC424J600_DEBUG & 0x20
    unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MACON1_U, 2, &aux[0]);
    printf("MACON1 %02x%02x\n", aux[1], aux[0]);
    unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MACON2_U, 2, &aux[0]);
    printf("MACON2 %02x%02x\n", aux[1], aux[0]);
    read_phy(dpd, ENC424J600_PHSTAT1, (uint16_t *) &aux[0]);
    printf("PHSTAT1 %02x%02x\n", aux[1], aux[0]);
    read_phy(dpd, ENC424J600_PHSTAT3, (uint16_t *) &aux[0]);
    printf("PHSTAT3 %02x%02x\n", aux[1], aux[0]);
    read_phy(dpd, ENC424J600_PHANLPA, (uint16_t *) &aux[0]);
    printf("PHANLPA %02x%02x\n", aux[1], aux[0]);
    read_phy(dpd, ENC424J600_PHANA, (uint16_t *) &aux[0]);
    printf("PHANA %02x%02x\n", aux[1], aux[0]);
    banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_ECON1L, 2, &aux[0]);
    printf("ECON1 %02x%02x\n", aux[1], aux[0]);
    unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_ECON2_U, 2, &aux[0]);
    printf("ECON2 %02x%02x\n", aux[1], aux[0]);

#endif

    // Clear Link interrupt flag
    banked_register_operation(dpd, ENC424J600_BITFIELDCLEAR, ENC424J600_EIRH, 1, &iflag);
}

static void
enc424j600_spi_PCFULevent(enc424j600_priv_data_t *dpd)
{
    uint8_t flags = ENC424J600_PCFULIF;

#if ENC424J600_DEBUG & 8
    printf("\nenc424j600_spi_PCFULevent()....\n\n");
#endif
    simple_operation(dpd, ENC424J600_SETPKTDEC);   // Decrement received packet counter
    banked_register_operation(dpd, ENC424J600_BITFIELDCLEAR, ENC424J600_EIRL, 1, &flags);
}

static void
enc424j600_spi_RXABTevent(enc424j600_priv_data_t *dpd)
{
    uint8_t flags = ENC424J600_RXABTIF;

#if ENC424J600_DEBUG & 8
    printf("\nenc424j600_spi_RXABTevent()....\n\n");
#endif
    banked_register_operation(dpd, ENC424J600_BITFIELDCLEAR, ENC424J600_EIRL, 1, &flags);
}

static void
enc424j600_spi_TXevent(enc424j600_priv_data_t *dpd)
{
    uint8_t iflag = ENC424J600_TXIF;

#if ENC424J600_DEBUG & 8
    printf("TX event.\n");
#endif

    dpd->txbusy = false;
    banked_register_operation(dpd, ENC424J600_BITFIELDCLEAR, ENC424J600_EIRL, 1, &iflag);
}

static void
enc424j600_spi_TXABTevent(enc424j600_priv_data_t *dpd)
{
    uint8_t iflag = ENC424J600_TXABTIF;

#if ENC424J600_DEBUG & 8
    printf("TXABT event.\n");
#endif

    dpd->txbusy = false;
    banked_register_operation(dpd, ENC424J600_BITFIELDCLEAR, ENC424J600_EIRL, 1, &iflag);
}

static void
enc424j600_spi_RXevent(enc424j600_priv_data_t *dpd)
{
    uint8_t pkcnt;    // Local copy of the packet counter
    uint8_t rsv[2];
    uint16_t packet_length;

#if ENC424J600_DEBUG & 8
    printf("enc424j600_spi_RXevent()....\n");
#endif

    // Take info on single received packet
    banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_ESTATL, 1, &pkcnt);
    if (0 != pkcnt)
    {
#if ENC424J600_DEBUG & 8
        printf("There are %u packets pending in receive buffer.\n", pkcnt);
#endif
        // Prepare on-chip read pointer
        set_pointer(dpd, ENC424J600_RXRDPT, dpd->NextPacketPointer);
        // The two bytes to read next point to the next packet
        read_from_rx_buffer(dpd, 2, (uint8_t *) & dpd->NextPacketPointer);

        // Next 6 bytes to read are the Read Status Vector. We need only the first two.
        read_from_rx_buffer(dpd, 2, & rsv[0]);
        read_from_rx_buffer(dpd, 4, NULL);
        packet_length = rsv[0] + rsv[1] * 256;

        enc424j600_spi_recv(dpd, packet_length);
    }

    return;
}

// - End of local service functions -------------------------

// - Higher level interface functions -----------------------

// Initialization of driver and chip

#define ENC424J600_INIT_SPI_READY_RETRY 100
#define ENC424J600_INIT_CLK_READY_RETRY 100
#define ENC424J600_INIT_RETRY_PERIOD_US 100

static enc424j600_priv_data_t*
hw_init(void)
{
    enc424j600_priv_data_t *dpd = &eth0_enc424j600_priv_data;

    uint8_t aux_8[2];
    unsigned int i;  // loop counter

    spiInit();

    // Check if the Ethernet chip is connected and listening
    for (i=0; i <= ENC424J600_INIT_SPI_READY_RETRY; i++)
    {
        aux_8[1] = (uint8_t)0x12;   // Test pattern
        aux_8[0] = (uint8_t)0x34;
        banked_register_operation(dpd, ENC424J600_WRITECR,
                                  ENC424J600_EUDASTL, 2, &aux_8[0]);
        banked_register_operation(dpd, ENC424J600_READCR,
                                  ENC424J600_EUDASTL, 2, &aux_8[0]);
        // Verify the test pattern
        if ((0x12 == aux_8[1]) && (0x34 == aux_8[0]))
        {   // verified
            break;
        }
        usleep(ENC424J600_INIT_RETRY_PERIOD_US);
    }

    if (i > ENC424J600_INIT_SPI_READY_RETRY)
    {
        panic("enc424j600 SPI is not ready!\n");
    }

#if ENC424J600_DEBUG & 4
        printf("enc424j600 SPI is there.\n");
#endif

    // Check if ENC424J600_CLKRDY flag is set
    for (i=0; i <= ENC424J600_INIT_CLK_READY_RETRY; i++)
    {
        banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_ESTATL, 2, &aux_8[0]);
        if (0 == (aux_8[1] & ENC424J600_CLKRDY))
        {
            i++;
        }
        else
        {   // clock is ready
            break;
        }
        usleep(ENC424J600_INIT_RETRY_PERIOD_US);
    }
    if (i > ENC424J600_INIT_CLK_READY_RETRY)
    {
        panic("enc424j600 clock is not ready!\n");
    }

#if ENC424J600_DEBUG & 4
        printf("enc424j600 clock is ready.\n");
#endif

    // Issue a System Reset for Ethernet chip
    aux_8[0] = ENC424J600_ETHRST;
    unbanked_register_operation(dpd, ENC424J600_BITFIELDSETU, ENC424J600_ECON2_U, 1, &aux_8[0]);
    usleep(25);
    // Check if the reset really happened
    banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_EUDASTL, 2, &aux_8[0]);
    if ((0 != aux_8[0]) || (0 != aux_8[1]))
    {
        panic("enc424j600 is not reset!\n");
    }
#if ENC424J600_DEBUG & 4
    printf("Successful reset of the Ethernet chip.\n");
#endif

    usleep(256);

#define CLOCKOUT_CODE(_freq_) CLOCKOUT_LABEL(_freq_)
#define CLOCKOUT_LABEL(_freq_) (ENC424J600_CLKO_##_freq_)

    // Set clock output frequency. External hardware may need it.
    unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_ECON2_U + 1, 1, &aux_8[1]);
    aux_8[1] &= 0xf0;
    aux_8[1] |= (uint8_t)CLOCKOUT_CODE(ENC424J600_CLOCKOUT_FREQUENCY);
#if ENC424J600_DEBUG & 4
    printf("Setting clock out, ECON2H 0x%02x\n", aux_8[1]);
#endif
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ECON2_U + 1, 1, &aux_8[1]);

    {
# if ENC424J600_DEBUG & 4
        printf("HW address is not provided. Getting it from the chip.\n");
# endif
        int retval = read_esa_u(dpd, dpd->hwaddr);
        if (0 != retval)
        {
            panic("Reading HW address from chip failed.\n");
        }
        printf("HW address %x:%x:%x:%x:%x:%x\n",
               dpd->hwaddr[0],
               dpd->hwaddr[1],
               dpd->hwaddr[2],
               dpd->hwaddr[3],
               dpd->hwaddr[4],
               dpd->hwaddr[5]);
    }
/*     else */
/*     { */
/* # if ENC424J600_DEBUG & 4 */
/*         printf("Setting ESA into chip. Will last till next reset.\n"); */
/* # endif */
/*         retval = write_esa_u(dpd, &(dpd->esa[0])); */
/*         if (0 != retval) */
/*         { */
/*             panic("Writing ESA into chip failed.\n"); */
/*         } */
/*     } */

    // Set receive filters
    aux_8[0] = ENC424J600_BCEN | ENC424J600_UCEN | ENC424J600_RUNTEN | ENC424J600_CRCEN;
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ERXFCON_U, 1, &aux_8[0]);

#ifdef ENC424J600_FLOWC_OnChip
#if ENC424J600_DEBUG & 4
    printf("eth_spi_enc424j600_init() Setting auto flow control\n");
#endif
    // Duration of pause
    aux_8[0] = ENC424J600_FLOWC_PAUSE & 0xff;
    aux_8[1] = (ENC424J600_FLOWC_PAUSE >> 8 ) & 0xff;
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_EPAUS_U, 2, &aux_8[0]);

    // Thresholds for flow control
    aux_8[0] = FLOWC_LOWER_WATERMARK;
    aux_8[1] = FLOWC_UPPER_WATERMARK;
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ERXWM_U, 2, &aux_8[0]);

    aux_8[0] = ENC424J600_AUTOFC;  // Enable automatic flow control
    unbanked_register_operation(dpd, ENC424J600_BITFIELDSETU, ENC424J600_ECON2_U, 1, &aux_8[0]);

#else
    // No flow control
    simple_operation(dpd, ENC424J600_FCDISABLE);
#endif

    // Set PHY speed and duplex
    aux_8[0] = aux_8[1] = 0;
#ifndef ENC424J600_NO_AUTO_NEGOTIATION
    aux_8[1] = ENC424J600_ANEN;  // Enable auto negotiation
#else
#   if(ENC424J600_SPEED == 100)
        aux_8[1] |= ENC424J600_SPD100;
#   endif
#   ifdef ENC424J600_FULL_DUPLEX
        aux_8[1] |= ENC424J600_PFULDPX;
#   endif
#endif
#if ENC424J600_DEBUG & 4
    printf("Writing PHCON1 0x%02x%02x\n", aux_8[1], aux_8[0]);
#endif
    write_phy(dpd, ENC424J600_PHCON1, (aux_8[1] << 8) + aux_8[0]);
    usleep(26);

    // Compose and set PHY capabilities advertisement register
    aux_8[1] = 0;
    aux_8[0] = ENC424J600_IEEE802_3STD;
#ifdef ENC424J600_FLOWC_ENC424J600_ONCHIP_AUTO_FC
    aux_8[1] |= ENC424J600_ADPAUS_SYMM;
#endif

#ifdef ENC424J600_NO_AUTO_NEGOTIATION
#   ifdef ENC424J600_FULL_DUPLEX
#      if(ENC424J600_SPEED == 100)
           aux_8[1] |= ENC424J600_AD100FD;
#      else
           aux_8[0] |= ENC424J600_AD10FD;
#      endif
#   else
#      if(ENC424J600_SPEED == 100)
           aux_8[0] |= ENC424J600_AD100;
#      else
           aux_8[0] |= ENC424J600_AD10;
#      endif
#   endif
#else
    aux_8[1] |= ENC424J600_AD100FD;
    aux_8[0] |= ENC424J600_AD100 | ENC424J600_AD10FD | ENC424J600_AD10;
#endif

    write_phy(dpd, ENC424J600_PHANA, (aux_8[1] << 8) + aux_8[0]);

#if ENC424J600_DEBUG & 4
    printf("Setting CRC generation\n");
#endif

    // Set ENC424J600_TXCRCEN, PADCFG
    unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MACON2_U, 2, &aux_8[0]);
    aux_8[0] = 0xa0;      // Pad VLAN frames to 64bytes, others to 60
    aux_8[0] |= ENC424J600_TXCRCEN;  // Calculate and append CRC in transmit frames
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MACON2_U, 2, &aux_8[0]);

#if ENC424J600_DEBUG & 4
    printf("Setting acceptable packet size\n");
#endif
    // Set acceptable packet size
    aux_8[0] = ENC424J600_ACCEPTABLE_PACKET_SIZE & 0xff;
    aux_8[1] = (ENC424J600_ACCEPTABLE_PACKET_SIZE >> 8) & 0xff;
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MAMXFL_U, 2, &aux_8[0]);

#if ENC424J600_DEBUG & 2
    printf("Initializing upper level driver\n");
#endif
    //(sc->funs->eth_drv->init)(sc, dpd->esa);

#if ENC424J600_DEBUG & 4
    printf("End of _init().\n");
#endif

    return dpd;
}

//
// This function is called to "start up" the interface.  It may be called
// multiple times, even when the hardware is already running.  It will be
// called whenever something "hardware oriented" changes and should leave
// the hardware ready to send/receive packets.
//
static void
enc424j600_spi_start(enc424j600_priv_data_t *dpd, unsigned char *enaddr, int flags)
{
    uint8_t aux[2];
    uint8_t pkcnt;

#if ENC424J600_DEBUG & 8
    printf("enc424j600_spi_start() begin.\n");
#endif
    // Disable packet reception
    simple_operation(dpd, ENC424J600_DISABLERX);

    // Enable interrupts in Ethernet chip
    simple_operation(dpd, ENC424J600_CLREIE); // First disable interrupts globally

    aux[0] = ENC424J600_PKTIE | ENC424J600_TXIE | ENC424J600_TXABTIE | ENC424J600_RXABTIE | ENC424J600_PCFULIE; // then set the desired ones
    aux[1] = ENC424J600_LINKIE;
    unbanked_register_operation(dpd, ENC424J600_BITFIELDSETU, ENC424J600_EIE_U, 2, &aux[0]);

    // Are there any packets left in receive buffer?
    banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_ESTATL, 1, &pkcnt);

    while (0 < pkcnt--)
    {
        simple_operation(dpd, ENC424J600_SETPKTDEC);   // Decrement on-chip packet counter
    }                                       // This clears ENC424J600_PKTIF, if set.

    // Set the chip's receive buffer start address and buffer head.
    aux[0] = ENC424J600_RXBUF_START & 0xfe;          // make it even, just in case it's not
    aux[1] = (ENC424J600_RXBUF_START >> 8) & 0x7f;   // clear the MSB
#if ENC424J600_DEBUG & 8
    printf("Setting receive buffer start address, ERXST 0x%02x%02x.\n",
                aux[1], aux[0]);
#endif
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ERXST_U, 2, &aux[0]);
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ERXHEAD_U, 2, &aux[0]);

    // Next packet will be received at the buffer start address
    *(uint8_t *) & dpd->NextPacketPointer = ENC424J600_RXBUF_START & 0xff;
    *(((uint8_t *)&dpd->NextPacketPointer) + 1) = (ENC424J600_RXBUF_START >> 8 ) & 0xff;

    // Set receive buffer tail
    aux[1] = 0x5f;  // just bellow the end of on-chip SRAM
    aux[0] = 0xfe;
#if ENC424J600_DEBUG & 8
    printf("Setting receive buffer tail, ERXTAIL 0x%02x%02x.\n",
                aux[1], aux[0]);
#endif
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ERXTAIL_U, 2, &aux[0]);

    // Abort any pending transmission
    aux[0] = ENC424J600_TXRTS;
    banked_register_operation(dpd, ENC424J600_BITFIELDCLEAR, ENC424J600_ECON1L, 1, &aux[0]);
    dpd->txbusy = false;

    // Set start of transmit buffer
    aux[0] = ENC424J600_TXBUF_START & 0xff;
    aux[1] = (ENC424J600_TXBUF_START >> 8 ) & 0xff;
#if ENC424J600_DEBUG & 8
    printf("Writing ETXST 0x%02x%02x.\n", aux[1], aux[0]);
#endif
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ETXST_U, 2, &aux[0]);

    // Clear any remaining interrupt flags
    aux[0] = ENC424J600_TXIF | ENC424J600_TXABTIF | ENC424J600_RXABTIF | ENC424J600_PCFULIF;
    aux[1] = ENC424J600_LINKIF;
    banked_register_operation(dpd, ENC424J600_BITFIELDCLEAR, ENC424J600_EIRL, 2, &aux[0]);

    simple_operation(dpd, ENC424J600_SETEIE);  // Enable interrupts

    // Enable packet reception
    simple_operation(dpd, ENC424J600_ENABLERX);

    banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_ESTATL, 2, &aux[0]);
    if (0 != (aux[1] & ENC424J600_PHYLNK))
    {
        if (ENC424J600_LINK_ON != dpd->link_status)
        {
            dpd->link_status = ENC424J600_LINK_ON;
            set_MAC(dpd, aux[1]);
        }
    }
    else
    {
        dpd->link_status = ENC424J600_LINK_OFF;
    }

#if ENC424J600_DEBUG & 8
    printf("enc424j600_spi_start() end.\n");
#endif

    return;
}

// Continue transfer from chip's buffer where the enc424j600_spi_RXevent() stopped.
// Take one packet.
static void
enc424j600_spi_recv(enc424j600_priv_data_t *dpd, int len)
{
    uint16_t rbuf_tail;
    uint8_t * aux_p;
    uint8_t aux[2];

#if ENC424J600_DEBUG & 8
    printf("enc424j600_spi_recv()...\n");
#endif

    struct pbuf *pbuf = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    struct pbuf *p = pbuf;

    while(p) {
      read_from_rx_buffer(dpd, p->len, (uint8_t *)p->payload);
      p = p->next;
    }

    aux_p = (uint8_t *) & dpd->NextPacketPointer;
    rbuf_tail = *aux_p + *(aux_p+1) * (uint16_t)256 ;
    rbuf_tail -= 2;
    if (rbuf_tail == ENC424J600_RXBUF_START)
    {   // Wrap over
        rbuf_tail = 0x5ffe;
    }
    aux[0] = rbuf_tail & 0xff;
    aux[1] = (rbuf_tail >> 8) & 0xff;
#if ENC424J600_DEBUG & 8
    printf("Setting receive buffer tail, ERXTAIL 0x%02x%02x.\n",
        aux[1], aux[0]);
#endif
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ERXTAIL_U, 2, &aux[0]);

    simple_operation(dpd, ENC424J600_SETPKTDEC);   // Decrement packet count. If zeroed,
                                        // ENC424J600_PKTIF interrupt flag will be reset.

    dpd->netif->input(pbuf, dpd->netif);

    return;
}

// This routine is called to send data to the hardware.
static void
enc424j600_spi_send(enc424j600_priv_data_t *dpd,
                    struct pbuf *p)
{
    int total = p->tot_len;
    uint8_t aux[2];
    uint8_t opcode = ENC424J600_WGPDATA;

    // Check if there is enough room in transmit buffer
    if (total > ENC424J600_RXBUF_START - ENC424J600_TXBUF_START)
    {
      //sc->funs->eth_drv->tx_done(sc, (void*)key, 0);
#if ENC424J600_DEBUG & 1
      printf("enc424j600_spi_send(): Packet to send is too large (%d bytes).\n",
            total);
#endif
        return;
    }

    dpd->txbusy = true;

    aux[0] = ENC424J600_TXBUF_START & 0xff;
    aux[1] = (((uint8_t)ENC424J600_TXBUF_START) >> 8) & 0xff;
    set_pointer(dpd, ENC424J600_GPBWRPT, (aux[1] << 8) + aux[0]);

#if ENC424J600_DEBUG & 8
    printf("Starting SPI transaction to send a packet.\n");
#endif
    spi_transfer(
        1,
        & opcode,
        NULL,
        ENC424J600_DONT_DROP_CS);

    while(p)
    {
        spi_transfer(
            p->len,
            (uint8_t *)p->payload,
            NULL,
            (!p->next) ? ENC424J600_DO_DROP_CS : ENC424J600_DONT_DROP_CS );
        p = p->next;
    }

    aux[0] = total & 0xff;
    aux[1] = (total >> 8 ) & 0xff;
#if ENC424J600_DEBUG & 8
    printf("Writing ETXLEN 0x%02x%02x.\n", aux[1], aux[0]);
#endif
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ETXLEN_U, 2, &aux[0]);

#if ENC424J600_DEBUG & 8
    printf("Setting request to send.\n");
#endif
    simple_operation(dpd, ENC424J600_SETTXRTS);

    //sc->funs->eth_drv->tx_done(sc, (void*)key, 0);

    return;
}

static void
hw_poll(enc424j600_priv_data_t *dpd)
{
    uint8_t   aux[2];
    uint16_t  iflags;

#if ENC424J600_DEBUG & 8
    //    printf("enc424j600_spi_poll(%x)...\n", (uint32_t)dpd);
#endif

    // Globally disable interrupt source in Ethernet chip
    simple_operation(dpd, ENC424J600_CLREIE);

    do
    {
        // Figure out what caused the interrupt
        banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_EIRL, 2, &aux[0]);
        aux[1] &= ENC424J600_LINKIF;
        aux[0] &= (ENC424J600_PKTIF | ENC424J600_TXIF | ENC424J600_TXABTIF | ENC424J600_RXABTIF | ENC424J600_PCFULIF);
        iflags = aux[0] + (uint16_t)256 * aux[1];
        if (0 != (aux[0] & ENC424J600_PKTIF))
        {   // Packets are pending in chip's receive buffer
            enc424j600_spi_RXevent(dpd);
        }
        if ( 0 != (aux[1] & ENC424J600_LINKIF))
        {   // Link status changed
            enc424j600_spi_LINKevent(dpd);
        }
        if (0 != (aux[0] & ENC424J600_TXIF))
        {   // Packet has been sent
            enc424j600_spi_TXevent(dpd);
        }
        if (0 != (aux[0] & ENC424J600_TXABTIF))
        {   // Packet send has been aborted
            enc424j600_spi_TXABTevent(dpd);
        }
        if ( 0 != (aux[0] & ENC424J600_PCFULIF))
        {   // Packet counter overflow
            enc424j600_spi_PCFULevent(dpd);
        }
        if ( 0 != (aux[0] & ENC424J600_RXABTIF))
        {   // Incoming packet rejected due to buffer or counter overflow
            enc424j600_spi_RXABTevent(dpd);
        }
    } while(0 != iflags);

    // Globally enable interrupt source in Ethernet chip
    simple_operation(dpd, ENC424J600_SETEIE);

    return;
}

///////////////////////////////////////////////////////////////////////////////
// spi handling

static void spiInit(void) {
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  // configure spi pins
  GPIO_PinModeSet(ETH_TX_PORT, ETH_TX_BIT, gpioModePushPull, 1);
  GPIO_PinModeSet(ETH_RX_PORT, ETH_RX_BIT, gpioModeInput, 0);
  GPIO_PinModeSet(ETH_CLK_PORT, ETH_CLK_BIT, gpioModePushPull, 0);
  GPIO_PinModeSet(ETH_CS_PORT, ETH_CS_BIT, gpioModePushPull, 1);

  USART_Reset(ETH_USART);

  // enable clock
  CMU_ClockEnable(ETH_USART_CLK, true);

  // configure
  init.baudrate = 12000000;
  init.msbf     = true;
  USART_InitSync(ETH_USART, &init);  
  
  ETH_USART->ROUTE = (ETH_USART->ROUTE & ~_USART_ROUTE_LOCATION_MASK) |
                      ETH_USART_LOC;

  ETH_USART->ROUTE |= USART_ROUTE_TXPEN | USART_ROUTE_RXPEN |
                       USART_ROUTE_CLKPEN;
}

static void spi_transfer(uint32_t count, const uint8_t *tx_data,
                         uint8_t *rx_data, int drop) {
  GPIO_PinOutClear(ETH_CS_PORT, ETH_CS_BIT);

  for(int i = 0; i < count; i++) {
    uint8_t tx = tx_data ? tx_data[i] : 0;
    uint8_t rx = USART_SpiTransfer(ETH_USART, tx);
    if(rx_data) rx_data[i] = rx;
  }

  if(drop) GPIO_PinOutSet(ETH_CS_PORT, ETH_CS_BIT);
}

///////////////////////////////////////////////////////////////////////////////
// lwip interface

err_t enc424j600_spi_linkoutput(struct netif *netif, struct pbuf *p) {
  // p is destroyed when this function returns
  enc424j600_spi_send(netif->state, p);
  return ERR_OK;
}

void enc424j600_spi_poll(struct netif *netif) {
  hw_poll(netif->state);
}

err_t enc424j600_spi_init(struct netif *netif) {
  enc424j600_priv_data_t *dpd = hw_init();
  dpd->netif = netif;
  enc424j600_spi_start(dpd, 0, 0);

  netif->state = dpd;
  netif->hwaddr_len = ETHER_ADDR_LEN;
  memcpy(netif->hwaddr, dpd->hwaddr, ETHER_ADDR_LEN);
  netif->mtu = 1500;
  memcpy(netif->name, "en", 2);
  netif->num = 0;

  netif->output = etharp_output;
  netif->linkoutput = enc424j600_spi_linkoutput;

  netif->flags |= NETIF_FLAG_ETHARP;

  return ERR_OK;
}

// End of enc424j600_spi.c
