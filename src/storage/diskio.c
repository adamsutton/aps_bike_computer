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
 * Storage - DISK IO
 *
 * Glue between the PicoFAT library IO functions and the underlying SDCARD
 * API
 *
 * ***************************************************************************/

#include "diskio.h"
#include "hal/sdcard.h"

#include <string.h>

static int      di_sector = -1;
static uint8_t  di_buffer[512];
static sdcard_s *di_card;

DSTATUS
disk_initialize (void)
{
  if (NULL == di_card) {
    di_card = sdcard_open(0, NULL);
  }
  return (NULL == di_card) ? STA_NODISK : 0;
}

DRESULT
disk_readp (BYTE* buff, DWORD sector, UINT offser, UINT count)
{
  /* Read */
  if (di_sector != (int)sector) {
    if (512 != sdcard_read(di_card, sector, di_buffer, 512)) {
      di_sector = -1;
      return STA_NODISK;
    }
  }
  memcpy(buff, di_buffer + offser, count);
  return 0;
}

DRESULT disk_writep (const BYTE* buff, DWORD sc)
{
  (void)buff;
  (void)sc;
  return 0; // TODO: not yet supported
}

/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
