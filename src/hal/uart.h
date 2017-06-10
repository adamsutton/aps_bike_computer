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
 * HAL - UART interface
 *
 * Definition of simple UART interface, it is always run 8N1 with no HW flow
 * control (just the way I like it).
 *
 * The interface is entirely non-blocking.
 *
 * ***************************************************************************/

#ifndef ABC_HAL_UART_H
#define ABC_HAL_UART_H

#include "types.h"

/**
 * Opaque reference to the UART
 */
typedef struct uart uart_s;

/**
 * Initialise the UART subsystem
 */
void    uart_init ( void );

/**
 * Open a UART
 *
 * @param idx  The device index
 * @param baud The baud rate to operate at
 *
 * @return NULL if something goes wrong (inc if already open)
 */
uart_s *uart_open ( uint8_t idx, uint32_t baud );

/**
 * Close the UART
 *
 * @param uart The UART to close
 */
void    uart_close ( uart_s *uart );

/**
 * Read from the UART
 *
 * @param uart The UART to read from
 * @param buf  The buffer to read into
 * @param len  The size of the buffer (max read length)
 *
 * @return The number of bytes read (<0 indicates an error)
 */
ssize_t uart_read ( uart_s *uart, uint8_t *buf, size_t len );

/**
 * Write to the UART
 *
 * @param uart The UART to write to
 * @param buf  The buffer to write from
 * @param len  The length of the data in buffer
 *
 * @return The number of bytes written (<0 indicates an error)
 */
ssize_t uart_write ( uart_s *uart, const uint8_t *buf, size_t len );

#endif /* ABC_HAL_UART_H */

/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
