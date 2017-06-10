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
 * HAL - SPI interface
 *
 * ***************************************************************************/

#ifndef ABC_HAL_SPI_H
#define ABC_HAL_SPI_H

#include "types.h"

/**
 * Opaque reference to the SPI
 */
typedef struct spi spi_s;

/**
 * Initialise the SPI subsystem
 */
void    spi_init ( void );

/**
 * Open the SPI
 *
 * @param idx   The device index
 * @param speed The speed (in Hz) to run at
 *
 * @return NULL if something goes wrong (inc if already open)
 */
spi_s   *spi_open ( uint8_t idx, uint32_t speed );

/**
 * Close the SPI
 *
 * @param spi The SPI to close
 */
void    spi_close ( spi_s *spi );

/**
 * Transmit data to the SPI and read the response
 *
 * @param spi   The SPI to operate on
 * @param txbuf The data to transmit
 * @param txlen The number of bytes to transmit
 * @param rxbuf The buffer to receive into
 * @param rxlen The number of bytes to receive
 *
 * @return True if operation successful, else false
 */
bool spi_tx_rx
  ( spi_s *spi,
    const uint8_t *txbuf, const size_t txlen,
          uint8_t *rxbuf, const size_t rxlen );

#endif /* ABC_HAL_SPI_H */

/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
