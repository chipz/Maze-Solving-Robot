/*
 * 3pi-linefollower - demo code for the Pololu 3pi Robot
 * 
 * This code will follow a black line on a white background, using a
 * very simple algorithm.  It demonstrates auto-calibration and use of
 * the 3pi IR sensors, motor control, bar graphs using custom
 * characters, and music playback, making it a good starting point for
 * developing your own more competitive line follower.
 *
 * http://www.pololu.com/docs/0J21
 * http://www.pololu.com
 * http://forum.pololu.com
 *
 */

// The 3pi include file must be at the beginning of any program that
// uses the Pololu AVR library and 3pi.
#include <pololu/3pi.h>

// This include file allows data to be stored in program space.  The
// ATmegaxx8 has 16x more program space than RAM, so large
// pieces of static data should be stored in program space.
#include <avr/pgmspace.h>

// Introductory messages.  The "PROGMEM" identifier causes the data to
// go into program space.
const char welcome_line1[] PROGMEM = " Custom";
const char welcome_line2[] PROGMEM = "3\xf7 Robot";
const char demo_name_line1[] PROGMEM = "Line";
const char demo_name_line2[] PROGMEM = "follower";

// A couple of simple tunes, stored in program space.
const char welcome[] PROGMEM = ">g32>>c32";
const char go[] PROGMEM = "L16 cdegreg4";

// Data for generating the characters used in load_custom_characters
// and display_readings.  By reading levels[] starting at various
// offsets, we can generate all of the 7 extra characters needed for a
// bargraph.  This is also stored in program space.
const char levels[] PROGMEM = {
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111
};

unsigned int counterjalur=0;
unsigned int jumlahcounter=0;
char arahprint[255];
const int max = 80; // the maximum speed

// This function loads custom characters into the LCD.  Up to 8
// characters can be loaded; we use them for 7 levels of a bar graph.
void load_custom_characters()
{
	lcd_load_custom_character(levels+0,0); // no offset, e.g. one bar
	lcd_load_custom_character(levels+1,1); // two bars
	lcd_load_custom_character(levels+2,2); // etc...
	lcd_load_custom_character(levels+3,3);
	lcd_load_custom_character(levels+4,4);
	lcd_load_custom_character(levels+5,5);
	lcd_load_custom_character(levels+6,6);
	clear(); // the LCD must be cleared for the characters to take effect
}

// This function displays the sensor readings using a bar graph.
void display_readings(const unsigned int *calibrated_values)
{
	unsigned char i;

	for(i=0;i<5;i++) {
		// Initialize the array of characters that we will use for the
		// graph.  Using the space, an extra copy of the one-bar
		// character, and character 255 (a full black box), we get 10
		// characters in the array.
		const char display_characters[10] = {' ',0,0,1,2,3,4,5,6,255};

		// The variable c will have values from 0 to 9, since
		// calibrated values are in the range of 0 to 1000, and
		// 1000/101 is 9 with integer math.
		char c = display_characters[calibrated_values[i]/101];

		// Display the bar graph character.
		print_character(c);
	}
}

// Initializes the 3pi, displays a welcome message, calibrates, and
// plays the initial music.
void initialize()
{
	unsigned int counter; // used as a simple timer
	unsigned int sensors[5]; // an array to hold sensor values

	// This must be called at the beginning of 3pi code, to set up the
	// sensors.  We use a value of 2000 for the timeout, which
	// corresponds to 2000*0.4 us = 0.8 ms on our 20 MHz processor.
	pololu_3pi_init(2000);
	load_custom_characters(); // load the custom characters
	
	// Play welcome music and display a message
	print_from_program_space(welcome_line1);
	lcd_goto_xy(0,1);
	print_from_program_space(welcome_line2);
	play_from_program_space(welcome);
	delay_ms(1000);

	clear();
	print_from_program_space(demo_name_line1);
	lcd_goto_xy(0,1);
	print_from_program_space(demo_name_line2);
	delay_ms(1000);

	// Display battery voltage and wait for button press
	while(!button_is_pressed(BUTTON_B))
	{
		int bat = read_battery_millivolts();

		clear();
		print_long(bat);
		print("mV");
		lcd_goto_xy(0,1);
		print("Press B");

		delay_ms(100);
	}

	// Always wait for the button to be released so that 3pi doesn't
	// start moving until your hand is away from it.
	wait_for_button_release(BUTTON_B);
	delay_ms(1000);

	// Auto-calibration: turn right and left while calibrating the
	// sensors.
	for(counter=0;counter<80;counter++)
	{
		if(counter < 20 || counter >= 60)
			set_motors(40,-40);
		else
			set_motors(-40,40);

		// This function records a set of sensor readings and keeps
		// track of the minimum and maximum values encountered.  The
		// IR_EMITTERS_ON argument means that the IR LEDs will be
		// turned on during the reading, which is usually what you
		// want.
		calibrate_line_sensors(IR_EMITTERS_ON);

		// Since our counter runs to 80, the total delay will be
		// 80*20 = 1600 ms.
		delay_ms(20);
	}
	set_motors(0,0);

	// Display calibrated values as a bar graph.
	while(!button_is_pressed(BUTTON_B))
	{
		// Read the sensor values and get the position measurement.
		unsigned int position = read_line(sensors,IR_EMITTERS_ON);

		// Display the position measurement, which will go from 0
		// (when the leftmost sensor is over the line) to 4000 (when
		// the rightmost sensor is over the line) on the 3pi, along
		// with a bar graph of the sensor readings.  This allows you
		// to make sure the robot is ready to go.
		clear();
		print_long(position);
		lcd_goto_xy(0,1);
		display_readings(sensors);

		delay_ms(100);
	}
	wait_for_button_release(BUTTON_B);

	clear();

	print("Go!");		

	// Play music and wait for it to finish before we start driving.
	play_from_program_space(go);
	while(is_playing());
} // akhir inisialisasi

//cari arah
char cariarah(int kiri,int kanan,int lurus)
{
	if(kanan) { return 'R'; }
	else if(lurus) { return 'S'; }
	else if(kiri) { return 'L'; }
	else { return 'B'; }
}

void belok(char arah)
{
	//unsigned int sensors[5];
	unsigned int delay = 200;
	//unsigned int position = read_line(sensors,IR_EMITTERS_ON);
	//read_line(sensors,IR_EMITTERS_ON);
	if(arah == 'L') {
		//Belok kiri
		set_motors(-max,max);
		delay_ms(delay);
	} else if(arah == 'R') {
		// Belok kanan
		set_motors(max,-max);
		delay_ms(delay);
	} else if(arah == 'S') {
		//lurus aja
	} else if(arah == 'B') {
		//mbalik
		set_motors(max,-max);
		delay_ms(2*delay);
	}
}

void susur() {
	int last_proportional = 0;
	long integral=0;

	while(1)
	{
		// Normally, we will be following a line.  The code below is
		// similar to the 3pi-linefollower-pid example, but the maximum
		// speed is turned down to 60 for reliability.

		// Get the position of the line.
		unsigned int sensors[5];
		unsigned int position = read_line(sensors,IR_EMITTERS_ON);

		// The "proportional" term should be 0 when we are on the line.
		int proportional = ((int)position) - 2000;

		// Compute the derivative (change) and integral (sum) of the
		// position.
		int derivative = proportional - last_proportional;
		integral += proportional;

		// Remember the last position.
		last_proportional = proportional;

		// Compute the difference between the two motor power settings,
		// m1 - m2.  If this is a positive number the robot will turn
		// to the left.  If it is a negative number, the robot will
		// turn to the right, and the magnitude of the number determines
		// the sharpness of the turn.
		int power_difference = proportional/20 + integral/10000 + derivative*3/2;

		// Compute the actual motor settings.  We never set either motor
		// to a negative value.
		if(power_difference > max)
			power_difference = max;
		if(power_difference < -max)
			power_difference = -max;
		
		if(power_difference < 0)
			set_motors(max+power_difference,max);
		else
			set_motors(max,max-power_difference);

		// We use the inner three sensors (1, 2, and 3) for
		// determining whether there is a line straight ahead, and the
		// sensors 0 and 4 for detecting lines going to the left and
		// right.
		
		if(sensors[1] < 100 && sensors[2] < 100 && sensors[3] < 100)
		{
			// There is no line visible ahead, and we didn't see any
			// intersection.  Must be a dead end.
			return;
		}
		else if(sensors[0] > 200 || sensors[4] > 200)
		{
			// Found an intersection.
			return;
		}
	}
}

void sederhanakan()
{
	switch(arahprint[counterjalur-2])
		{
		case 'L':
			switch(arahprint[counterjalur])
				{
				case 'L':
					arahprint[counterjalur] = '\0';
					arahprint[counterjalur-1] = '\0';
					counterjalur-=2;
					arahprint[counterjalur] = 'S';
					break;
				case 'R':
					arahprint[counterjalur] = '\0';
					arahprint[counterjalur-1] = '\0';
					counterjalur-=2;
					arahprint[counterjalur] = 'B';
					break;
				case 'S':
					arahprint[counterjalur] = '\0';
					arahprint[counterjalur-1] = '\0';
					counterjalur-=2;
					arahprint[counterjalur] = 'R';
					break;
				}			
			break;
		case 'R':
			switch(arahprint[counterjalur])
				{
				case 'L':
					arahprint[counterjalur] = '\0';
					arahprint[counterjalur-1] = '\0';
					counterjalur-=2;
					arahprint[counterjalur] = 'B';
					break;
				case 'R':
					arahprint[counterjalur] = '\0';
					arahprint[counterjalur-1] = '\0';
					counterjalur-=2;
					arahprint[counterjalur] = 'S';
					break;
				case 'S':
					arahprint[counterjalur] = '\0';
					arahprint[counterjalur-1] = '\0';
					counterjalur-=2;
					arahprint[counterjalur] = 'L';
					break;
				}
			break;
		case 'S':
			switch(arahprint[counterjalur])
				{
				case 'L':
					arahprint[counterjalur] = '\0';
					arahprint[counterjalur-1] = '\0';
					counterjalur-=2;
					arahprint[counterjalur] = 'R';
					break;
				case 'R':
					arahprint[counterjalur] = '\0';
					arahprint[counterjalur-1] = '\0';
					counterjalur-=2;
					arahprint[counterjalur] = 'L';
					break;
				case 'S':
					arahprint[counterjalur] = '\0';
					arahprint[counterjalur-1] = '\0';
					counterjalur-=2;
					arahprint[counterjalur] = 'B';
					break;
				}
			break;
		}

}

char jalurpulang(char arahprint[255]) {
	for(int i=0; arahprint[i] != 0; i++) {
		switch(arahprint[i]) {
			case 'L' :
				arahprint[i] = 'R';
				break;
			case 'R' :
				arahprint[i] = 'L';
				break;
		}
	}
	return arahprint[255];
}


// This is the main function, where the code starts.  All C programs
// must have a main() function defined somewhere.
int main()
{
	// set up the 3pi
	initialize();

	while(1) {
		unsigned int sensors[5];
		unsigned int kiri=0;
		unsigned int kanan=0;
		unsigned int lurus=0;
		left_led(0);
		right_led(0);

		susur();


		set_motors(50,50);
		delay_ms(50);
		read_line(sensors,IR_EMITTERS_ON);
		
		if(sensors[0] > 100)
		{
			// Belok kiri.
			kiri=1;
			left_led(1);
		}
		if(sensors[4] > 100)
		{
			//Belok kanan
			kanan=1;
			right_led(1);
		}
		
		//maju sithik
		set_motors(40,40);
		delay_ms(200);
		read_line(sensors,IR_EMITTERS_ON);

		if(sensors[0] > 100 || sensors[2] > 100 || sensors[4] > 100)
		{
			lurus=1;
		}

		if(sensors[1] > 600 && sensors[2] > 600 && sensors[3] > 600)
			break;


		unsigned char arah = cariarah(kiri,kanan,lurus);

		clear();
		arahprint[counterjalur] = arah;
		arahprint[counterjalur+1] = 0;
		print(arahprint);
		if(counterjalur > 8)
			{
				lcd_goto_xy(0,1);
				print(arahprint+8);
			}

		if(arahprint[counterjalur-1] == 'B') {
			sederhanakan();
		}
		
		counterjalur++;

		jumlahcounter = counterjalur;

		belok(arah);
	}


	counterjalur = 0;

	

	while(1) {
		// Beep to show that we finished the maze.
		set_motors(0,0);
		play(">>a32");

		// Wait for the user to press a button, while displaying
		// the solution.
		while(!button_is_pressed(BUTTON_B))
		{
			if(get_ms() % 2000 < 1000)
			{
				clear();
				print("Jadi!");
				lcd_goto_xy(0,1);
				print("Press B");
			}
			else
			clear();
			arahprint[jumlahcounter+1] = 0;
			print(arahprint);
			if(jumlahcounter > 8)
			{
				lcd_goto_xy(0,1);
				print(arahprint+8);
			}
			delay_ms(30);
		}
		while(button_is_pressed(BUTTON_B));
	
		delay_ms(1000);
		for(int i=0;i<jumlahcounter;i++){
			susur();
			set_motors(50,50);
			delay_ms(50);
			set_motors(40,40);
			delay_ms(200);
			belok(arahprint[i]);
		}
		susur();
		
	}
}
