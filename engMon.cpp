#include <engMon.h>
#include <fontLabel.h>
#include <dispTools.h>
#include <mask.h>
#include <dispTools.h>
#include <navII.h>
#include <debug.h>

#define ENG_MON_X	60
#define ENG_MON_Y	16

#define TACH_X			0
#define TACH_Y			50
#define TACH_W			155
#define TACH_H			TACH_W
#define TACH_NAME		"tach155.bmp"

#define SPEED_X		165
#define SPEED_Y		TACH_Y
#define SPEED_W		TACH_W
#define SPEED_H		TACH_W
#define SPEED_NAME	"speed155.bmp"

#define FUEL_X			81	
#define FUEL_Y			TACH_Y + TACH_H		
#define FUEL_W			TACH_W
#define FUEL_H			TACH_W
#define FUEL_NAME		"fuel155.bmp"

#define OILPSI_X		20
#define OILPSI_Y		380
#define OILPSI_W		32
#define OILPSI_H		32
#define OILPSI_ON		"ltYelo32.bmp"
#define OILPSI_OFF	"dkYelo32.bmp"
#define OILPSI_LBL	"Low oil psi"
#define OIL_LBL_X		OILPSI_X + 40
#define OIL_LBL_Y		OILPSI_Y + 16
#define OIL_LBL_W		100
#define OIL_LBL_H		17		

#define TEMP_X			OILPSI_X
#define TEMP_Y			OILPSI_Y + 50
#define TEMP_W			32
#define TEMP_H			32
#define TEMP_ON		"ltBlue32.bmp"
#define TEMP_OFF		"dkBlue32.bmp"
#define TEMP_LBL		"Overtemp"
#define TEMP_LBL_X	TEMP_X + 40
#define TEMP_LBL_Y	TEMP_Y + 16
#define TEMP_LBL_W	OIL_LBL_W
#define TEMP_LBL_H	OIL_LBL_H


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
// NMEAgauge
//****************************************************************************************


NMEAgauge::NMEAgauge(rect* inRect)
	: gauge(inRect) {
	
	ourHandler = NULL;
	setWaitTime(1000);			// We'll use 1 second as a default.
}
	
	 
NMEAgauge::~NMEAgauge(void) {  }
	
	
// Hand in our NMEA handler and, if non NULL, fire us up in auto.
void NMEAgauge::setHandler(msgHandler* inHandler) {

	ourHandler = inHandler;	// Save off our handler.
	if (ourHandler) {			// If it's not NULL..
		hookup();				// Hook us up for aute.
	}								// Else, why bother?
}

	
void NMEAgauge::setWaitTime(float inMs) {

	ourTimer.setTime(inMs);
}
	
	
	
//****************************************************************************************
// guage128x128x240Deg
//****************************************************************************************


guage128x128x240Deg::guage128x128x240Deg(rect* inRect,const char* inBGPath)
	: NMEAgauge(inRect) {

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

	point	ptA;
	point	ptB;
	point	ptC;
	
	if (needleMapper &&  !isnan(value)) {	
		setupNeedle(&ptA,&ptB,&ptC,value);
		screen->drawLine(&ourCenter,&ptA,&red);
		screen->fillTriangle(&ptA,&ptB,&ptC,&red);
		screen->fillCircle(ourCenter.x,ourCenter.y,9,&red);
	}
} 


//****************************************************************************************
// tach
//****************************************************************************************


tach::tach(rect* inRect,const char* inBGPath)
	: guage128x128x240Deg(inRect,inBGPath) { }

	
tach::~tach(void) { }


void tach::idle(void) {

	float newRPM;
	
	if (ourTimer.ding()) {
		if (ourHandler) {
			newRPM = ((engParam*)ourHandler)->getRPM();
			setValue(newRPM);
		}
		ourTimer.start();
	}
}


//****************************************************************************************
// fuelGuage
//****************************************************************************************


fuelGuage::fuelGuage(rect* inRect,const char* inBGPath)
	: guage128x128x240Deg(inRect,inBGPath) { }

	
fuelGuage::~fuelGuage(void) { }


void fuelGuage::idle(void) {

	float newLevel;
	
	if (ourTimer.ding()) {
		if (ourHandler) {
			newLevel = ((fluidLevelObj*)ourHandler)->getLevel();
			setValue(newLevel);
		}
		ourTimer.start();
	}
}


//****************************************************************************************
// knotGauge
//****************************************************************************************


knotGauge::knotGauge(rect* inRect,const char* inBGPath)
	: guage128x128x240Deg(inRect,inBGPath) { }

	
knotGauge::~knotGauge(void) { }

bool knotGauge::isDifferent(float inVal) {

	 return round(value*10) != round(inVal*10); }


void knotGauge::idle(void) {

	float newSpeed;
	
	if (ourHandler) {
		newSpeed = ((waterSpeedObj*)ourHandler)->knots;
		setValue(newSpeed);
	}
}

//****************************************************************************************
// engMon
//****************************************************************************************


engMon::engMon(int newAppID)
	: panel(newAppID) {
	
	alarmTimer		= NULL;
	ourTach			= NULL;
	ourFuelGuage	= NULL;
	ourKnotGauge	= NULL;
	lowOilPsi		= NULL;
	overheat			= NULL;
	alarmTimer		= new timeObj(1000);
}


engMon::~engMon(void) { if (alarmTimer) delete(alarmTimer); }


void engMon::setup(void) {

	rect			frame;
	fontLabel*	aLbl;
	char*			onPath;
	char*			offPath;
	fontLabel*	aLabel;
	
	onPath	= NULL;
	offPath	= NULL;
	aLabel	= NULL;
	
	frame.setRect(ENG_MON_X,ENG_MON_Y,200,30);
	aLbl = new fontLabel(&frame);
	ourTxtPallette.setTypeFace(aLbl,panelTitle);
	aLbl->setValue("Engine");
	addObj(aLbl);
	
	frame.setRect(TACH_X,TACH_Y,TACH_W,TACH_H);
	setFilePath(TACH_NAME);
	ourTach = new tach(&frame,mFilePath);
	if (ourTach) {
		ourTach->setSwing(0,4000,-120,120);
		ourTach->setValue(0);
		ourTach->setHandler(ourNavApp.engHdler);
		addObj(ourTach);
	}
	
	frame.setRect(FUEL_X,FUEL_Y,FUEL_W,FUEL_H);
	setFilePath(FUEL_NAME);
	ourFuelGuage = new fuelGuage(&frame,mFilePath);
	if (ourFuelGuage) {
		ourFuelGuage->setSwing(0,100,-120,120);
		ourFuelGuage->setValue(0);
		ourFuelGuage->setHandler(ourNavApp.fuelGauge);
		addObj(ourFuelGuage);
	}
	
	frame.setRect(SPEED_X,SPEED_Y,SPEED_W,SPEED_H);
	setFilePath(SPEED_NAME);
	ourKnotGauge = new knotGauge(&frame,mFilePath);
	if (ourKnotGauge) {
		ourKnotGauge->setSwing(0,20,-120,120);
		ourKnotGauge->setValue(0);
		ourKnotGauge->setHandler(ourNavigator->knotMeter);
		addObj(ourKnotGauge);
	}

	frame.setRect(OILPSI_X,OILPSI_Y,OILPSI_W,OILPSI_H);
	setFilePath(OILPSI_ON);
	heapStr(&onPath,mFilePath);
	setFilePath(OILPSI_OFF);
	heapStr(&offPath,mFilePath);
	lowOilPsi = new bmpFlasher(&frame,onPath,offPath);
	if (lowOilPsi) {
		addObj(lowOilPsi);
	}
	frame.setRect(OIL_LBL_X,OIL_LBL_Y,OIL_LBL_W,OIL_LBL_H);
	aLabel = new fontLabel(&frame);
	if (aLabel) {
		aLabel->setValue(OILPSI_LBL);
		ourTxtPallette.setTypeFace(aLabel,tinyLabel);
		addObj(aLabel);
	}
	
	frame.setRect(TEMP_X,TEMP_Y,TEMP_W,TEMP_H);
	setFilePath(TEMP_ON);
	heapStr(&onPath,mFilePath);
	setFilePath(TEMP_OFF);
	heapStr(&offPath,mFilePath);
	overheat = new bmpFlasher(&frame,onPath,offPath);
	if (overheat) {
		addObj(overheat);
	}
	frame.setRect(TEMP_LBL_X,TEMP_LBL_Y,TEMP_LBL_W,TEMP_LBL_H);
	aLabel = new fontLabel(&frame);
	if (aLabel) {
		aLabel->setValue(TEMP_LBL);
		ourTxtPallette.setTypeFace(aLabel,tinyLabel);
		addObj(aLabel);
	}
	
	freeStr(&onPath);
	freeStr(&offPath);
}


void engMon::loop(void) {

	alarms ourAlarms;
	
	if (alarmTimer->ding()) {
		ourAlarms = ourNavApp.eAlarmHdler->getAlarms();
		lowOilPsi->manualSetHiLow(ourAlarms.lowOilPSI);
		overheat->manualSetHiLow(ourAlarms.overTemp);
		alarmTimer->start();
	}
}


void engMon::drawSelf(void) { screen->fillScreen(&black); }