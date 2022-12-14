/*
This program demonstrate how to use hps communicate with FPGA through light AXI Bridge.
uses should program the FPGA by GHRD project before executing the program
refer to user manual chapter 7 for details about the demo
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )
#define TEST_PIO_BASE 0x0 // pinos INPUT do modulo criado no quartus para dar acesso a pinos da FPGA

const int GPIO_D0_MASK = 0b00000001;



int main() {

	void *virtual_base;
	int fd;
	//int loop_count;
	//int led_direction;
	//int led_mask;
	void *h2p_lw_led_addr;

	// map the address space for the LED registers into user space so we can interact with them.
	// we'll actually map in the entire CSR span of the HPS since we want to access various registers within that span

	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}

	virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );

	if( virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return( 1 );
	}


	// endereco de memoria dos pinos 
	// virtual base é o endereço da memoria do HPS no linux, ALT_LWFPGASLVS_OFST é o endereço da FPGA no HPS, TEST_PIO_BASE é o endereço dos PIOS (test_pio criado no quartus) na FPGA e HW_REGS_MASK é a mascara da memoria da FPGA
	h2p_lw_led_addr = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + TEST_PIO_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	printf("Started ...\n");
	int pin_0;


//--------------------------- checks if the pin has been activated ---------------------------------
	while(1) {
		// printf("%x \n", 	(*(uint8_t *)h2p_lw_led_addr & GPIO_D0_MASK));  // printa o valor do primeiro pino (registrador dos 8 pinos com uma mascara)
		pin_0 = *(uint8_t *)h2p_lw_led_addr & GPIO_D0_MASK;
		if (pin_0 == 0){
			//printf("pin low");
			system("echo 'approved' > result.txt");
			system("ls");
			usleep( 4000000);
			system("rm result.txt");
			system("ls");
		}
	}


	// ----------------------------- toggle the LEDs a bit ----------------------------------------

	// loop_count = 0;
	// led_mask = 0x01;
	// led_direction = 0; // 0: left to right direction
	// while( loop_count < 60 ) {
		
	// 	// control led
	// 	*(uint32_t *)h2p_lw_led_addr = ~led_mask; 

	// 	// wait 100ms
	// 	usleep( 100*1000 );
		
	// 	// update led mask
	// 	if (led_direction == 0){
	// 		led_mask <<= 1;
	// 		if (led_mask == (0x01 << (LED_PIO_DATA_WIDTH-1)))
	// 			 led_direction = 1;
	// 	}else{
	// 		led_mask >>= 1;
	// 		if (led_mask == 0x01){ 
	// 			led_direction = 0;
	// 			loop_count++;
	// 		}
	// 	}
		
	// } // while

	//------------------------------------------------------------------------------------------



	// clean up our memory mapping and exit
	
	if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	}

	close( fd );

	return( 0 );
}


