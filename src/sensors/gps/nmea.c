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
 * NMEA Parser
 * ***************************************************************************/

#include "nmea.h"
#include "abc_misc.h"
#include "hal/uart.h"
#include "diag/Trace.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/*
 * Find start of next element
 */
static bool
next_element ( const char *l, const char **ptr )
{
  l = strchr(l, ',');
  if (NULL == l) return false;
  *ptr = l + 1;
  return true;
}

/*
 * Parse position element
 */
static bool
parse_pos ( const char *l, double *pos )
{
  uint32_t d;
  double   x;

  if (1 != sscanf(l, "%lf", &x)) return false;

  d    = (uint32_t)(x / 100);
  x   -= (d * 100);
  *pos = (d + (x / 60.0));

  return true;
}

static bool
parse_time ( const char *l, struct tm *tm )
{
  int u32;

  if (1 != sscanf(l, "%d", &u32)) return false;

  tm->tm_sec  = u32 % 100;
  u32 /= 100;
  tm->tm_min  = u32 % 100;
  tm->tm_hour = u32 / 100;

  return true;
}

static bool
parse_date ( const char *l, struct tm *tm )
{
  int u32;

  if (1 != sscanf(l, "%d", &u32)) return false;

  tm->tm_year  = 100 + (u32 % 100);
  u32 /= 100;
  tm->tm_mon   = (u32 % 100) - 1;
  tm->tm_mday  = u32 / 100;

  return true;
}

/*
 * Parse line
 */
void
nmea_parse ( const char *line )
{
  uint8_t csum1, csum2;
  const char *l;

  /* Invalid start */
  if ('$' != *line) return;
  l = line + 1;

  /* Calc checksum */
  csum1 = 0;
  while (('\0' != *l) && ('*' != *l)) {
    csum1 ^= (uint8_t)*l;
    ++l;
  }
  if ('\0' == *l) return;

  /* Check */
  csum2 = (uint8_t)((nibble(l[1]) << 4) + nibble(l[2]));
  if (csum1 != csum2) return;

  /* Process */
  if (NULL != strstr(line, "$GPRMC")) {
    struct tm tm = { 0 };
    double   lat, lon;

    /* Time */
    if (!parse_time(line + 7, &tm)) return;

    /* Valid? */
    if (!next_element(line+7, &l))  return;
    if ('A' != *l) return;

    /* Latitude */
    if (!parse_pos(l + 2, &lat))    return;
    if (!next_element(l + 2, &l))   return;
    if ('S' == *l) lat = -lat;

    /* Longitude */
    if (!parse_pos(l + 2, &lon))    return;
    if (!next_element(l + 2, &l))   return;
    if ('W' == *l) lon = -lon;

    /* Skip next two */
    if (!next_element(l + 2, &l))   return;
    if (!next_element(l + 0, &l))   return;

    /* Date */
    if (!parse_date(l, &tm))        return;

    char tms[32];
    strftime(tms, sizeof(tms), "%Y-%m-%d %H:%M:%S", &tm);
    trace_printf("%s : lat %0.6f lon %0.6f\n", tms, lat, lon);

  }
}


/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
