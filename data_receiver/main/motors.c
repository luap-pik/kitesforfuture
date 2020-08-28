//You can get these value from the datasheet of servo you use, in general pulse width varies between 1000 to 2000 microsecond
#define SERVO_MIN_PULSEWIDTH 1000 //Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH 2000 //Maximum pulse width in microsecond
#define SERVO_MAX_DEGREE 90 //Maximum angle in degree upto which servo can rotate

static uint32_t degree2pulsewidth(uint32_t degree_of_rotation)
{
    uint32_t cal_pulsewidth = 0;
    cal_pulsewidth = (SERVO_MIN_PULSEWIDTH + (((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * (degree_of_rotation)) / (SERVO_MAX_DEGREE)));
    return cal_pulsewidth;
}



void initMotors(){
	//Set GPIO 18 as PWM0A, to which the servo is connected
	//MCPWM_UNIT_0 and MCPWM_UNIT_1 are the two pwm units, each capable of controlling 3 motors
	//MCPWM0A, MCPWM0B, MCPWM1A, MCPWM1B, MCPWM2A, MCPWM2B are the possible outputs
	//in each MCPWM_UNIT there are 3 operators (0,1,2 number is associated with timer) each containing 2 generators (A and B)
	
	//SO TOTAL NUMBER OF simple one input signal MOTORS PER UNIT IS 6 and TOTAL NUMBER OF MOTORS PER ESP32 is thus 12
	
	//SYNC_X (for timer), FAULT_X, CAP_X ("CAP"=capture), X=0,1,2 are the possible inputs for the three timers 0,1,2
	//18 is the pin number written on the board. can be any gpio-pin.
	
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, 12);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, 13);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, 27);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0B, 26);
    /*mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, 16);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, 17);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, 5);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0B, 12);*/
    
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50;
    pwm_config.cmpr_a = 0;
    pwm_config.cmpr_b = 0;
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config);
}

void setSpeed(int motor, uint32_t degree){
	mcpwm_unit_t unit = MCPWM_UNIT_0;
	mcpwm_generator_t gen = MCPWM_OPR_A;
	
	switch(motor) {
		case 0: break;
		case 1: gen = MCPWM_OPR_B; break;
		case 2: unit = MCPWM_UNIT_1; break;
		case 3: unit = MCPWM_UNIT_1; gen = MCPWM_OPR_B; break;
		default: break;
	}
	//MCPWM_TIMER_0 automatically sets the output to MCPWM0X?
	mcpwm_set_duty_in_us(unit, MCPWM_TIMER_0, gen, degree2pulsewidth(degree));
	//mcpwm_set_duty() to set duty in % instead of microseconds
}
