#include "TrafficLight.h"

TrafficLight::TrafficLight(int dir, int sec, int CLK, int DIO):TM1637(CLK, DIO)
{
	direction = dir;
	defaultSec = sec;
	variaSec = sec;
}

void TrafficLight::init(int sec){
	TM1637.setBrightness(0x0f);
	nowSec = sec;
	TM1637.showNumberDec(sec);
}

void TrafficLight::show(){
	TM1637.showNumberDec(nowSec);
}

void TrafficLight::setSec(int sec){
	nowSec = sec;
}

void TrafficLight::add(int sec){
	nowSec += sec;
}

void TrafficLight::minus(int sec){
	nowSec -= sec;
}

void TrafficLight::multiply(float sec){
	float temp = nowSec * sec;
	nowSec = int(temp);
}

void TrafficLight::divide(float sec){
	float temp = nowSec / sec;
	nowSec = int(temp);
}

void TrafficLight::changeVariaSec(int sec){
	variaSec -= sec;
}

void TrafficLight::clear(){
	TM1637.clear();
}
