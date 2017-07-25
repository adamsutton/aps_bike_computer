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

/* ****************************************************************************
 * IRQ Handler
 * ***************************************************************************/

/*
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
*/

/* ****************************************************************************
 * Public Interface
 * ***************************************************************************/

void
pps_init ( void )
{
}


/* ****************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 * ***************************************************************************/
