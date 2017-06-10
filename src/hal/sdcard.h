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
 * HAL - SD Card interface
 *
 * Very simple interface for reading / writing sectors to an SD card
 *
 * ***************************************************************************/

#ifndef ABC_HAL_SDCARD_H
#define ABC_HAL_SDCARD_H

#include "types.h"

/**
 * Opaque reference to the SDCARD
 */
typedef struct sdcard sdcard_s;

/**
 * Chip select function
 */
typedef void (*sdcard_cs_cb) ( bool state );

/**
 * Initialise the SD Card subsystem
 */
void    sdcard_init ( void );

/**
 * Open the card
 *
 * @param idx  The SPI interface index
 *
 * @return NULL if something goes wrong
 */
sdcard_s *sdcard_open ( uint8_t idx, sdcard_cs_cb cb );

/**
 * Close the card
 *
 * @param sd   The SD card to close
 */
sdcard_s sdcard_close ( sdcard_s *sd );

/**
 * Read from the SD card
 *
 * Note: you cannot read across sector boundaries
 *
 * @param sd   The SD card to read from
 * @param sect The sector to read from
 * @param buf  The buffer to read into
 * @param len  The number of bytes to read
 *
 * @return The number of bytes read (or -1 for error)
 */
ssize_t sdcard_read
  ( sdcard_s *sd, size_t sect, uint8_t *buf, size_t len );

/**
 * Write to the SD card
 *
 * Note: you cannot write across sector boundaries
 *
 * @param sd   The SD card to write to
 * @param sect The sector to write to
 * @param buf  The data to write to the card
 * @param len  The number of bytes to write
 *
 * @return The number of bytes written (or -1 for error)
 */
ssize_t sdcard_write
  ( sdcard_s *sd, size_t sect, const uint8_t *buf, size_t len );

#endif /* ABC_HAL_SDCARD_H */

/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
