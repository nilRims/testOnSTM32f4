#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "defines.h"
#include "misc.h"

// приводим uint8_t к байту
typedef uint8_t byte;

/*
Функция Send - служит для отправки байт по UART;
Аргументы:
	USART_Typedef* USARTx - указатель на структуру c характеристиками USART, пример USART1, USART2 и т.д.(SPL);
	byte* message - указатель на массив байт, который необходимо передать по USART;
	int length - количество элементов в массиве, которое надо передать;
*/
void Send(USART_TypeDef* USARTx,byte *message, int length);

/*
Функция InitUSART - служит для настройки USART
Аргументы:
	USART_TypeDef* USARTx - указатель на структуру c характеристиками USART, пример USART1, USART2 и т.д.(SPL);
	int baudRate - скорость передачи данных (бод/c);
*/
void InitUSART(USART_TypeDef* USARTx, int baudRate);

/*
Функция InitPortsForUSART - служит для настройки портов для USART
Аргументы:
	GPIO_TypeDef* GPIOx - указатель на структуру с характеристиками GPIO, пример GPIOA, GPIOB и т.д.(SPL);
	uint16_t Tx - номер пина, на котором Tx, пример GPIO_Pin_10;
	uint16_t Rx - номер пина, на котором Rx, пример GPIO_Pin_9;
	byte pinSourceRx - пин, который надо настроить как альтернативную функцию (RX), пример GPIO_PinSource10 или 0x0A;
	byte pinSourceTx - пин, который надо настроить как альтернативную функцию (RX), пример GPIO_PinSource9 или 0x09;
*/
void InitPortsForUSART(GPIO_TypeDef* GPIOx, uint16_t Tx, uint16_t Rx, byte pinSourceRx, byte pinSourceTx);

/*
Функция InitNVIC - служит для настройки прерываний по USART
Аргументы:
	USART_TypeDef* USARTx - указатель на структуру c характеристиками USART, пример USART1, USART2 и т.д.(SPL);
	int USART_IRQn - enum из SPL, который соответствует прерыванию по заданному USART, например USART1_IRQn;
*/
void InitNVIC(USART_TypeDef* USARTx, int USART_IRQn);

byte crc8(const byte *pcBlock, uint32_t len);