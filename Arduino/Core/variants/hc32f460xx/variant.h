/*
 Copyright (c) 2015 Arduino LLC.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef BOARD_VARIANT_H_
#define BOARD_VARIANT_H_



#define Func_GPIO       GPIO_FUNC_0
#define Func_Tim4       GPIO_FUNC_2
#define Func_Tim6       GPIO_FUNC_3
#define Func_Tima0      GPIO_FUNC_4
#define Func_Tima1      GPIO_FUNC_5
#define Func_Tima2      GPIO_FUNC_6
#define Func_Emb        GPIO_FUNC_6
#define Func_Usart_Ck   GPIO_FUNC_7
#define Func_Spi_Nss    GPIO_FUNC_7
#define Func_Qspi       GPIO_FUNC_7
#define Func_Key        GPIO_FUNC_8
#define Func_Sdio       GPIO_FUNC_9
#define Func_I2s        GPIO_FUNC_10
#define Func_UsbF       GPIO_FUNC_10
#define Func_Evnpt      GPIO_FUNC_14
#define Func_Eventout   GPIO_FUNC_15
#define Func_Usart1_Tx  GPIO_FUNC_32
#define Func_Usart3_Tx  GPIO_FUNC_32
#define Func_Usart1_Rx  GPIO_FUNC_33
#define Func_Usart3_Rx  GPIO_FUNC_33
#define Func_Usart1_Rts GPIO_FUNC_34
#define Func_Usart3_Rts GPIO_FUNC_34
#define Func_Usart1_Cts GPIO_FUNC_35
#define Func_Usart3_Cts GPIO_FUNC_35
#define Func_Usart2_Tx  GPIO_FUNC_36
#define Func_Usart4_Tx  GPIO_FUNC_36
#define Func_Usart2_Rx  GPIO_FUNC_37
#define Func_Usart4_Rx  GPIO_FUNC_37
#define Func_Usart2_Rts GPIO_FUNC_38
#define Func_Usart4_Rts GPIO_FUNC_38
#define Func_Usart2_Cts GPIO_FUNC_39
#define Func_Usart4_Cts GPIO_FUNC_39
#define Func_Spi1_Mosi  GPIO_FUNC_40
#define Func_Spi3_Mosi  GPIO_FUNC_40
#define Func_Spi1_Miso  GPIO_FUNC_41
#define Func_Spi3_Miso  GPIO_FUNC_41
#define Func_Spi1_Nss0  GPIO_FUNC_42
#define Func_Spi3_Nss0  GPIO_FUNC_42
#define Func_Spi1_Sck   GPIO_FUNC_43
#define Func_Spi3_Sck   GPIO_FUNC_43
#define Func_Spi2_Mosi  GPIO_FUNC_44
#define Func_Spi4_Mosi  GPIO_FUNC_44
#define Func_Spi2_Miso  GPIO_FUNC_45
#define Func_Spi4_Miso  GPIO_FUNC_45
#define Func_Spi2_Nss0  GPIO_FUNC_46
#define Func_Spi4_Nss0  GPIO_FUNC_46
#define Func_Spi2_Sck   GPIO_FUNC_47
#define Func_Spi4_Sck   GPIO_FUNC_47
#define Func_I2c1_Sda   GPIO_FUNC_48
#define Func_I2c3_Sda   GPIO_FUNC_48
#define Func_I2c1_Scl   GPIO_FUNC_49
#define Func_I2c3_Scl   GPIO_FUNC_49
#define Func_I2c2_Sda   GPIO_FUNC_50
#define Func_Can1_Tx    GPIO_FUNC_50
#define Func_I2c2_Scl   GPIO_FUNC_51
#define Func_Can1_Rx    GPIO_FUNC_51
#define Func_I2s1_Sd    GPIO_FUNC_52
#define Func_I2s3_Sd    GPIO_FUNC_52
#define Func_I2s1_Sdin  GPIO_FUNC_53
#define Func_I2s3_Sdin  GPIO_FUNC_53
#define Func_I2s1_Ws    GPIO_FUNC_54
#define Func_I2s3_Ws    GPIO_FUNC_54
#define Func_I2s1_Ck    GPIO_FUNC_55
#define Func_I2s3_Ck    GPIO_FUNC_55
#define Func_I2s2_Sd    GPIO_FUNC_56
#define Func_I2s4_Sd    GPIO_FUNC_56
#define Func_I2s2_Sdin  GPIO_FUNC_57
#define Func_I2s4_Sdin  GPIO_FUNC_57
#define Func_I2s2_Ws    GPIO_FUNC_58
#define Func_I2s4_Ws    GPIO_FUNC_58
#define Func_I2s2_Ck    GPIO_FUNC_59
#define Func_I2s4_Ck    GPIO_FUNC_59
//
// GPIO pin aliases (index into PIN_MAP array)
//

typedef enum
{
    PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
    PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
    PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15,
#ifdef GPIO_D
    PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15,
#endif
#ifdef GPIO_E
    PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15,
#endif
		PH0 = 80, PH1 = 81, PH2 = 82,
    PIN_MAX
} Pin_TypeDef;

//
// GPIO pin count (size of PIN_MAP array)
//
#define BOARD_NR_GPIO_PINS (PIN_MAX)


//
// USART gpio pins
//
#define VARIANT_USART1_TX_PIN PA4
#define VARIANT_USART1_RX_PIN PA11

#define VARIANT_USART2_TX_PIN PA10
#define VARIANT_USART2_RX_PIN PA15

#define VARIANT_USART3_TX_PIN PB12
#define VARIANT_USART3_RX_PIN PB13

#endif /* BOARD_VARIANT_H_ */
