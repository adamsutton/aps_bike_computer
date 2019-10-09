#include "board.h"
#include "hal/uart.h"
#include "hal/spi.h"
#include "hal/sdcard.h"
#include "hal/pps.h"
#include "hal/trace.h"
#include "sensors/gps/nmea.h"
#include "storage/pff.h"
#include "storage/diskio.h"

#include <stdio.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main(int argc, char* argv[])
{
  char line[128], path[32];
  int n = 0;
  bool open = false;
  time_t now;
  double lat, lon;
  struct tm tm;

  /* Setup */
  uart_init();
  trace_init();
  spi_init();
  sdcard_init();
  disk_initialize();
  pps_init();
  trace_printf("abc - begin\n");

  /* Mount the disk */
  FATFS fs;
  if (FR_OK != pf_mount(&fs)) return 1;

  /* Open the GPS UART */
  uart_s *u = uart_open(ABC_UART_GPS, 9600);
  
  /* Read data */
  while (1) {
    if (1 != uart_read(u, (uint8_t*)(line + n), 1)) continue;
    if (line[n] == '\r') continue;
    if (line[n] == '\n') {
      line[n] = '\0';
      n       = 0;

      /* Data */
      if (nmea_gprmc(line, &tm, &lat, &lon)) {
        if (!open) {
          open = true;
          now  = mktime(&tm);
          snprintf(path, sizeof(path), "/%ld.track", now);
          if (FR_OK != pf_open(path)) return 1;
        }

        /* Write line to file */
        UINT c;
        snprintf(line, sizeof(line),
                 "{ \"time\" : %ld, \"latitude\" : %0.6f, \"longitude\" : %0.6f }\n",
                 now, lat, lon);
        pf_write(line, strlen(line), &c);
      }
    } else {
      ++n;
    }
  }
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
