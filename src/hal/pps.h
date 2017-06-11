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
 * HAL - 1PPS input
 *
 * Driver to receive 1Hz interrupt (typically from GPS)
 *
 * ***************************************************************************/

#ifndef ABC_HAL_PPS_H
#define ABC_HAL_PPS_H

#include "types.h"

/**
 * Callback
 */
typedef void (*pps_cb) ( void );

/**
 * Initialise the subsystem/driver
 */
void pps_init ( void );

/**
 * Register a listener
 *
 * Note: this is called under interrupt
 */
void pps_add_callback ( pps_cb cb );

/**
 * Remove a listener
 */
void pps_rem_callback ( pps_cb cb );

#endif /* ABC_HAL_SDCARD_H */

/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
