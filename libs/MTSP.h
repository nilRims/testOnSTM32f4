#include "IProtocol.h"


void InitMaster(GPIO_TypeDef *GPIOx, uint16_t Tx, uint16_t Rx, byte pinSourceRx, byte pinSourceTx,int USART_IRQn, USART_TypeDef *USARTx, byte startByte, int baudRate);

void InitSlave(GPIO_TypeDef *GPIOx, uint16_t Tx, uint16_t Rx,byte pinSourceRx, byte pinSourceTx,int USART_IRQn, USART_TypeDef *USARTx, byte startByte, byte slaveAddress, int baudRate);

void SendRequest();

void parse(byte localByte);
	
void shiftLeft(byte amount);

void onRightMessage(byte *rightMessage, int isDataRightFromSensor);
