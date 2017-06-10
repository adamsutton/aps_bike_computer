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
 * Miscellaneous functions and MACROs
 * ***************************************************************************/

#ifndef ABC_MISC_H
#define ABC_MISC_H

#include "types.h"

#define ARRAY_SIZE(_x) (sizeof((_x)) / sizeof((_x)[0]))

static inline uint8_t nibble ( char c )
{
  uint8_t t = (uint8_t)c;

  if ((0x30 <= t) && (0x39 >= t))
     return (uint8_t)(t - 0x30);
  return (uint8_t)(((t | 0x20) - 0x61) + 10);
}

#endif /* ABC_MISC_H */

/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
