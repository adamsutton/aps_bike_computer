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
 * STM32 Drivers - SPI
 * 
 * Wrapper around STM standard peripheral library to support the custom HAL
 * interface
 *
 * This is NOT using DMA, it's polling the data in and out
 *
 * It's fixed as Master, 8-bit, CPOL=0, CPHA=1, MSB first
 * ***************************************************************************/

#include "board.h"
#include "abc_misc.h"
#include "hal/spi.h"

/*
 * Structure used to represent SPI
 */
struct spi
{
};

/*
 * Module data
 */
static spi_s spis[2];

/* ****************************************************************************
 * Hardware Setup
 * ***************************************************************************/

/*
 * Initialise USART1
 */
static void
_spi1_init ( void )
{
}

/*
 * Initialise USART2
 */
static void
_spi2_init ( void )
{
}

/* ****************************************************************************
 * Public Interface
 * ***************************************************************************/

/*
 * Initialise the SPI subsystem
 */
void
spi_init ( void )
{
  _spi1_init();
  _spi2_init();
}

/*
 * Open SPI
 */
spi_s *
spi_open ( uint8_t idx, uint32_t speed )
{
  /* Return object */
  return spis + idx;
}

/*
 * Close the SPI
 *
 * @param spi The SPI to close
 */
void
spi_close ( spi_s *spi )
{
}

/*
 * Send and receive
 */
bool spi_tx_rx
  ( spi_s *spi,
    const uint8_t *txbuf, const size_t txlen,
          uint8_t *rxbuf, const size_t rxlen )
{
  return true;
}

/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
