#include <stdio.h>
#include "RN2483_Driver.h"
int main(void)
{
	//Inititalizing RN2483 module
	init("lora", 863000000, 200, true, 66000);
	transmit("AB6587");
	return 0;
}