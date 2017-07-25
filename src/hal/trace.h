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
 * HAL - DEBUG interface
 *
 * Simple debug output
 *
 * ***************************************************************************/

#ifndef ABC_HAL_TRACE_H
#define ABC_HAL_TRACE_H

#include "types.h"

/*
 * Initialise
 */
void trace_init ( void );


/*
 * Output debug
 */
void trace_printf ( const char *fmt, ... );

#endif /* ABC_HAL_TRACE_H */

/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
