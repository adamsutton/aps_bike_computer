/* ****************************************************************************
 *
 * Copyright (C) 2017 Adam Sutton
 *
 * This file is part of ApsBikeComp (ABC)
 *
 * ApsBikeComp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ApsBikeComp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ApsBikeComp.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For more details, including opportunities for alternative licensing,
 * please read the LICENSE file.
 *
 * ***************************************************************************/

/* ****************************************************************************
 * STM32 Drivers - UART
 * 
 * Wrapper around STM standard peripheral library to support the custom HAL
 * interface
 * ***************************************************************************/

#include "board.h"
#include "abc_misc.h"
#include "hal/uart.h"

#include "nrf_drv_uart.h"
#include "nrf.h"
#include "bsp.h"


/*
 * Structure used to represent UART
 */
struct uart
{
  nrf_drv_uart_t u_hw;                     /**< HW interface */
  uint8_t        u_rxb[ABC_UART_RXBUF_SZ]; /**< RX buffer */
  uint8_t        u_rxi;                    /**< RX buffer input ptr */
  uint8_t        u_rxo;                    /**< RX buffer output ptr */
  uint8_t        u_txb[ABC_UART_TXBUF_SZ]; /**< TX buffer */
  uint8_t        u_txi;                    /**< TX buffer input ptr */
  uint8_t        u_txo;                    /**< TX buffer output ptr */
};

/*
 * Module data
 */
static uart_s uarts[1];

/* ****************************************************************************
 * IRQ Handlers
 * ***************************************************************************/

static void
uart_event_handler ( nrf_drv_uart_event_t *ev, void *p )
{
  uart_s *uart = (uart_s*)p;

  if (NRF_DRV_UART_EVT_RX_DONE == ev->type) {
    nrf_drv_uart_rx(&uart->u_hw, uart->u_rxb + uart->u_rxi, 1);
    uart->u_rxi = (uart->u_rxi + 1) % ABC_UART_RXBUF_SZ;
  } else if (NRF_DRV_UART_EVT_TX_DONE == ev->type) {
    uart->u_txo = (uart->u_txo + 1) % ABC_UART_TXBUF_SZ;
    if (uart->u_txo != uart->u_txi) {
      nrf_drv_uart_tx(&uart->u_hw, uart->u_txb + uart->u_txo, 1);
    }
  }
}

/* ****************************************************************************
 * Hardware Setup
 * ***************************************************************************/

/*
 * Initialise USART1
 */
static void
_usart1_init ( void )
{
  uint8_t b;

  /* Setup instance */
  nrf_drv_uart_t tmp = NRF_DRV_UART_INSTANCE(0);
  memcpy(&uarts[0].u_hw, &tmp, sizeof(tmp));// = NRF_DRV_UART_INSTANCE(0);

  /* Configure */
  nrf_drv_uart_config_t conf = NRF_DRV_UART_DEFAULT_CONFIG;
  conf.baudrate  = NRF_UART_BAUDRATE_9600;
  conf.hwfc      = NRF_UART_HWFC_DISABLED;
  conf.parity    = NRF_UART_PARITY_EXCLUDED;
  conf.pselrxd   = 16; // for GPS
  conf.pseltxd   = 27; // on the debug header
  conf.p_context = &uarts[0];
  nrf_drv_uart_init(&uarts[0].u_hw, &conf, uart_event_handler);
  nrf_drv_uart_rx_enable(&uarts[0].u_hw);

  /* Dummy read */
  nrf_drv_uart_rx(&uarts[0].u_hw, &b, 1);
}

/* ****************************************************************************
 * Public Interface
 * ***************************************************************************/

/*
 * Initialise the UART subsystem
 */
void
uart_init ( void )
{
  _usart1_init();
}

/*
 * Open UART
 */
uart_s *
uart_open ( uint8_t idx, uint32_t baud )
{
  /* Invalid */
  if (idx >= ARRAY_SIZE(uarts))
    return NULL;

  /* Return object */
  return uarts + idx;
}

/**
 * Close the UART
 *
 * @param uart The UART to close
 */
void
uart_close ( uart_s *uart )
{
  // nothing
}

/**
 * Read from the UART
 *
 * @param uart The UART to read from
 * @param buf  The buffer to read into
 * @param len  The size of the buffer (max read length)
 *
 * @return The number of bytes read (<0 indicates an error)
 */
ssize_t
uart_read ( uart_s *uart, uint8_t *buf, size_t len )
{
  ssize_t n = 0;
  while ((uart->u_rxi != uart->u_rxo) && (0 != len)) {
    *buf = uart->u_rxb[uart->u_rxo];
    ++uart->u_rxo;
    if (uart->u_rxo >= ABC_UART_RXBUF_SZ)
      uart->u_rxo = 0;
    ++n;
    --len;
  }
  return n;
}

/**
 * Write to the UART
 *
 * @param uart The UART to write to
 * @param buf  The buffer to write from
 * @param len  The length of the data in buffer
 *
 * @return The number of bytes written (<0 indicates an error)
 */
ssize_t
uart_write ( uart_s *uart, const uint8_t *buf, size_t len )
{
  ssize_t n = 0;

  /* Copy to buffer */
  while (n < len) {
    uint8_t pin = (uart->u_txi + 1) % ABC_UART_TXBUF_SZ;
    if (pin == uart->u_txo) break;
    uart->u_txb[uart->u_txi] = buf[n];
    uart->u_txi              = pin;
    ++n;
  }

  /* Start TX */
  if (!nrf_drv_uart_tx_in_progress(&uart->u_hw)) {
    nrf_drv_uart_tx(&uart->u_hw, uart->u_txb + uart->u_txo, 1);
  }

  return n;
}

/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
