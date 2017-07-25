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
 * HAL - Trace over UART
 * 
 * Implementation to send trace debug via the debug uart
 * ***************************************************************************/

#include "board.h"
#include "abc_misc.h"
#include "hal/trace.h"
#include "hal/uart.h"

#include <stdarg.h>

/* ****************************************************************************
 * State
 * ***************************************************************************/

static uart_s *trace_uart;

/* ****************************************************************************
 * Public Interface
 * ***************************************************************************/

void
trace_init ( void )
{
  trace_uart = uart_open(ABC_UART_TRACE, 9600);
}

void
trace_printf ( const char *fmt, ... )
{
  char line[128];
  va_list va;
  ssize_t c;

  /* Ignore */
  if (NULL == trace_uart) return;

  /* Build Line */
  va_start(va, fmt);
  c = vsnprintf(line, sizeof(line)-2, fmt, va);
  va_end(va);

  /* Invalid */
  if (c <= 0) return;

  /* Add \n */
  line[c++] = '\n';
  line[c]   = '\0';

  /* Send */
  uart_write(trace_uart, (uint8_t*)line, (size_t)c);
}


/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
