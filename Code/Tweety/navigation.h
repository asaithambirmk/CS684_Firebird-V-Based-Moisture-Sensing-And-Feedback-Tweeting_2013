#include <math.h> //included to support power function
//#include "common.h"
//volatile static int currentCheckPoint=0;
volatile unsigned char flag = 0;
volatile unsigned char Left_white_line = 0;
volatile unsigned char Center_white_line = 0;
volatile unsigned char Right_white_line = 0;

int normalSpeed=200;
int lowSpeed=140;

unsigned int botLength=75;

int isFollowLine(unsigned char line)
{
	if(line<0x28)
		return 0;       // return 1 for BWB, 0 otherwise
	else
		return 1;		// complement of above value
}

void stopOnFollowLine()
{
	while(1)
	{
		Center_white_line = ADC_Conversion(2);
		if(isFollowLine(Center_white_line))
		{
			stop();
			break;
		}
	}
}

void right_upto_follow_line()
{
	velocity(200,200);
	right();
	stopOnFollowLine();
}

void left_upto_follow_line()
{
	velocity(200,200);
	left();
	stopOnFollowLine();
}

//Function used for moving robot forward by specified distance following follow line
void linear_distance_follow_mm(unsigned int DistanceInMM)
{
	 float ReqdShaftCount = 0;
 	 unsigned long int ReqdShaftCountInt = 0;

	 ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
	 ReqdShaftCountInt = (unsigned long int) ReqdShaftCount;
	 ShaftCountRight = 0;
	 while(1)
	 {
	 		Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
			Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
			Right_white_line = ADC_Conversion(1);	//Getting data of Right WL Senso

	 		// bot has slipped left
	 		if(!isFollowLine(Left_white_line) /*&& !isFollowLine(Center_white_line)*/ && isFollowLine(Right_white_line))//flag=0 => -b- here(b-b-0)
			{
				forward();
				velocity(normalSpeed,lowSpeed);
			}
			// bot has slipped right
			else if(isFollowLine(Left_white_line)/* && !isFollowLine(Center_white_line)*/ && !isFollowLine(Right_white_line))//flag=0 => -b- here(0-b-b)
			{
				forward();
				velocity(lowSpeed,normalSpeed);
			}
			else
			{
				forward();
				velocity(normalSpeed,normalSpeed);
			}
		  if(ShaftCountRight > ReqdShaftCountInt)
		  {
		  	break;
		  }
	 } 
	 stop();
}

// forward "DistanceInMM" distance following white(or black) line
void forward_follow_mm(unsigned int DistanceInMM)
{
	stop();
 	forward();
	velocity(normalSpeed,normalSpeed);
 	linear_distance_follow_mm(DistanceInMM);
}

// go to previous checkpoint in backward direction
void goToPreviousPoint()
{
	stop();
	while(1)
	{
		Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
		Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
		Right_white_line = ADC_Conversion(1);	//Getting data of Right WL Senso

		// bot has slipped left
		if(/*!isFollowLine(Left_white_line)) && !isFollowLine(Center_white_line) && */isFollowLine(Right_white_line))//flag=0 => -b- here(b-b-0)
		{
			back();
			velocity(lowSpeed,normalSpeed);
		}
		// bot has slipped right
		else if(isFollowLine(Left_white_line))///* && !isFollowLine(Center_white_line)*/ && !isFollowLine(Right_white_line))//flag=0 => -b- here(0-b-b)
		{
			back();
			velocity(normalSpeed,lowSpeed);
		}
		// center is on follow line
		else if(isFollowLine(Center_white_line)  && !isFollowLine(Left_white_line) && !isFollowLine(Right_white_line))//center(just start when 0-w-0)
		{
			back();
			velocity(normalSpeed,normalSpeed);
		}
		// checkpoint found
		else if(isFollowLine(Center_white_line) && isFollowLine(Left_white_line) && isFollowLine(Right_white_line)) //checkpoint-(w-w-w)
		{
			stop();
			break;
		}
	}
}

// go to next checkpoint 1->2, 2->3,...
void goToNextPoint()
{
	stop();
	forward();
	while(1)
	{

		Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
		Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
		Right_white_line = ADC_Conversion(1);	//Getting data of Right WL Sensor

//		lcd_clear();
		print_sensor(1,1,3);	//Prints value of White Line Sensor1
		print_sensor(1,5,2);	//Prints Value of White Line Sensor2
		print_sensor(1,9,1);	//Prints Value of White Line Sensor3
		
		
		//checkpointFound
		if(isFollowLine(Center_white_line) && isFollowLine(Left_white_line) && isFollowLine(Right_white_line)) //checkpoint-(w-w-w)
		{
			stop();
			break;
		}
		// bot has slipped left
		else if(!isFollowLine(Left_white_line) /*&& !isFollowLine(Center_white_line)*/ && isFollowLine(Right_white_line))//flag=0 => -b- here(b-b-0)
		{
			forward();
			velocity(normalSpeed,lowSpeed);
		}
		// bot has slipped right
		else if(isFollowLine(Left_white_line) /*&& !isFollowLine(Center_white_line)*/ && !isFollowLine(Right_white_line))//flag=0 => -b- here(0-b-b)
		{
			flag=1;
			forward();
			velocity(lowSpeed,normalSpeed);
		}
		// center is on follow line
		else if(isFollowLine(Center_white_line))//center(just start when 0-w-0)
		{
			forward();
			velocity(normalSpeed,normalSpeed);
		}
		//Take right turn
		/*if(!isFollowLine(Left_white_line) && isFollowLine(Center_white_line) && isFollowLine(Right_white_line))//end stop(b-w-w)
		{
			stop();
			right_degrees(90);
		}
		//Take left turn
		if(isFollowLine(Left_white_line) && isFollowLine(Center_white_line) && !isFollowLine(Right_white_line))//end stop(w-w-b)
		{
			stop();
			left_degrees(90);
		}*/
		//End of white line or bot out of track
		//if(!isFollowLine(Center_white_line) && !isFollowLine(Left_white_line) && !isFollowLine(Right_white_line))//end stop(b-b-b)
		//{
			//velocity(0,0);
		//}
	}
}


/*
*
* when bot receives "0" as checkpoint point value to go, it means it has to go rest position
* rotate 180 degree from current position and follow the line upto 0th checkpoint.
* rotate 180 degree there and stop.
*
*/
void goToRestPosition(unsigned char currentCheckPoint)
{
	forward_mm(botLength);
	right_degrees(135);
	right_upto_follow_line();

	int checkpointFound=0;

	stop();
	while(1)
	{

		Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
		Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
		Right_white_line = ADC_Conversion(1);	//Getting data of Right WL Sensor

		flag=0;

		print_sensor(1,1,3);	//Prints value of White Line Sensor1
		print_sensor(1,5,2);	//Prints Value of White Line Sensor2
		print_sensor(1,9,1);	//Prints Value of White Line Sensor3
		
		
		//checkpointFound
		if(isFollowLine(Center_white_line) && (isFollowLine(Left_white_line) || isFollowLine(Right_white_line))) //checkpoint-(w-w-w)
		{
			
			if (checkpointFound==0)
			{
				checkpointFound=1;
				currentCheckPoint--;
				//lcd_clear_print(1,1,currentCheckPoint,3);
				//_delay_ms(1000);
				if(currentCheckPoint%3!=0)
				{
					//_delay_ms(1000);
					stop();
					forward_mm(botLength);
					stop();
					_delay_ms(1000);
					left_degrees(45);
					left_upto_follow_line();
					_delay_ms(1000);
				}
				if(currentCheckPoint==0)
				{
					stop();
					forward_mm(botLength);
					right_degrees(135);
					right_upto_follow_line();
					break;
				}
			}
			else
			{
				forward();
				velocity(normalSpeed,normalSpeed);
			}
		}
		// bot has slipped left
		else if(!isFollowLine(Left_white_line) /*&& !isFollowLine(Center_white_line)*/ && isFollowLine(Right_white_line))//flag=0 => -b- here(b-b-0)
		{
			forward();
			velocity(normalSpeed,lowSpeed);
		}
		// bot has slipped right
		else if(isFollowLine(Left_white_line) /*&& !isFollowLine(Center_white_line)*/ && !isFollowLine(Right_white_line))//flag=0 => -b- here(0-b-b)
		{
			flag=1;
			forward();
			velocity(lowSpeed,normalSpeed);
		}
		// center is on follow line
		else if(isFollowLine(Center_white_line))//center(just start when 0-w-0)
		{
			forward();
			velocity(normalSpeed,normalSpeed);
			checkpointFound=0;
		}
		//Take right turn
		/*if(isFollowLine(Center_white_line) && isFollowLine(Right_white_line))//end stop(b-w-w)
		{
			_delay_ms(1000);
			stop();
			_delay_ms(1000);
			right_degrees(90);
			_delay_ms(1000);

		}
		//Take left turn
		if(isFollowLine(Left_white_line) && isFollowLine(Center_white_line))//end stop(w-w-b)
		{
			_delay_ms(1000);
			stop();
			_delay_ms(1000);
			left_degrees(90);
			_delay_ms(1000);
		}*/
		//End of white line or bot out of track
		//if(!isFollowLine(Center_white_line) && !isFollowLine(Left_white_line) && !isFollowLine(Right_white_line))//end stop(b-b-b)
		//{
			//velocity(0,0);
		//}
		if(!isFollowLine(Left_white_line) || !isFollowLine(Right_white_line))
		{
			checkpointFound=0;
		}
	}
}


/*
*
* go to a particular checkpoint
* constraint : goes only in further direction
*
*/

void goToCheckPoint(unsigned char currentCheckPoint,unsigned char checkpointToProcess)
{
	if(checkpointToProcess==0)
	{
		goToRestPosition(currentCheckPoint);
		return;
	}

	int checkpointFound=0;

	stop();
	while(1)
	{

		Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
		Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
		Right_white_line = ADC_Conversion(1);	//Getting data of Right WL Sensor

		flag=0;

		print_sensor(1,1,3);	//Prints value of White Line Sensor1
		print_sensor(1,5,2);	//Prints Value of White Line Sensor2
		print_sensor(1,9,1);	//Prints Value of White Line Sensor3
		
		
		//checkpointFound
		if(isFollowLine(Center_white_line) && (isFollowLine(Left_white_line) || isFollowLine(Right_white_line))) //checkpoint-(w-w-w)
		{
			
			if (checkpointFound==0)
			{
				checkpointFound=1;
				currentCheckPoint++;
				//lcd_clear_print(1,1,currentCheckPoint,3);
				//_delay_ms(1000);
				/*if((currentCheckPoint-1)%3!=0)
				{
					//_delay_ms(1000);
					stop();
					forward_mm(botLength);
					stop();
					_delay_ms(1000);
					right_degrees(45);
					right_upto_follow_line();
					_delay_ms(1000);
				}*/

					stop();
					buzzer_on();
					_delay_ms(500);
					buzzer_off();
					forward();

				if(checkpointToProcess==currentCheckPoint)
				{
					stop();
					break;
				}
			}
			else
			{
				forward();
				velocity(normalSpeed,normalSpeed);
			}
		}
		// bot has slipped left
		else if(!isFollowLine(Left_white_line) /*&& !isFollowLine(Center_white_line)*/ && isFollowLine(Right_white_line))//flag=0 => -b- here(b-b-0)
		{
			forward();
			velocity(normalSpeed,lowSpeed);
		}
		// bot has slipped right
		else if(isFollowLine(Left_white_line) /*&& !isFollowLine(Center_white_line)*/ && !isFollowLine(Right_white_line))//flag=0 => -b- here(0-b-b)
		{
			flag=1;
			forward();
			velocity(lowSpeed,normalSpeed);
		}
		// center is on follow line
		else //if(isFollowLine(Center_white_line))//center(just start when 0-w-0)
		{
			forward();
			velocity(normalSpeed,normalSpeed);
			//checkpointFound=0;
		}
		//Take right turn
		/*if(isFollowLine(Center_white_line) && isFollowLine(Right_white_line))//end stop(b-w-w)
		{
			_delay_ms(1000);
			stop();
			_delay_ms(1000);
			right_degrees(90);
			_delay_ms(1000);

		}
		//Take left turn
		if(isFollowLine(Left_white_line) && isFollowLine(Center_white_line))//end stop(w-w-b)
		{
			_delay_ms(1000);
			stop();
			_delay_ms(1000);
			left_degrees(90);
			_delay_ms(1000);
		}*/
		//End of white line or bot out of track
		//if(!isFollowLine(Center_white_line) && !isFollowLine(Left_white_line) && !isFollowLine(Right_white_line))//end stop(b-b-b)
		//{
			//velocity(0,0);
		//}
		if(!isFollowLine(Left_white_line) && !isFollowLine(Right_white_line))
		{
			checkpointFound=0;
		}
	}
}

void goToLineLane(unsigned char currentCheckPoint, unsigned char linePointToStop)
{
	currentCheckPoint++;
	velocity(normalSpeed,normalSpeed);
	forward_mm(botLength);
	right_degrees(135);
	right_upto_follow_line();

	int checkPointFound=0;
	stop();
	while(1)
	{
		Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
		Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
		Right_white_line = ADC_Conversion(1);	//Getting data of Right WL Sensor

		flag=0;

		print_sensor(1,1,3);	//Prints value of White Line Sensor1
		print_sensor(1,5,2);	//Prints Value of White Line Sensor2
		print_sensor(1,9,1);	//Prints Value of White Line Sensor3
	
		//checkpointFound
		if(isFollowLine(Center_white_line) && (isFollowLine(Left_white_line) || isFollowLine(Right_white_line))) //checkpoint-(w-w-w)
		{
			if (checkPointFound==0)
			{
				checkPointFound=1;
				currentCheckPoint--;
				stop();
				buzzer_on();
				_delay_ms(500);
				buzzer_off();
				forward();
				if(currentCheckPoint==0)
				{
					stop();
					forward_mm(botLength);
					if(linePointToStop==0)
					{
						left_degrees(45);
						left_upto_follow_line();
					}
					else
					{
						right_degrees(45);
						right_upto_follow_line();
					}
					stop();
					break;
				}
			}
			else
			{
				forward();
				velocity(normalSpeed,normalSpeed);
			}
		}
		// bot has slipped left
		else if(!isFollowLine(Left_white_line) /*&& !isFollowLine(Center_white_line)*/ && isFollowLine(Right_white_line))//flag=0 => -b- here(b-b-0)
		{
			forward();
			velocity(normalSpeed,lowSpeed);
		}
		// bot has slipped right
		else if(isFollowLine(Left_white_line) /*&& !isFollowLine(Center_white_line)*/ && !isFollowLine(Right_white_line))//flag=0 => -b- here(0-b-b)
		{
			forward();
			velocity(lowSpeed,normalSpeed);
		}
		// center is on follow line
		else //if(isFollowLine(Center_white_line))//center(just start when 0-w-0)
		{
			forward();
			velocity(normalSpeed,normalSpeed);
			//checkPointFound=0;
		}
		if(!isFollowLine(Left_white_line) && !isFollowLine(Right_white_line))
		{
			checkPointFound=0;
		}
	}	
}


void goToLine(unsigned char oldLinePoint,unsigned char linePointToStop, unsigned char currentCheckPoint)
{
	if(oldLinePoint!=0)
	{
		goToLineLane(currentCheckPoint,linePointToStop);
	}
	
	int currentLinePoint=oldLinePoint;
	if(linePointToStop==0)
	{
		currentLinePoint--;
		if(currentLinePoint==0)
		{
			forward_mm(botLength*4);
			stop();
		//	buzzer_on();
			_delay_ms(1000);
		//	buzzer_off();
			right_degrees(135);
		//	buzzer_on();
		//	_delay_ms(1000);
		//	buzzer_off();
			right_upto_follow_line();
		//	buzzer_on();
		//	_delay_ms(1000);
		//	buzzer_off();
			stop();
			return;
		}
	}


	int linePointFound=0;
	stop();
	while(1)	
	{
		Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
		Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
		Right_white_line = ADC_Conversion(1);	//Getting data of Right WL Sensor

		flag=0;

		print_sensor(1,1,3);	//Prints value of White Line Sensor1
		print_sensor(1,5,2);	//Prints Value of White Line Sensor2
		print_sensor(1,9,1);	//Prints Value of White Line Sensor3
	
		//checkpointFound
		if(isFollowLine(Center_white_line) && (isFollowLine(Left_white_line) || isFollowLine(Right_white_line))) //checkpoint-(w-w-w)
		{
			
			if (linePointFound==0)
			{
				//stop();
				//buzzer_on();
				//_delay_ms(500);
				//buzzer_off();
				//forward();
				linePointFound=1;
				if(linePointToStop==0)
				{
					currentLinePoint--;
					if(linePointToStop==currentLinePoint)
					{
						forward_mm(botLength*4);
						stop();
						_delay_ms(1000);
						right_degrees(135);
						right_upto_follow_line();
		//				_delay_ms(1000);
						stop();
						return;
					}
				}
				else 
				{
					//stop();
					//buzzer_on();
					//_delay_ms(500);
					//buzzer_off();
					//forward();
					currentLinePoint++;
					if(linePointToStop==currentLinePoint)
					{
						stop();
						break;
					}
				}
			}
			else
			{
				forward();
				velocity(normalSpeed,normalSpeed);
			}
		}
		// bot has slipped left
		else if(!isFollowLine(Left_white_line) /*&& !isFollowLine(Center_white_line)*/ && isFollowLine(Right_white_line))//flag=0 => -b- here(b-b-0)
		{
			forward();
			velocity(normalSpeed,lowSpeed);
		}
		// bot has slipped right
		else if(isFollowLine(Left_white_line) /*&& !isFollowLine(Center_white_line)*/ && !isFollowLine(Right_white_line))//flag=0 => -b- here(0-b-b)
		{
			forward();
			velocity(lowSpeed,normalSpeed);
		}
		// center is on follow line
		else// if(isFollowLine(Center_white_line))//center(just start when 0-w-0)
		{
			forward();
			velocity(normalSpeed,normalSpeed);
			//linePointFound=0;
		}
		if(!isFollowLine(Left_white_line) && !isFollowLine(Right_white_line))
		{
			linePointFound=0;
		}
	}		
	stop();
	//velocity(lowSpeed,lowSpeed);
	forward_mm(botLength);
	stop();
	//velocity(normalSpeed,normalSpeed);
	_delay_ms(1000);
	right_degrees(45);
	right_upto_follow_line();
	_delay_ms(1000);
}
