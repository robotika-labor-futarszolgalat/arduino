int left_motor = 9;
int right_motor = 11;
int left_dir = 8;
int right_dir = 10;

int mleft_sensor=2;
int mright_sensor=3;

int forward_speed = 200;
int turn_speed = 150;

//int distance_sensors[];

volatile long left_steps = 0;
volatile long right_steps = 0;

long left_target = 0;
long right_target = 0;

volatile bool stop_flag = false; 

void lstep_interrupt();
void rstep_interrupt();
void forward(int unit);
void backward(int unit);
int digitalPinToInterrupt(int pint);
void turn(int direction, int unit);
void stop();

void setup()
{
	pinMode(mleft_sensor,INPUT);
	pinMode(mright_sensor,INPUT);
	pinMode(left_motor,OUTPUT);
	pinMode(right_motor,OUTPUT);
	pinMode(left_dir,OUTPUT);
	pinMode(right_dir,OUTPUT);
	
	
	attachInterrupt(digitalPinToInterrupt(mleft_sensor), lstep_interrupt, FALLING);
	attachInterrupt(digitalPinToInterrupt(mright_sensor), rstep_interrupt, FALLING);
	Serial.begin(9600);
	
}

void loop()
{
	if(Serial.available())
	{
		int action = Serial.parseInt();
		
		switch(action){
				case 1:
					forward(Serial.parseInt());
					break;
				case 2:
					turn(Serial.parseInt(),Serial.parseInt());
				case 3:
					backward(Serial.parseInt());
					break;

		}
	}
	
	if(stop_flag){
			stop();
		stop_flag = false;
	}
	
	//Serial.println(left_steps);
	//Serial.println(right_steps);
}

void stop(){
	analogWrite(left_motor,0);
	analogWrite(right_motor,0);
}

void forward(int unit)
{
	int leftpos = left_steps;
	int rightpos = right_steps;
	
	digitalWrite(left_dir,0);
	digitalWrite(right_dir,0);
	
	analogWrite(left_motor,forward_speed);
	analogWrite(right_motor,forward_speed);
	
	left_target = leftpos+unit;
	right_target = rightpos+unit;
	
	//while(left_steps< leftpos+unit || right_steps < rightpos+unit){}
	
	
}

void backward(int unit)
{
	int leftpos = left_steps;
	int rightpos = right_steps;
	
	digitalWrite(left_dir,1);
	digitalWrite(right_dir,1);
	
	analogWrite(left_motor,forward_speed);
	analogWrite(right_motor,forward_speed);
	
	left_target = leftpos+unit;
	right_target = rightpos+unit;
	
	
}

void turn(int direction, int unit)
{
	int leftpos = left_steps;
	int rightpos = right_steps;
	
	if(direction){
		digitalWrite(left_dir,0);
		digitalWrite(right_dir,1);
	}else{
		digitalWrite(left_dir,1);
		digitalWrite(right_dir,0);
	}
	
	analogWrite(left_motor,turn_speed);
	analogWrite(right_motor,turn_speed);
	
	left_target = leftpos+unit;
	right_target = rightpos+unit;
	
	/*while(left_steps< leftpos+unit || right_steps < rightpos+unit){}
	
	analogWrite(left_motor,0);
	analogWrite(right_motor,0);*/
}

void lstep_interrupt()
{
	++left_steps;
	
	stop_flag = left_steps > left_target;
}

void rstep_interrupt()
{
	++right_steps;
	
	stop_flag = right_steps > right_target;
}

int digitalPinToInterrupt(int pin)
{
	//arduino uno
	switch(pin){
			case 2:
				return 0;
			case 3:
				return 1;
	}
	
	//arduino leonardo,yun
	/*switch(pin){
			case 3:
				return 0;
			case 2:
				return 1;
			case 0:
				return 2;
			case 1:
				return 3;
			case 7:
				return 4;
				
	}*/
}
