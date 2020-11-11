#include <stdio.h>
#include "RN2483_Driver.h"
int main(void)
{
	//Inititalizing RN2483 module
	init("lora", 863000000, 200, true, 66000);
	char recv_buffer[256];
	receive(recv_buffer);
	return 0;
}
