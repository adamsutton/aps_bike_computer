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

#include "hal/sdcard.h"
#include "hal/spi.h"
#include "abc_misc.h"
#include "board.h"

#include <string.h>

/* ****************************************************************************
 * SDCard - generic SPI implementation
 *
 * This is written on top of the existing SPI HAL
 * ***************************************************************************/

#define SDCARD_SPI_SPEED_SLOW   (400000)
#define SDCARD_SPI_SPEED_FAST (12500000)
#define SDCARD_RETRIES            (1000)

/* ****************************************************************************
 * Module data
 * ***************************************************************************/

struct sdcard
{
  spi_s        *sd_spi;
  sdcard_cs_cb  sd_cs;
  uint8_t       sd_idx;
  size_t        sd_sectors;
  struct {
    bool f_sdv2 : 1;
    bool f_sdhc : 1;
  }             sd_flags;
};

sdcard_s sdcards[ABC_SDCARD_NUM];

/* ****************************************************************************
 * Utility functions
 * ***************************************************************************/

/*
 * Debug
 */
//#define sdcard_printf trace_printf
#define sdcard_printf(...) ((void)0)

/*
 * Calculate 7-bit CRC
 */
static uint8_t
crc7 ( const uint8_t *data, size_t len )
{
  uint8_t crc = 0;

  while (len) {
    crc ^= *data;
    for (uint8_t i = 0; i < 8; i++) {
      if (crc & 0x80)
        crc ^= 0x89;
      crc = (uint8_t)(crc << 1);
    }
    ++data;
    --len;
  }

  return crc;
}

/*
 * Calculate 16-bit CRC
 */
static uint16_t
crc16 ( const uint8_t *data, size_t len )
{
  uint16_t crc = 0;

  while (len) {
    crc = (uint16_t)((crc >> 8) | (crc << 8));
    crc = (uint16_t)(crc ^ *data);
    crc = (uint16_t)(crc ^ ((crc & 0xFF) >> 4));
    crc = (uint16_t)(crc ^ ((crc << 8) << 4));
    crc = (uint16_t)(crc ^ (((crc & 0xFF) << 4) << 1));
    --len;
    ++data;
  }

  return crc;
}

/*
 * Set chip select state
 */
static void
sdcard_cs ( sdcard_s *sd, bool state )
{
  if (sd->sd_cs) sd->sd_cs(state);
}

/*
 * Dummy bytes
 */
static void
sdcard_nec ( sdcard_s *sd )
{
  static const uint8_t nec[]
    = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  spi_tx_rx(sd->sd_spi, nec, sizeof(nec), NULL, 0);
}

/*
 * Send a command
 */
static void
sdcard_cmd ( sdcard_s *sd, uint8_t cmd, uint32_t arg )
{
  /* Build TX buffer */
  uint8_t txbuf[] = {
    (cmd | 0x40),
    (uint8_t)((arg >> 24) & 0xFF),
    (uint8_t)((arg >> 16) & 0xFF),
    (uint8_t)((arg >>  8) & 0xFF),
    (uint8_t)((arg >>  0) & 0xFF),
    0,
  };

  /* Calculate the CRC */
  txbuf[5] = crc7(txbuf, 5) | 0x1;

  /* Debug */
  sdcard_printf("sdcard: cmd(%02X %02X %02X %02X %02X %02X)\n",
                txbuf[0], txbuf[1], txbuf[2], txbuf[3], txbuf[4], txbuf[5]);

  /* Send the command */
  spi_tx_rx(sd->sd_spi, txbuf, 6, NULL, 0);
}

/*
 * Get R1 response
 */
static uint8_t
sdcard_get_r1 ( sdcard_s *sd )
{
  uint8_t  r = 0;
  uint16_t tries = SDCARD_RETRIES;

  while (--tries) {
    spi_tx_rx(sd->sd_spi, NULL, 0, &r, 1);
    if (0 == (r & 0x80))
      return r;
  }

  return 0xFF;
}

/*
 * Get R7 response
 */
static uint8_t
sdcard_get_r7 ( sdcard_s *sd, uint32_t *r7 )
{
  uint8_t r1;
  uint8_t tmp[4];

  /* Get R1 */
  r1 = sdcard_get_r1(sd);
  if (0x01 != r1) return r1;

  /* Get R7 */
  spi_tx_rx(sd->sd_spi, NULL, 0, tmp, 4);
  *r7 = (uint32_t)(tmp[0] << 24)
      | (uint32_t)(tmp[1] << 16)
      | (uint32_t)(tmp[2] <<  8)
      | (uint32_t)(tmp[3] <<  0);

  return r1;
}
#define sdcard_get_r3 sdcard_get_r7

/*
 * Get data
 */
static bool
sdcard_get_data ( sdcard_s *sd, uint8_t *buf, size_t len )
{
  int16_t  tries = SDCARD_RETRIES;
  uint8_t  start;
  uint8_t  tmp[2];
  uint16_t crc1, crc2;

  /* Wait for first start byte */
  while (--tries) {
    spi_tx_rx(sd->sd_spi, NULL, 0, &start, 1);
    if (0xFE == start) break;
  }
  if (0 > tries) return false;

  /* Read the bytes */
  spi_tx_rx(sd->sd_spi, NULL, 0, buf, len);

  /* Read the CRC */
  spi_tx_rx(sd->sd_spi, NULL, 0, tmp, 2);
  crc1 = (uint16_t)((tmp[0] << 8) | tmp[1]);

  /* Calculate CRC */
  crc2 = crc16(buf, len);
  if (crc1 != crc2) {
    sdcard_printf("sdcard: crc mismatch (%02X != %02X)\n", crc1, crc2);
    return false;
  }
  
  return true;
}

/*
 * Put data
 */
static bool
sdcard_put_data ( sdcard_s *sd, const uint8_t *buf, size_t len )
{
  int32_t  tries = SDCARD_RETRIES;
  uint8_t  start = 0xFE;
  uint8_t  tmp[2];
  uint16_t crc;

  /* Calculate CRC */
  crc = crc16(buf, len);
  tmp[0] = (uint8_t)((crc >> 8) & 0xFF);
  tmp[1] = (uint8_t)(crc & 0xFF);

  /* Send start byte */
  spi_tx_rx(sd->sd_spi, &start, 1, NULL, 0);

  /* Send the data */
  spi_tx_rx(sd->sd_spi, buf, len, NULL, 0);

  /* Send CRC */
  spi_tx_rx(sd->sd_spi, tmp, 2, NULL, 0);

  /* Dummy read */
  while (--tries) {
    spi_tx_rx(sd->sd_spi, NULL, 0, tmp, 1);
    if (0xFF != tmp[0]) break;
  }
  if (0 > tries) return false;

  /* Poll for busy */
  tries = 100000;
  while (--tries) {
    spi_tx_rx(sd->sd_spi, NULL, 0, tmp+1, 1);
    if (0xFF == tmp[1]) break;
  }
  if (0 > tries) return false;

  /* OK */
  if ((tmp[0] & 0x1F) == 0x05) return true;

  /* Failed */
  return false;
}

/* ****************************************************************************
 * Card Setup
 * ***************************************************************************/

static bool
sdcard_read_cid ( sdcard_s *sd )
{
  bool r;
  uint8_t r1, buf[16];

  /* Send request */
  sdcard_cs(sd, false);
  sdcard_cmd(sd, 10, 0);
  r1 = sdcard_get_r1(sd);
  if ((0xFF == r1) || (0xFE & r1)) {
    sdcard_cs(sd, true);
    return false;
  }

  /* Read response */
  r = sdcard_get_data(sd, buf, 16);
  sdcard_nec(sd);
  sdcard_cs(sd, true);
  if (!r) return false;

  /* Output */
  sdcard_printf("sdcard: mid:%02X oid:%c%c pnm:%c%c%c%c%c prv:%d.%d\n"
                "        psn:%02X%02X%02X%02X mdt:%d/%d\n",
                buf[0],
                buf[1], buf[2],
                buf[3], buf[4], buf[5], buf[6], buf[7],
                buf[8] >> 4, buf[8] & 0xF,
                buf[9], buf[10], buf[11], buf[12],
                2000 + ((buf[13] << 4) | (buf[14] >> 4)), 1 + (buf[14] & 0xF));

  return true;
}

static bool
sdcard_read_csd ( sdcard_s *sd )
{
  bool r;
  uint8_t r1, buf[16];

  /* Send request */
  sdcard_cs(sd, false);
  sdcard_cmd(sd, 9, 0);
  r1 = sdcard_get_r1(sd);
  if ((0xFF == r1) || (0xFE & r1)) {
    sdcard_cs(sd, true);
    return false;
  }

  /* Read response */
  r = sdcard_get_data(sd, buf, 16);
  sdcard_nec(sd);
  sdcard_cs(sd, true);
  if (!r) return false;

  /* V1 */
  if (1 == ((buf[0] >> 6) + 1)) {
    sd->sd_sectors = (size_t)((((buf[6] & 0x3) << 10)
                                 | (buf[7] << 2)
                                 |  (buf[8] >> 6)) + 1);
  } else {
    sd->sd_sectors = (size_t)((buf[7] << 16)
                               | (buf[8] <<  8)
                               | buf[9]);
    sd->sd_flags.f_sdhc = true;
    sdcard_printf("sdcard: card is SDHC\n");
  }

  /* Output */
  sdcard_printf("sdcard: sectors  %d\n", sd->sd_sectors);
  sdcard_printf("sdcard: capacity %d B\n", sd->sd_sectors * 512);

  return true;
}


static bool
sdcard_setup ( sdcard_s *sd )
{
  uint8_t  r1;
  uint32_t r7, r3, init = 0;
  int16_t  tries = SDCARD_RETRIES;

  /* Wait */
  sdcard_cs(sd, true);
  sdcard_nec(sd);

  /* Reset the card */
  sdcard_cs(sd, false);
  sdcard_cmd(sd, 0, 0);
  r1 = sdcard_get_r1(sd);
  sdcard_nec(sd);
  sdcard_cs(sd, true);
  if (0x01 != r1) return false;

  /* Check if this is SD v1 or v2 */
  sdcard_cs(sd, false);
  sdcard_cmd(sd, 8, 0x1AA);
  r1 = sdcard_get_r7(sd, &r7);
  sdcard_nec(sd);
  sdcard_cs(sd, true);
  if (0xFF == r1) return false;
  if (0x01 == r1) {
    sd->sd_flags.f_sdv2 = true;
    sdcard_printf("sdcard: SD v2 supported\n");
    init |= (1 << 30);
  } else if (0x04 & r1) {
    sd->sd_flags.f_sdv2 = false;
    sdcard_printf("sdcard: SD v1 supported\n");
  } else {
    return false;
  }

  /* Get supply voltage */
  sdcard_cs(sd, false);
  sdcard_cmd(sd, 58, 0);
  r1 = sdcard_get_r3(sd, &r3);
  sdcard_nec(sd);
  sdcard_cs(sd, true);
  if ((r1 != 0x01) && !(r1 & 0x4)) return false;
  // TODO: output voltage?

  /* Initialise card */
  do {
    sdcard_cs(sd, false);
    sdcard_cmd(sd, 55, 0);
    r1 = sdcard_get_r1(sd);
    sdcard_nec(sd);
    sdcard_cs(sd, true);
    if (0xFF == r1) return false;
    if (0xFE & r1)  return false;

    sdcard_cs(sd, false);
    sdcard_cmd(sd, 41, init);
    r1 = sdcard_get_r1(sd);
    sdcard_nec(sd);
    sdcard_cs(sd, true);
    if (0xFF == r1) return false;
    if (0xFE & r1)  return false;
  } while (0 != r1 && --tries);
  if (0 > tries) return false;
  sdcard_printf("sdcard: initialisation started\n");

  /* Read OCR (again) */
  sdcard_cs(sd, false);
  sdcard_cmd(sd, 58, 0);
  r1 = sdcard_get_r3(sd, &r3);
  sdcard_nec(sd);
  sdcard_cs(sd, true);
  if (0xFF == r1) return false;
  if (0xFE & r1)  return false;

  // TODO: do we care about voltages?

  /* SDHC? */
  if (r3 & (1 << 30)) {
    sdcard_printf("sdcard: SDHC supported\n");
    sd->sd_flags.f_sdhc = true;
    // TODO: is this a problem as SDHC uses 1024B blocks?
  }

  /* Set block length (512B) */
  if (!sd->sd_flags.f_sdhc) {
    sdcard_cs(sd, false);
    sdcard_cmd(sd, 16, 512);
    r1 = sdcard_get_r1(sd);
    sdcard_nec(sd);
    sdcard_cs(sd, true);
    if (0xFF == r1) return false;
    if (0xFE & r1)  return false;
    sdcard_printf("sdcard: block length set 512B\n");
  }

  /* Read CID (info only) */
  if (!sdcard_read_cid(sd)) return false;

  /* Read CSD */
  if (!sdcard_read_csd(sd)) return false;

  /* Enable CRCs */
  sdcard_cs(sd, false);
  sdcard_cmd(sd, 59, 0);
  r1 = sdcard_get_r1(sd);
  sdcard_nec(sd);
  sdcard_cs(sd, true);
  if (0xFF == r1) return false;
  if (0xFE & r1)  return false;
  sdcard_printf("sdcard: enabled CRCs\n");

  /* We're all done, re-open SPI at faster speed */
  spi_close(sd->sd_spi);
  sd->sd_spi = spi_open(sd->sd_idx, SDCARD_SPI_SPEED_FAST);
  if (NULL != sd->sd_spi) {
    sdcard_printf("sdcard: re-opened @ FAST speed\n");
  }

  return (NULL != sd->sd_spi);
}

/* ****************************************************************************
 * Public API
 * ***************************************************************************/

void
sdcard_init ( void )
{
  memset(sdcards, 0, sizeof(sdcards));
}

sdcard_s *
sdcard_open ( uint8_t idx, sdcard_cs_cb cscb )
{
  int16_t tries = SDCARD_RETRIES;

  for (uint8_t i = 0; i < ABC_SDCARD_NUM; i++) {
    if (NULL != sdcards[i].sd_spi) continue;

    /* Open SPI */
    sdcards[i].sd_idx = idx;
    sdcards[i].sd_cs  = cscb;

    /* Try setup a few times */
    do {
      sdcards[i].sd_spi = spi_open(idx, SDCARD_SPI_SPEED_SLOW);
      if (sdcard_setup(sdcards + i)) break;
      if (NULL != sdcards[i].sd_spi) {
        spi_close(sdcards[i].sd_spi);
        sdcards[i].sd_spi = NULL;
      }
    } while (--tries);

    /* Done */
    if (0 < tries) return sdcards + i;
  }
  return NULL;
}

ssize_t
sdcard_read
  ( sdcard_s *sd, size_t sect, uint8_t *buf, size_t len )
{
  uint8_t r1;
  bool r;

  /* Validate */
  if (sect >= sd->sd_sectors) return -1;
  if (len > 512)              return -1;
  
  sdcard_cs(sd, false);
  if (sd->sd_flags.f_sdhc)
    sdcard_cmd(sd, 17, sect);
  else
    sdcard_cmd(sd, 17, sect * 512);
  r1 = sdcard_get_r1(sd);
  if ((0xFF == r1) || (0xFE & r1)) {
    sdcard_cs(sd, true);
    return -1;
  }

  /* Read */
  r = sdcard_get_data(sd, buf, len);
  sdcard_nec(sd);
  sdcard_cs(sd, true);

  return r ? (ssize_t)len : -1;
}

ssize_t
sdcard_write
  ( sdcard_s *sd, size_t sect, const uint8_t *buf, size_t len )
{
  uint8_t r1;
  bool r;

  /* Validate */
  if (sect >= sd->sd_sectors) return -1;
  if (len  >  512)            return -1;
  
  sdcard_cs(sd, false);
  if (sd->sd_flags.f_sdhc)
    sdcard_cmd(sd, 24, sect);
  else
    sdcard_cmd(sd, 24, sect * 512);
  r1 = sdcard_get_r1(sd);
  if ((0xFF == r1) || (0xFE & r1)) {
    sdcard_cs(sd, true);
    return -1;
  }

  /* Read */
  spi_tx_rx(sd->sd_spi, NULL, 0, &r1, 1); // dummy output
  r = sdcard_put_data(sd, buf, len);
  sdcard_nec(sd);
  sdcard_cs(sd, true);

  return r ? (ssize_t)len : -1;
}


/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
