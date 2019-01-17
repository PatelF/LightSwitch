#include "gpiolib_addr.h"
#include "gpiolib_reg.h"
#include "gpiolib_reg.c"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>


//This function allows us to turn on a light
void on(GPIO_Handle gpio, int pin){
	gpiolib_write_reg(gpio, GPSET(0), 1 << pin);
}

//This function allows us to turn off a light
void off (GPIO_Handle gpio, int pin){
	gpiolib_write_reg(gpio, GPCLR(0), 1 << pin);
}

//This function makes a light blink on and off
void blink(GPIO_Handle gpio, int pin){

	on(gpio, pin);

	usleep(1000000);

	off(gpio, pin);
	
	usleep(1000000);

}

int main(void){

	//Initialize the GPIO pins
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();

	if(gpio == NULL){
		perror("Could not initialize GPIO");
	}

	//Create a variable that contains exactly 32 bits
	uint32_t sel_reg = gpiolib_read_reg(gpio, GPFSEL(1));

	//Set pin 17 as an output pin using bitwise OR
	sel_reg |= 1  << 21;
	//Set pin 19 as an output pin using bitwise OR
	sel_reg |= 1  << 27;

	//Writing sel_reg into register 1
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg);

	//Infinite Loop
	while(1==1){

		int run = 0;
		int count = 0;

		int running = 1;

		//Keep the first light on until the button is pressed and released
		while(running){

			//Turn on first light
			on(gpio,17);

			//Assign value of level register on variable level_reg of 32 bit size
			uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));
			usleep(100);
			//Value bit 4 from level_reg assigned to pin_state
			int pin_state = level_reg & (1 << 4);

			//if button is unpressed
			if (pin_state){
				on(gpio,17);
			}

			else{
				//keep checking to see if button is released
				while(!pin_state){

					level_reg = gpiolib_read_reg(gpio, GPLEV(0));
					usleep(100);
					pin_state = level_reg & (1 << 4);
				}
				//Set running = 0 to break while loop
				running = 0;
			}
			

		}

		//Turn first light off
		off(gpio,17);

		//This for is the ten second loop where button presses are counted
		for(int i = 0; i < 10000; ++i){

			//Assign value of level register on variable level_reg of 32 bit size
			uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));

			//Value bit 4 from level_reg assigned to pin_state
			int pin_state = level_reg & (1 << 4);

			//If button was pressed and released
			if (run == 1 && pin_state){
				run = 0;
				count++;
			}
			//If button is pressed
			else if (!pin_state){
				run = 1;
			}
				
			//Make the program sleep
			usleep(900);
		}	

		//This for loop makes the second light blink
		for(int i = 0; i < count; i++){

			blink(gpio, 19);
		}
		//Wait five seconds
		usleep(5000000);
	}
}



