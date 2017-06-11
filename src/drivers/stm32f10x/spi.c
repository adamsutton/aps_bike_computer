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
 * STM32 Drivers - SPI
 * 
 * Wrapper around STM standard peripheral library to support the custom HAL
 * interface
 *
 * This is NOT using DMA, it's polling the data in and out
 *
 * It's fixed as Master, 8-bit, CPOL=0, CPHA=1, MSB first
 * ***************************************************************************/

#include "board.h"
#include "abc_misc.h"
#include "hal/spi.h"

#include <stm32f10x.h>

/*
 * Structure used to represent SPI
 */
struct spi
{
  SPI_TypeDef *s_hw;                     /**< HW interface */
};

/*
 * Module data
 */
static spi_s spis[2];

/* ****************************************************************************
 * Hardware Setup
 * ***************************************************************************/

/*
 * Initialise USART1
 */
static void
_spi1_init ( void )
{
#if ABC_SPI_SPI1
  GPIO_InitTypeDef gi;

  /* Enable peripherals */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 |
                         RCC_APB2Periph_AFIO | 
                         RCC_APB2Periph_GPIOA, ENABLE);

  /* Enable GPIO */
  gi.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  gi.GPIO_Speed = GPIO_Speed_50MHz;
  gi.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &gi);
#endif
}

/*
 * Initialise USART2
 */
static void
_spi2_init ( void )
{
#if ABC_SPI_SPI2
#endif
}

/* ****************************************************************************
 * Public Interface
 * ***************************************************************************/

/*
 * Initialise the SPI subsystem
 */
void
spi_init ( void )
{
  _spi1_init();
  _spi2_init();

  spis[0].s_hw = SPI1;
  spis[1].s_hw = NULL;
}

/*
 * Open SPI
 */
spi_s *
spi_open ( uint8_t idx, uint32_t speed )
{
  SPI_InitTypeDef si;

  /* Invalid */
  if (idx >= ARRAY_SIZE(spis))
    return NULL;
  if (NULL == spis[idx].s_hw)
    return NULL;

  /* Calculate pre-scaler */
  uint8_t prescaler = 1;
  while (speed < (50000000u / (1u << prescaler))) {
    ++prescaler;
    if (prescaler == 8) break;
  }

  /* Configure the SPI */
  si.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
	si.SPI_Mode              = SPI_Mode_Master;
	si.SPI_DataSize          = SPI_DataSize_8b;
	si.SPI_CPOL              = SPI_CPOL_Low;
	si.SPI_CPHA              = SPI_CPHA_1Edge;
	si.SPI_NSS               = SPI_NSS_Soft;
	si.SPI_BaudRatePrescaler = (uint16_t)((prescaler - 1) * 8);
	si.SPI_FirstBit          = SPI_FirstBit_MSB;
	si.SPI_CRCPolynomial     = 7;
	SPI_Cmd (spis[idx].s_hw, DISABLE);
	SPI_Init(spis[idx].s_hw, &si);
	SPI_Cmd (spis[idx].s_hw, ENABLE);

  /* Return object */
  return spis + idx;
}

/*
 * Close the SPI
 *
 * @param spi The SPI to close
 */
void
spi_close ( spi_s *spi )
{
	SPI_Cmd (spi->s_hw, DISABLE);
}

/*
 * Send and receive
 */
bool spi_tx_rx
  ( spi_s *spi,
    const uint8_t *txbuf, const size_t txlen,
          uint8_t *rxbuf, const size_t rxlen )
{
  size_t i;

  /* Send data */
  i = 0;
  while (i < txlen) {
    spi->s_hw->DR = txbuf[i];
	  while ((spi->s_hw->SR & SPI_I2S_FLAG_RXNE) == 0);
	  (void)spi->s_hw->DR;
	  ++i;
  }

  /* Receive data */
  i = 0;
  while (i < rxlen) {
    spi->s_hw->DR = 0xFF;
	  while ((spi->s_hw->SR & SPI_I2S_FLAG_RXNE) == 0);
    rxbuf[i] = (uint8_t)spi->s_hw->DR;
    ++i;
  }
    
  return true;
}

/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
