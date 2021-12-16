/*******************************************************************************
  Test functions to validate the BM71 operation.

  Company:
    Microchip Technology Inc.

  File Name:
    bm71_testFunctions.h

  Summary:
    Provides functions for the BM71 UART interface.

  Description:
    The BM71 module is a BLE module that provide BT functionality.

  Remarks:

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#ifndef BM71_TESTFUNCTIONS_H
#define BM71_TESTFUNCTIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>
#include <stdbool.h>  

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility

	extern "C" {

#endif

uint8_t hello[] = "Hello Raghu!\r\n";

uint8_t rd = 0xcd;
uint8_t event00 = 0x00;
uint8_t read11 = 0x11;
uint8_t readErr = 0x99;
uint8_t handler1B = 0x1b;


uint8_t testOutput[] = "Test Output\r\n";
uint8_t success[] = "Result Good\r\n";
uint8_t failed[] = "Unexpected Response\r\n";
uint8_t readDevice[] = "Read Device name\r\n";
uint8_t currentStatus[] = "Current Status: \r\n";
uint8_t idleMode[] = "Idle Mode \r\n";


#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //PLIB_CLK_H
