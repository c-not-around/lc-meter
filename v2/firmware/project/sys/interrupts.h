#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H


#define MAKE_IV(v)				__vector_##v

#define IV_INT0_REQUEST			MAKE_IV(1)
#define IV_INT1_REQUEST			MAKE_IV(2)
#define IV_TMR2_COMPARE			MAKE_IV(3)
#define IV_TMR2_OVERLOW			MAKE_IV(4)
#define IV_TMR1_CAPTURE			MAKE_IV(5)
#define IV_TMR1_COMPARE_A		MAKE_IV(6)
#define IV_TMR1_COMPARE_B		MAKE_IV(7)
#define IV_TMR1_OVERLOW			MAKE_IV(8)
#define IV_TMR0_OVERLOW			MAKE_IV(9)
#define IV_SPI_COMPLETE			MAKE_IV(10)
#define IV_UART_RX_COMPLETE		MAKE_IV(11)
#define IV_UART_BUF_EMPTY		MAKE_IV(12)
#define IV_UART_TX_COMPLETE		MAKE_IV(13)
#define IV_ADC_CONV_COMPLETE	MAKE_IV(14)
#define IV_EEPROM_READY			MAKE_IV(15)
#define IV_CMP_COMPARE			MAKE_IV(16)
#define IV_I2C_COMPLETE			MAKE_IV(17)
#define IV_SPM_READY			MAKE_IV(18)

#define INT_HANDLER(iv)			void __attribute__((signal)) iv(void)

#define INT_DISABLE()			asm volatile("cli\n")
#define INT_ENABLE()			asm volatile("sei\n")


#endif