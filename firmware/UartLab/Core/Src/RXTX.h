/*
 * RXTX.h
 *
 *  Created on: Mar 2, 2025
 *      Author: tissa
 */

#ifndef SRC_RXTX_H_
#define SRC_RXTX_H_

// Функция для обработки приема данных
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//    char _receivedData;
	if (huart == &huart2){
		HAL_UART_Receive_IT(&huart2, (uint8_t*)&receivedData, 1); // Принимаем 1 байт
		ProcessReceivedData(receivedData); // Обрабатываем полученные данные
	}
}

#endif /* SRC_RXTX_H_ */
