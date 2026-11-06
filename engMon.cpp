#include <engMon.h>
#include <fontLabel.h>
#include <dispTools.h>
#include <mask.h>
//#include <colorRect.h>

#include <debug.h>

#define ENG_MON_X	60
#define ENG_MON_Y	16

#define TACH_X		5
#define TACH_Y		100
#define TACH_W		128
#define TACH_H		TACH_W
#define TACH_NAME	"tach128.bmp"

//#define SPEED_X		187
//#define SPEED_Y		TACH_Y
//#define SPEED_W		TACH_W
//#define SPEED_H		TACH_W
//#define SPEED_NAME	"speed128.bmp"

#define FUEL_X		187			//96
#define FUEL_Y		TACH_Y		// 292
#define FUEL_W		TACH_W
#define FUEL_H		TACH_W
#define FUEL_NAME	"fuel128.bmp"



//****************************************************************************************
// gauge
//****************************************************************************************


gauge::gauge(rect* inRect)
	: drawObj(inRect) {
	
	needBackGnd = true;
	value			= NAN;
	oldValue		= NAN;
}


gauge::~gauge(void) {  }


bool gauge::isDifferent(float inVal) { return value != inVal; }


void gauge::setValue(float inVal) {

	if (isDifferent(inVal)) {
		oldValue = value;
		value = inVal;
		setNeedRefresh(true);
	}
}


void gauge::drawBackGround(void) { }


void gauge::eraseNeedle(void) { }


void gauge::drawNeedle(void) { }


void gauge::drawSelf(void) {

	if (needBackGnd) {
		drawBackGround();
		needBackGnd = false;
	}
	eraseNeedle();
	drawNeedle();
}
	
	
	
//****************************************************************************************
// guage128x128x240Deg
//****************************************************************************************


guage128x128x240Deg::guage128x128x240Deg(rect* inRect,const char* inBGPath)
	: gauge(inRect) {

	tachFace			= new bmpObj(inRect,inBGPath);
	needleMapper	= NULL;
	ourCenter		= inRect->getCenter();
	needleLen		= DEF_NEEDLE_LEN;
}

guage128x128x240Deg::~guage128x128x240Deg(void) { if (needleMapper) delete(needleMapper); }


void guage128x128x240Deg::setNeedleLen(int inLen) { needleLen = inLen; }

 
void guage128x128x240Deg::setSwing(float startVal,float endVal,float startAngle,float endAngle) {
	
	needleMapper = new mapper(startVal,endVal,startAngle,endAngle);	
} 	


bool guage128x128x240Deg::isDifferent(float inVal) { return round(value) != round(inVal); }


void guage128x128x240Deg::drawBackGround(void) { tachFace->drawSelf(); }


float guage128x128x240Deg::setupNeedle(point* ptA,point* ptB,point* ptC,float inValue) {

	float		angle;
	
	ptA->x = 0;
	ptA->y = -needleLen;
	ptB->x = -1;
	ptB->y = 0;
	ptC->x = +1;
	ptC->y = 0;

	angle = needleMapper->map(inValue);

	rotate(ptA,angle);
	rotate(ptB,angle);
	rotate(ptC,angle);

	ptA->x += ourCenter.x;
	ptA->y += ourCenter.y;
	ptB->x += ourCenter.x;
	ptB->y += ourCenter.y;
	ptC->x += ourCenter.x;
	ptC->y += ourCenter.y;
	
	return angle;
}


void guage128x128x240Deg::eraseNeedle(void) {

	float		angle;
	point		ptA;
	point		ptB;
	point		ptC;
	maskRect	ourMask(0,0,14,14);
	point		ptM;
		
	if (needleMapper &&  !isnan(oldValue)) {	
		angle = setupNeedle(&ptA,&ptB,&ptC,oldValue);
		
		ptM.x = 0;
		ptM.y = -(needleLen-9);
		rotate(&ptM,angle);
		ptM.x += ourCenter.x;
		ptM.y += ourCenter.y;
		ptM.x -= tachFace->x;
		ptM.y -= tachFace->y;
		ourMask.centerRect(&ptM);
		ourMask.setInverse(true);
		tachFace->setMask(&ourMask);
		
		screen->drawLine(&ourCenter,&ptA,&black);
		screen->fillTriangle(&ptA,&ptB,&ptC,&black);
		screen->fillCircle(ourCenter.x,ourCenter.y,9,&black);
		tachFace->drawSelf();
		
		tachFace->setMask(NULL);
	}
}


void guage128x128x240Deg::drawNeedle(void) {

	float	angle;
	point	ptA;
	point	ptB;
	point	ptC;
	
	if (needleMapper &&  !isnan(value)) {	
		setupNeedle(&ptA,&ptB,&ptC,value);
		screen->drawLine(&ourCenter,&ptA,&red);
		screen->fillTriangle(&ptA,&ptB,&ptC,&red);
		screen->fillCircle(ourCenter.x,ourCenter.y,9,&red);
		screen->drawCircle(ourCenter.x,ourCenter.y,132,&white);
		screen->drawCircle(ourCenter.x,ourCenter.y,133,&white);
	}
} 


//****************************************************************************************
// tach
//****************************************************************************************


tach::tach(rect* inRect,const char* inBGPath)
	: guage128x128x240Deg(inRect,inBGPath) { }

	
tach::~tach(void) { }


//****************************************************************************************
// fuelGuage
//****************************************************************************************


fuelGuage::fuelGuage(rect* inRect,const char* inBGPath)
	: guage128x128x240Deg(inRect,inBGPath) { }

	
fuelGuage::~fuelGuage(void) { }

//****************************************************************************************
// engMon
//****************************************************************************************


engMon::engMon(int newAppID)
	: panel(newAppID) { ourTach = NULL; }


engMon::~engMon(void) {  }


void engMon::setup(void) {

	rect			frame;
	fontLabel*	aLbl;
	
	frame.setRect(ENG_MON_X,ENG_MON_Y,200,30);
	aLbl = new fontLabel(&frame);
	ourTxtPallette.setTypeFace(aLbl,panelTitle);
	aLbl->setValue("Engine panel");
	addObj(aLbl);
	
	frame.setRect(TACH_X,TACH_Y,TACH_W,TACH_H);
	setFilePath(TACH_NAME);
	ourTach = new tach(&frame,mFilePath);
	if (ourTach) {
		//ourTach->setNeedleLen(50);
		ourTach->setSwing(0,4000,-120,120);
		ourTach->setValue(10);
		addObj(ourTach);
	}
	
	frame.setRect(FUEL_X,FUEL_Y,FUEL_W,FUEL_H);
	setFilePath(FUEL_NAME);
	ourFuelGuage = new fuelGuage(&frame,mFilePath);
	if (ourFuelGuage) {
		//ourTach->setNeedleLen(50);
		ourFuelGuage->setSwing(0,100,-120,120);
		ourFuelGuage->setValue(37);
		addObj(ourFuelGuage);
	}
}

timeObj	tachTimer(100);
int		RPM = 0;
bool		rising = true;

void engMon::loop(void) {

	if (tachTimer.ding() && ourTach) {
		if (rising) {
			RPM = RPM + 20;
			if (RPM>4000) {
				rising = false;
			} else {
				ourTach->setValue(RPM);
			}
		}
		if (!rising) {
			RPM = RPM - 20;
			if (RPM<0) {
				rising = true;
			} else {
				ourTach->setValue(RPM);
			}
		}
		tachTimer.start();
	}
}


void engMon::drawSelf(void) { screen->fillScreen(&black); }