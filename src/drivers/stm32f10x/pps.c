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
 * STM32 Drivers - PPS input
 * 
 * External interrupt signal from PPS input pin
 * ***************************************************************************/

#include "board.h"
#include "abc_misc.h"
#include "hal/pps.h"
#include "diag/Trace.h"

#include <stm32f10x.h>

/* ****************************************************************************
 * IRQ Handler
 * ***************************************************************************/

void EXTI4_IRQHandler ( void );

void
EXTI4_IRQHandler ( void )
{
  static bool state = false;
  if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
    state = !state;
    if (state) GPIO_ResetBits(GPIOA, GPIO_Pin_5);
    else       GPIO_SetBits(GPIOA, GPIO_Pin_5);
    EXTI_ClearITPendingBit(EXTI_Line4);
  }
}

/* ****************************************************************************
 * Public Interface
 * ***************************************************************************/

void
pps_init ( void )
{
  GPIO_InitTypeDef gi;
  EXTI_InitTypeDef ei;
  NVIC_InitTypeDef ni;

  /* Setup GPIO */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  gi.GPIO_Pin   = GPIO_Pin_4;
  gi.GPIO_Speed = GPIO_Speed_50MHz;
  gi.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &gi);

  /* Setup NVIC */
  ni.NVIC_IRQChannel                   = EXTI4_IRQn;
  ni.NVIC_IRQChannelPreemptionPriority = 0x02;
  ni.NVIC_IRQChannelSubPriority        = 0x02;
  ni.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&ni);
  NVIC_EnableIRQ(EXTI4_IRQn);

  /* Setup EXTI */
  ei.EXTI_Mode    = EXTI_Mode_Interrupt;
  ei.EXTI_Line    = EXTI_Line4;
  ei.EXTI_Trigger = EXTI_Trigger_Rising;
  ei.EXTI_LineCmd = ENABLE;
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
  EXTI_Init(&ei);

  /* TODO: debug */
#if 0
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  gi.GPIO_Pin   = GPIO_Pin_5;
  gi.GPIO_Speed = GPIO_Speed_2MHz;
  gi.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &gi);
#endif
}


/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
