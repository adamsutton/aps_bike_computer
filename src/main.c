#include "hal/uart.h"
#include "diag/Trace.h"
#include "sensors/gps/nmea.h"
#include "hal/spi.h"
#include "hal/sdcard.h"
#include "hal/pps.h"
#include "storage/pff.h"
#include "storage/diskio.h"

#include <stm32f10x.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main(int argc, char* argv[])
{
  char line[128];
  int n = 0;

  /* Setup */
  uart_init();
  spi_init();
  sdcard_init();
  disk_initialize();
  pps_init();

  /* FAT test */
  FATFS fs;
  DIR d;
  FILINFO fno;

  if (0 != pf_mount(&fs)) {
    trace_printf("failed to mount disk\n");
    return 1;
  }

  if (0 == pf_opendir(&d, "/")) {
    trace_printf("ls /\n");
    while (0 == pf_readdir (&d, &fno) && (fno.fname[0] != '\0')) {
      trace_printf("  %20s [%8d B]\n", fno.fname, fno.fsize);
    }

  } else {
    trace_printf("failed to open dir\n");
    return 1;
  }

  /* UART test */
  uart_s *u = uart_open(0, 9600);
  while (1) {
    if (1 != uart_read(u, (uint8_t*)(line + n), 1)) continue;
    if (line[n] == '\r') continue;
    if (line[n] == '\n') {
      line[n] = '\0';
      n       = 0;
      nmea_parse(line);
    } else {
      ++n;
    }
  }

}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
