#include "MTSP.h"

// Сделать массив структур, если и мастер и слейв на одном мк, чтобы можно было разные старт и стоп байты задавать
struct MTSP
{
	byte _isSlaveInited;
	byte _isMasterInited;
	byte _slaveAddress;
	byte _startByte;
	byte localMessage[MTSP_MESSAGE_LENGTH];
	byte inputMessage[MTSP_SENSOR_MESSAGE_LENGTH];								      // входной буфер для всех пакетов, кроме пакета с данными о датчиках
	int countData;																				      // вспомогательная переменная для парсинга
	uint32_t usartNumber;
} objMTSP;


void InitMaster(GPIO_TypeDef *GPIOx, uint16_t Tx, uint16_t Rx, byte pinSourceRx, byte pinSourceTx,int USART_IRQn, USART_TypeDef *USARTx, byte startByte, int baudRate)
{
	objMTSP._isSlaveInited = FALSE;
	objMTSP._isMasterInited = TRUE;
	objMTSP._startByte = startByte;
	objMTSP.countData = 0;
	InitPortsForUSART(GPIOx, Tx, Rx, pinSourceRx, pinSourceTx);
	InitUSART(USARTx, baudRate);
	InitNVIC(USARTx, USART_IRQn);
	objMTSP.usartNumber = (uint32_t) USARTx;
}

void InitSlave(GPIO_TypeDef *GPIOx, uint16_t Tx, uint16_t Rx,byte pinSourceRx, byte pinSourceTx,int USART_IRQn, USART_TypeDef *USARTx, byte startByte, byte slaveAddress, int baudRate)
{
	objMTSP._isMasterInited = FALSE;
	objMTSP._isSlaveInited = TRUE;
	objMTSP._startByte = startByte;
	objMTSP._slaveAddress = slaveAddress;
	objMTSP.countData = 0;
	InitPortsForUSART(GPIOx, Tx, Rx, pinSourceRx, pinSourceTx);
	InitUSART(USARTx, baudRate);
	InitNVIC(USARTx, USART_IRQn);
	objMTSP.usartNumber = (uint32_t) USARTx;
}

byte* makeMessage(byte *message)
{
	objMTSP.localMessage[START_BYTE_OFFSET] = objMTSP._startByte;
	for(int i = 0; i < 3; i++)
	{
		objMTSP.localMessage[i+1] = message[i];
	}
	objMTSP.localMessage[CRC8_OFFSET] = crc8(objMTSP.localMessage, MTSP_MESSAGE_LENGTH - 1);
	return objMTSP.localMessage;
}

void SendRequest(byte *message)
{
	if(objMTSP._isMasterInited == TRUE)
	{
		Send((USART_TypeDef*) objMTSP.usartNumber, makeMessage(message), MTSP_MESSAGE_LENGTH);
	}
}

void parse(byte localByte)
{
	
	if(objMTSP._isMasterInited == TRUE)					// Парсер для мастера
	{
		objMTSP.inputMessage[objMTSP.countData] = localByte;
		switch(objMTSP.countData)
		{
			case START_BYTE_OFFSET:
				if(localByte == objMTSP._startByte) objMTSP.countData++; 
				break;
			case SLAVE_ADDRESS_OFFSET:
				objMTSP.countData++;
				break;
			case COMAND_NUMBER_OFFSET:
				objMTSP.countData++;
				break;
			default:
				if(objMTSP.inputMessage[COMAND_NUMBER_OFFSET] == SENSOR_COMAND)
				{
					if(objMTSP.countData < MTSP_SENSOR_MESSAGE_LENGTH - 1)
					{
						objMTSP.countData++;
						break;
					}
					if(objMTSP.inputMessage[CRC8_SENSOR_OFFSET] == crc8(objMTSP.inputMessage, MTSP_SENSOR_MESSAGE_LENGTH - 1))
					{
						byte localRightMessage[MTSP_SENSOR_MESSAGE_LENGTH - 2];
						for(int i = 0; i<MTSP_SENSOR_MESSAGE_LENGTH - 1;i++)
						{
							localRightMessage[i] = objMTSP.inputMessage[i+1];
						}
						onRightMessage(localRightMessage, TRUE);
						objMTSP.countData = 0;
					}
					else
					{
						for(byte i = 0; i < MTSP_SENSOR_MESSAGE_LENGTH;i++)
						{
							if(objMTSP.inputMessage[i] == objMTSP._startByte)
							{
								objMTSP.countData = MTSP_SENSOR_MESSAGE_LENGTH - i;
								shiftLeft(i);
								return;
							}
						}
					}
									
				}
				else
				{
					if(objMTSP.countData < MTSP_MESSAGE_LENGTH - 1)
					{
						objMTSP.countData++;
						break;
					}
					if(objMTSP.inputMessage[CRC8_OFFSET] == crc8(objMTSP.inputMessage, MTSP_MESSAGE_LENGTH - 1))
					{
						
						byte localRightMessage[MTSP_MESSAGE_LENGTH - 2];
						for(int i = 0; i<MTSP_MESSAGE_LENGTH - 1;i++)
						{
							localRightMessage[i] = objMTSP.inputMessage[i+1];
						}
						onRightMessage(localRightMessage, FALSE);
						objMTSP.countData = 0;
					}
					else
					{
						for(byte i = 0; i < MTSP_MESSAGE_LENGTH;i++)
						{
							if(objMTSP.inputMessage[i] == objMTSP._startByte)
							{
								objMTSP.countData = MTSP_MESSAGE_LENGTH - i;
								shiftLeft(i);
								return;
							}
						}
					}
				}
				
				break;
		}
	}
	else
	{
		if(objMTSP._isSlaveInited == TRUE)		// Парсер на slave
		{
			objMTSP.inputMessage[objMTSP.countData] = localByte;
			switch(objMTSP.countData)
			{
				case START_BYTE_OFFSET:
					if(localByte == objMTSP._startByte) objMTSP.countData++; 
					break;
				case SLAVE_ADDRESS_OFFSET:
					if(localByte == objMTSP._slaveAddress) objMTSP.countData++;
					else objMTSP.countData = 0;
					break;
				case COMAND_NUMBER_OFFSET:
					objMTSP.countData++;
					break;
				default:
					if(objMTSP.inputMessage[COMAND_NUMBER_OFFSET] == SENSOR_COMAND)
					{
						// SLAVE если пришла команда на SENSOR
					}
					else
					{
						if(objMTSP.countData < MTSP_MESSAGE_LENGTH - 1)
						{
							objMTSP.countData++;
							break;
						}
						if(objMTSP.inputMessage[CRC8_OFFSET] == crc8(objMTSP.inputMessage, MTSP_MESSAGE_LENGTH - 1))
						{
							byte localRightMessage[MTSP_MESSAGE_LENGTH - 2];
							for(int i = 0; i<MTSP_MESSAGE_LENGTH - 1;i++)
							{
								localRightMessage[i] = objMTSP.inputMessage[i+1];
							}
							onRightMessage(localRightMessage, FALSE);
							// Ответ slave, где вместо dataByte 0x00
							objMTSP.inputMessage[CRC8_OFFSET - 1] = 0x00;
							objMTSP.inputMessage[CRC8_OFFSET] = crc8(objMTSP.inputMessage, MTSP_MESSAGE_LENGTH - 1);
							Send((USART_TypeDef*) objMTSP.usartNumber, objMTSP.inputMessage, MTSP_MESSAGE_LENGTH);							
							objMTSP.countData = 0;
						}
						else
						{
							for(byte i = 0; i < MTSP_MESSAGE_LENGTH;i++)
							{
								if(objMTSP.inputMessage[i] == objMTSP._startByte)
								{
									objMTSP.countData = MTSP_MESSAGE_LENGTH - i;
									shiftLeft(i);
									return;
								}
							}
						}
					}
					break;
				
			}
		}
	
	}
}

  


void shiftLeft(byte amount)
{
	for(byte i=0, j=amount; i<MTSP_MESSAGE_LENGTH-amount; i++, j++)
  {
    objMTSP.inputMessage[i] = objMTSP.inputMessage[j];  
  }
}


void onRightMessage(byte *rightMessage, int isDataFromSensor)
{
	if(objMTSP._isMasterInited == TRUE)
	{
		if(isDataFromSensor == TRUE)
		{
			// если с датчиков инфа
			byte localBuf[MTSP_MESSAGE_LENGTH - 2] = {0x01, 0x02, 0x02};
			SendRequest(localBuf);
		}
		else
		{
		SendRequest(rightMessage);
		}
	}
	else
	{
		if(objMTSP._isSlaveInited == TRUE)
		{
		
		
		}
	}
	
}