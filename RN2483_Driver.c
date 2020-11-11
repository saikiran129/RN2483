#include <stdio.h>
#include <string.h>
#include "uc_uart.h"
/*header file corresponding to the UART protocol, for given microcontroller.
  Microcontroller SDK is assumed to contain uart_send() and uart_receive() commands
*/

char temp_data[300] = "";
void set_modulation_mode(char* modulation_mode)
{
	// Setting modulation mode - To do this, the command must be sent from microcontroller to RN2483 over UART
	strcpy(temp_data, "radio set mode ");
	strcat(temp_data, modulation_mode);

	//sending the command to RN2483 via UART
	uart_send(temp_data);

	//checking the status of the command
	if(!strcmp(uart_receive(), "ok"))
	{
		/*Since printf is a costly operation, many microcontrollers have custom, lightweight version of printf.The code
		  assumes that uc_print() is provided in SDK of microcontroller
		*/
		uc_print("Modulation mode: %s set", modulation_mode);
	}
	else
	{
		uc_print("Invalid modulation mode");
	}
	memset(temp_data, 0, 300); 
}

void set_frequency(unsigned long int frequency)
{
	// creating a string consisting of command to set frequency
	strcpy(temp_data, "radio set freq ");
	strcat(temp_data, frequency);

	//sending the command to RN2483 via UART
	uart_send(temp_data);

	//checking the status of the command
	if(!strcmp(uart_receive(), "ok"))
	{
		uc_print("Frequency set: %lu \n", frequency);
	}
	else
	{
		uc_print("Invalid frequency");
	}
	memset(temp_data, 0, 300); 
}

void set_signal_bandwidth(unsigned float signal_bandwidth)
{
	//creating a string consisting of command to set signal bandwidth
	strcpy(temp_data, "radio set rxbw ");
	strcat(temp_data, signal_bandwidth);

	//sending the command to RN2483 via UART
	uart_send(temp_data);

	//checking the status of the command
	if(!strcmp(uart_receive(), "ok"))
	{
		uc_print("Bandwidth set: %lu \n", frequency);
	}
	else
	{
		uc_print("Invalid bandwidth value");
	}
	memset(temp_data, 0, 300);
}

void set_crc(bool crcON)
{
	//creating a string consisting of command to set CRC
	strcpy(temp_data, "radio set crc ");
	if(crcON)
	{
		strcat(temp_data, "on");
	}
	else
	{
		strcat(temp_data, "off");
	}

	//sending command to RN2483 via UART
	uart_send(temp_data); 

	//checking the status of the command
	if((!strcmp(uart_receive(), "ok")) && crcON)
	{
		uc_print("CRC on");
	}
	else
	{
		uc_print("CRC off");
	}
	memset(temp_data, 0, 300);
}

void set_watchdog_timer(unsigned long int watchdog_timer)
{
	if(watchdog_timer > 65535)
	{ 
		/*watchdog timer must be greater than receive window size
		  The maximum value of window size is 65535
		  So watchdog timer is value is set to be greater than 65535
		*/
		strcpy(temp_data, "radio set wdt ");
		strcat(temp_data, watchdog_timer);
	}
	else
	{
		strcpy(temp_data, "radio set wdt ");
		strcat(temp_data, 65535);
	}

	//sending command to RN2483 via UART
	uart_send(temp_data);

	uc_print("Watchdog timer set");
	memset(temp_data, 0, 300);
}

void init(char* modulation_mode, unsigned long int frequency, unsigned float signal_bandwidth, bool crcON, unsigned long int watchdog_timer)
{
	//Initializing RN2483 module
	memset(temp_data, 0, 300); 
	set_modulation_mode(modulation_mode);
	set_frequency(frequency);
	set_signal_bandwidth(signal_bandwidth);
	set_crc(crcON);
	set_watchdog_timer(watchdog_timer);
}

void transmit(char* transmit_data_in_hex)
{
	//As mentioned in the User Manual, mac pause command must be called before doing any transmission
	uart_send("mac pause");
	if(!strcmp(uart_receive(),'0'))
	{
		uc_print("Error while transmitting: Unable to pause MAC");
	}

	//creating a string consisting command to transmit data over WiFi  
	strcpy(temp, "radio tx ");
	strcat(temp, transmit_data_in_hex); 

	//sending transmit command to RN2483 via UART
	uart_send(temp);

	/*RN2483 responds with 2 values after it receives "transmit command" from microcontroller
	  The first parameter tells if parameters specified in the command are valid
	  The second command tells if data were received successfully
	*/
	char parameter_status[20] = uart_receive();
	char transmit_status[20] = uart_receive();

	//checking the validity of transmit parameters
	if(!strcmp(parameter_status, "invalid_param"))
	{
		uc_print("Invalid transmit parameter");
	}
	if(!strcmp(parameter_status, "busy"))
	{
		uc_print("Transceiver is busy");
	}

	//checking if transmission is successful
	if(!strcmp(transmit_status, "radio_tx_ok"))
	{
		uc_print("Transmitted successfully");
	}
	if(!strcmp(transmit_status, "radio_err"))
	{
		uc_print("Error while transmitting");
	}

	//Resuming mac
	uart_send("mac resume");
	if(!strcmp(uart_receive(),'ok'))
	{
		uc_print("MAC resumed successfully");
	}
	else
	{
		uc_print("error while resuming MAC");
	}
	memset(temp, 0, 300);
}

void receive(unsigned int recv_window_size, char* receive_buffer)
{
	//pausing MAC
	uart_send("mac pause");
	if(!strcmp(uart_receive(),'0'))
	{
		uc_print("Error while receiving: Unable to pause MAC");
	}
	//converting unsigned int to string
	char recv_window[20];
	sprintf(recv_window, "%hu", recv_window_size);

	//creating a string consisting command to receive data over WiFi
	strcpy(temp, "radio rx ");
	strcat(temp, recv_window);

	//sending the command to RN2843 via UART
	uart_send(temp);

	/*RN2483 responds with 2 values after it receives "receive command" from microcontroller
	  The first parameter tells if parameters specified in the command are valid
	  The second command tells if data were received successfully
	*/
	char recv_parameter_status[20] = uart_receive(); 
	char received_data[20] = uart_receive();

	//checking the validity of transmit parameters
	if(!strcmp(recv_parameter_status, "invalid_param"))
	{
		uc_print("Invalid receive parameter ");
	}
	if(!strcmp(recv_parameter_status, "busy"))
	{
		uc_print("Transceiver is busy");
	}

	//checking if reception is successful
	if(!strcmp(received_data, "radio_err"))
	{
		uc_print("Failed to receive data");
	}
	else
	{
		uc_print("Data received successfully");
	}

	//Putting received data in specified memory location i.e, at received_data
	strcpy(receive_buffer, received_data + 8);
	/*received_data contains "radio_rx <actual data>"
	  to remove "radio_rx ", 8 is added as an offest
	*/

	//Resuming mac
	uart_send("mac resume");
	if(!strcmp(uart_receive(),'ok'))
	{
		uc_print("MAC resumed successfully");
	}
	else
	{
		uc_print("error while resuming MAC");
	}
}