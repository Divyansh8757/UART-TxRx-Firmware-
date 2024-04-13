#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

int main(){
// Creating serial port handle 
HANDLE serial_port = CreateFile("\\\\.\\COM4", GENERIC_READ | GENERIC_WRITE ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
FILE* file_pointer;					// File pointer to read through file
char tx_data;						// Character to read characters in the file and transmit them one by one
char rx_data[1010];					// Character buffer to receive data from MC
char start[1]={'1'};					// Start bit to start the communication between the MC and PC
char res[1];						// Response bit received from MC
int i=0;						// counter to keep count while receiving the data

DCB PortParas = {0};					// Declaring DCB structure named PortParas to set serila port parameters
PortParas.DCBlength = sizeof(PortParas);
	
if(GetCommState(serial_port, &PortParas)){		// Initializing serial Port parameters
PortParas.BaudRate = CBR_2400;
PortParas.ByteSize = 8;
PortParas.StopBits = ONESTOPBIT;
PortParas.Parity = NOPARITY;
}

else{
printf("Error getting Serial Port\n");
CloseHandle(serial_port);
return 1;
}

SetCommState(serial_port, &PortParas);				// Opening up the serial port

DWORD bytes_written;						// Variable to store the bytes transmitted to MC over the serial port
DWORD bytes_read;						// Variable to store the bytes received from MC over the serial port

WriteFile(serial_port, start, 1, &bytes_written, NULL);		// Sending start bit to initiate transmission 
printf("Transmission Request Sent\n");				

ReadFile(serial_port, &res[0], 1, &bytes_read, NULL);		// Reading response to start bit from MC
if(res[0]=='1'){
printf("Transmission starting\n");
	file_pointer = fopen("tx_file.txt","r");		// opening the file
	if(file_pointer == NULL){
	  printf("File not found\n");
	  return 1;
	  }
	while((tx_data = fgetc(file_pointer))!=EOF){		// reading each character of the file and transmitting it to MC
	  WriteFile(serial_port, &tx_data, sizeof(tx_data), &bytes_written, NULL);	
	  printf("%c", tx_data);
	  }
}
else{
	printf("No Acknowledgement Received");
	return 1;
}

printf("\nTransmission Complete\n");					// transmission complete message
	
WriteFile(serial_port, start, 1, &bytes_written, NULL);			// Transmission request from MC to PC
printf("Ready To Receive\n");		
ReadFile(serial_port, &res[0], 1, &bytes_read, NULL);
if(res[0] == '1'){							// Checking for start bit from MC side
	printf("Reception Starting\n");
	do{
	ReadFile(serial_port, &rx_data[i], 1, &bytes_read, NULL);	// start receiving the data from MC and print the received data 
	printf("%c", rx_data[i]);
	}while(rx_data[i]!='\0');					// receive data until NULL character 
}
else{
	printf("Reception Failed");
	return 1;
}
printf("\nReception Complete\n");					// Print reception complete message
	
fclose(file_pointer);
CloseHandle(serial_port);
return 0;
}

