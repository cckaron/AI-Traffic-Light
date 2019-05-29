#define TrafficLight_h
#include "TM1637Display.h"

class TrafficLight
{
public:
	TrafficLight(int dir, int sec, int CLK, int DIO);
	void init(int sec);
	void show();
	void add(int sec);
	void minus(int sec);
	void multiply(float sec);
	void divide(float sec);
	void setSec(int sec);
	void changeVariaSec(int sec);
	void clear();
	int direction;
	int defaultSec;
	int nowSec;
	int variaSec;
	TM1637Display TM1637;
};
