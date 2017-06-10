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
 * STM32 Drivers - UART
 * 
 * Wrapper around STM standard peripheral library to support the custom HAL
 * interface
 * ***************************************************************************/

#ifndef ABC_DRIVERS_STM32_UART_H
#define ABC_DRIVERS_STM32_UART_H

#include "board.h"
#include "misc.h"
#include "hal/uart.h"

#include <stm32f10x.h>

/*
 * Undefined interrupt vectors
 */
void USART1_IRQHandler ( void );
void USART2_IRQHandler ( void );

/*
 * Structure used to represent UART
 */
struct uart
{
  USART_TypeDef *u_hw;                     /**< HW interface */
  uint8_t        u_rxb[ABC_UART_RXBUF_SZ]; /**< RX buffer */
  uint8_t        u_rxi;                    /**< RX buffer input ptr */
  uint8_t        u_rxo;                    /**< RX buffer output ptr */
};

/*
 * Module data
 */
static uart_s uarts[3];

/* ****************************************************************************
 * Hardware Setup
 * ***************************************************************************/

/*
 * Initialise USART1
 */
static void
_usart1_init ( void )
{
#if ABC_UART_USART1
  GPIO_InitTypeDef gi;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 |
                         RCC_APB2Periph_AFIO | 
                         RCC_APB2Periph_GPIOA, ENABLE);

  /* Enable GPIO */
  gi.GPIO_Pin   = GPIO_Pin_9;
  gi.GPIO_Speed = GPIO_Speed_50MHz;
  gi.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &gi);

  gi.GPIO_Pin   = GPIO_Pin_10;
  gi.GPIO_Speed = GPIO_Speed_50MHz;
  gi.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &gi);

  /* Enable interrupts */
  NVIC_EnableIRQ(USART1_IRQn);
#endif
}

/*
 * Initialise USART2
 */
static void
_usart2_init ( void )
{
#if ABC_UART_USART2
  GPIO_InitTypeDef gi;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART2 |
                         RCC_APB2Periph_AFIO | 
                         RCC_APB2Periph_GPIOA, ENABLE);

  /* Enable GPIO */
  gi.GPIO_Pin   = GPIO_Pin_2;
  gi.GPIO_Speed = GPIO_Speed_50MHz;
  gi.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(ABC_UART0_PORT, &gi);

  gi.GPIO_Pin   = GPIO_Pin_3;
  gi.GPIO_Speed = GPIO_Speed_50MHz;
  gi.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &gi);

  /* Enable interrupts */
  NVIC_EnableIRQ(USART2_IRQn);
#endif
}

/* ****************************************************************************
 * IRQ Handlers
 * ***************************************************************************/

static void
_uart_irq_handler ( USART_TypeDef *hw )
{
  uint8_t c;

  /* Nothing available */
  if (!USART_GetITStatus(hw, USART_IT_RXNE)) return;

  /* Read */
  c = (uint8_t)USART_ReceiveData(hw);

  for (uint8_t i = 0; i < ARRAY_SIZE(uarts); i++) {
    if (hw != uarts[i].u_hw) continue;
    uarts[i].u_rxb[uarts[i].u_rxi++] = c;
    if (uarts[i].u_rxi >= ABC_UART_RXBUF_SZ)
      uarts[i].u_rxi = 0;
  }
}

void
USART1_IRQHandler ( void )
{
  _uart_irq_handler(USART1);
}

void
USART2_IRQHandler ( void )
{
  _uart_irq_handler(USART2);
}

/* ****************************************************************************
 * Public Interface
 * ***************************************************************************/

/*
 * Initialise the UART subsystem
 */
void
uart_init ( void )
{
  _usart1_init();
  _usart2_init();

  uarts[0].u_hw = USART1;
  uarts[1].u_hw = USART2;
}

/*
 * Open UART
 */
uart_s *
uart_open ( uint8_t idx, uint32_t baud )
{
  USART_InitTypeDef ui;

  /* Invalid */
  if (idx >= ARRAY_SIZE(uarts))
    return NULL;
  if (NULL == uarts[idx].u_hw)
    return NULL;

  /* Configure the UART */
  ui.USART_BaudRate            = baud;
  ui.USART_WordLength          = USART_WordLength_8b;  
  ui.USART_Parity              = USART_Parity_No ;
  ui.USART_StopBits            = USART_StopBits_1;   
  ui.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
  ui.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(uarts[idx].u_hw, &ui);

  /* Enable */
  USART_Cmd(uarts[idx].u_hw, ENABLE);
  USART_ITConfig(uarts[idx].u_hw, USART_IT_RXNE, ENABLE);

  /* Return object */
  return uarts + idx;
}

/**
 * Close the UART
 *
 * @param uart The UART to close
 */
void
uart_close ( uart_s *uart )
{
  USART_ITConfig(uart->u_hw, USART_IT_RXNE, DISABLE);
  USART_Cmd(uart->u_hw, DISABLE);
  USART_DeInit(uart->u_hw);
}

/**
 * Read from the UART
 *
 * @param uart The UART to read from
 * @param buf  The buffer to read into
 * @param len  The size of the buffer (max read length)
 *
 * @return The number of bytes read (<0 indicates an error)
 */
ssize_t
uart_read ( uart_s *uart, uint8_t *buf, size_t len )
{
  ssize_t n = 0;
  while ((uart->u_rxi != uart->u_rxo) && (0 != len)) {
    *buf = uart->u_rxb[uart->u_rxo];
    ++uart->u_rxo;
    if (uart->u_rxo >= ABC_UART_RXBUF_SZ)
      uart->u_rxo = 0;
    ++n;
    --len;
  }
  return n;
}

/**
 * Write to the UART
 *
 * @param uart The UART to write to
 * @param buf  The buffer to write from
 * @param len  The length of the data in buffer
 *
 * @return The number of bytes written (<0 indicates an error)
 */
ssize_t
uart_write ( uart_s *uart, const uint8_t *buf, size_t len )
{
  ssize_t n = 0;
  while ((0 != len) && (SET == USART_GetFlagStatus(USART1, USART_FLAG_TXE))) {
    USART_SendData(uart->u_hw, *buf);
    ++buf;
    --len;
    ++n;
  }
  return n;
}

#endif /* ABC_HAL_UART_H */

/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
