#include "main.h"
#include<stdio.h>

TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;

void SystemClock_Config(void);
static void SystemPower_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_USB_DRD_FS_HCD_Init(void);
static void MX_UART4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM4_Init(void);
void Send_Response();
void Receive_Data();
void Start_Transmission();
void Receive_TxRequest();

uint8_t rxData[1010];	    	// Data Receive Buffer
uint8_t rxReq[1];			      // Data Transmission request from PC
uint8_t txReq[1];		      	// Data Reception request from PC
uint8_t res[1]={'1'};		    // Positive response from MC to PC on request
int i=0;					          // counter for data reception buffer
int j=0;					          // counter for data transmission buffer
int size=0;					        // To check size of data received and transmitted
int rate=0;					        // to calculate rate of data reception and transmission
char Rate[36];				      // Character buffer to print reception and transmission rate

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  SystemPower_Config();
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_USB_DRD_FS_HCD_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();

// main code starts here

HAL_TIM_Base_Start(&htim4);

 while (1)
  {
	  i=0;
	  j=0;
	  HAL_UART_Receive(&huart1, rxReq, 1,HAL_MAX_DELAY);  // Wait for transmission request from PC
	  if(rxReq[0]=='1'){					 // If request received
	  	Send_Response();								                  // Send Acknowledgement response
	  	Receive_Data();								                	  // Function to receive data from PC to MC
	  } 
	  Start_Transmission();								                // Funtion To transmit data from MC to PC
  }
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart){				// To stop polling while printing transfer rate
}


void Send_Response(){									// Function to send response
	HAL_UART_Transmit(&huart1, res, 1, 1);
}

void Receive_Data(){									// Function to Receive data
	__HAL_TIM_SET_COUNTER(&htim4,0);						// Timer to calculate data rate
	do{
		size++;
		HAL_UART_Receive(&huart1, rxData+i++, 1,HAL_MAX_DELAY);
		if(__HAL_TIM_GET_COUNTER(&htim4)>=1000000){
			rate = size;
			__HAL_TIM_SET_COUNTER(&htim4,0);
			sprintf(Rate, "Reception Rate : %d Bytes/sec\r\n", rate);
			HAL_UART_Transmit_IT(&huart4, (uint8_t*)Rate, sizeof(Rate));	// Print data transfer rate every second
			size=0;
		}
	}while(i<1009);
}

void Start_Transmission(){							// Function to Transmit data
	__HAL_TIM_SET_COUNTER(&htim4,0);					// Timer to calculate rate
	 HAL_UART_Receive(&huart1, txReq, 1,HAL_MAX_DELAY);			// receive request from PC
	 if(txReq[0]=='1'){							// check for receive request from PC
		Send_Response();						// Send response that Transmission is starting from MC to PC
	size=0;
	while(j<=i){
		size++;
		HAL_UART_Transmit(&huart1, rxData+j++, 1,HAL_MAX_DELAY);	// Transmitting data from MC to PC
		if(__HAL_TIM_GET_COUNTER(&htim4)>=1000000){
			rate = size;
			__HAL_TIM_SET_COUNTER(&htim4,0);
			sprintf(Rate, "Transmission Rate : %d Bytes/sec\r\n", rate);
			HAL_UART_Transmit_IT(&huart4, (uint8_t*)Rate, sizeof(Rate));
			size=0;
			}
		}
	 }
}
