#include <math.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_now.h"

#include "driver/mcpwm.h"
#include "esp_adc_cal.h"

#include "driver/gpio.h"
#include "driver/i2c.h"

// some helper functions for mathematical operations
#include "mymath.c"

#include "timer.c"
#include "constants.c"

// read data from analog sensors, e.g. battery state
#include "analog_sensors.c"
// servo motors and propeller motor control
#include "motors.c"
// i2c protocol for communication with mpu6050 and bmp280
#include "interchip.c"
// gyroscope and accelerometer
#include "mpu6050.c"
// barometer
#include "bmp280.c"
// estimate current height from pressure and acceleration
#include "heightSensorFusion.c"

#include "../../COMMON_FILES/RC.c"

// determine wind direction
#include "windDirection.c"
// determine distance from ground station
#include "lineLength.c"
// angle towards wind direction in the plane parallel to ground
#include "sidewaysAngle.c"
// determine the kite's speed
#include "kiteSpeed.c"
// read battery state of charge from analog sensor
#include "batteryPercentage.c"

// rudder, elevator and motor speed control
#include "pid.c"

//int counter = 0;

void init(){
	initUptime();
	initSensors();
	setRole(KITE);
	network_setup();
	initMotors();
	
	setAngle(TOP_RIGHT, 0);
	setSpeed(BOTTOM_LEFT, 0);
	setSpeed(BOTTOM_RIGHT, 0);
	setAngle(TOP_LEFT, 0);
	
	initMPU6050();
	initHeightSensorFusion();
	setNumberOfOmittedSends(0); // debugging info sent to pc every x iterations
	
}

void update(){
	readMPURawData();
	processMPURawData();
	fuseHeightSensorData();
	updateBatteryPercentage();
	updateWindDirection();
	updateLineLength();
	updateSidewaysAngle();
	updateKiteSpeed();
}

void app_main(void){
	init();
	
	setGoalHeight(10);
	setRateOfClimb(0.5);
	
	int landing = false;
	
	
	while(1){
		
		update();
		
		// trim for balancing is between -0.3 and 0.3 (measured in radians with assumption sin(x)=x near 0)
		setZAxisTrim(0.);
		setAUXTrim(0.);
		setElevatorTrim(0.);

		// set control gains from input signal
		Py = pow(10., 2.*((float)receivedSignal[0])/(3003.0));
		Dy = pow(10., 2.*((float)receivedSignal[1])/(3003.0));
		Dz = pow(10., 2.*((float)receivedSignal[2])/(3003.0));
		Pz = pow(10., 2.*((float)receivedSignal[3])/(3003.0));


		
		if((float)receivedSignal[5] > -100){ // third knob from the right turns on manual kite fly mode, which ignores orientation
			flightMode = GLIDE_MODE;
		}else{
			flightMode = HOVER_MODE;
		}
		
		// START HOVER LAND AT 10% battery or signal loss
		
		if(landing == false){
			setGoalHeight(-5.0 - 10.0*((float)receivedSignal[5])/(3003.0));
			setYAxisTrim(0.);
			
			//if(queryTimer(t) > 35){
			
			if(timeSinceLastReceiveInSeconds() > 3.0 || (getBatteryPercentage() < 0.10 && getUptime() > 10)){
				setGoalHeight(-5);
				setYAxisTrim(0.0);
				landing = true;
			}
			
		}
		
		calculatePID();
	    
	    
	    
	    
	    /*
	    if(counter == 10){
	    	//printf("angle = %f.\n", angle);
	    	
			printf("accel_x = %f, %f.\n", mpu_pos_avg.accel_x, mpu_pos.accel_x);
			printf("accel_y = %f, %f.\n", mpu_pos_avg.accel_y, mpu_pos.accel_y);
			printf("accel_z = %f, %f.\n", mpu_pos_avg.accel_z, mpu_pos.accel_z);
			printf("gyro_x = %f.\n", mpu_pos.gyro_x-mpu_pos_avg.gyro_x);
			printf("gyro_y = %f.\n", mpu_pos.gyro_y-mpu_pos_avg.gyro_y);
			printf("gyro_z = %f.\n", mpu_pos.gyro_z-mpu_pos_avg.gyro_z);
			printf("rot= %f\t%f\t%f\t\n     %f\t%f\t%f\t\n     %f\t%f\t%f\t", rot[0], rot[1], rot[2], rot[3], rot[4], rot[5], rot[6], rot[7], rot[8]);
			
			printf("free memory = %d.\n", xPortGetFreeHeapSize());
	    	counter = 0;
	    }counter++;
	    */
	    vTaskDelay(1.0);
	    
    }
    
}