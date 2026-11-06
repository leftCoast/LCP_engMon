#include <engMon.h>
#include <fontLabel.h>
#include <dispTools.h>
#include <mask.h>
//#include <colorRect.h>

#include <debug.h>

#define ENG_MON_X	60
#define ENG_MON_Y	16

#define TACH_X		96
#define TACH_Y		100
#define TACH_W		128
#define TACH_H		128
#define TACH_NAME	"tach128.bmp"



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
// tach
//****************************************************************************************


tach::tach(rect* inRect,const char* inBGPath)
	: gauge(inRect) {
	
	tachFace			= new bmpObj(inRect,inBGPath);
	needleMapper	= NULL;
	ourCenter		= inRect->getCenter();
	needleLen		= 0;
}

	
tach::~tach(void) {
	
	if (needleMapper) delete(needleMapper);
}


void tach::setNeedleLen(int inLen) { needleLen = inLen; }

 
void tach::setSwing(float startVal,float endVal,float startAngle,float endAngle) {
	
	needleMapper = new mapper(startVal,endVal,startAngle,endAngle);	
} 	


bool tach::isDifferent(float inVal) { return round(value) != round(inVal); }


void tach::drawBackGround(void) { tachFace->drawSelf(); }


void tach::eraseNeedle(void) {

	float		angle;
	point		ptA;
	point		ptB;
	point		ptC;
	point		ptM;
	maskRect	ourMask(0,0,14,14);
		
	if (needleMapper &&  !isnan(oldValue)) {	
		ptA.x = 0;
		ptA.y = -needleLen;
		ptB.x = -1;
		ptB.y = 0;
		ptC.x = +1;
		ptC.y = 0;
		ptM.x = 0;
		ptM.y = -(needleLen-9);
		
		angle = needleMapper->map(oldValue);

		rotate(&ptA,angle);
		rotate(&ptB,angle);
		rotate(&ptC,angle);
		rotate(&ptM,angle);
		
		ptA.x += ourCenter.x;
		ptA.y += ourCenter.y;
		ptB.x += ourCenter.x;
		ptB.y += ourCenter.y;
		ptC.x += ourCenter.x;
		ptC.y += ourCenter.y;
		ptM.x += ourCenter.x;
		ptM.y += ourCenter.y;
		
		rect frame;
		ourMask.centerRect(&ptM);
		//screen->drawRect(&ourMask,&green);
		frame.setRect(&ourMask);
		frame.insetRect(2);
		ourMask.setInverse(true);
		tachFace->setMask(&ourMask);
		screen->drawLine(&ourCenter,&ptA,&black);
		screen->fillTriangle(&ptA,&ptB,&ptC,&black);
		screen->fillCircle(ourCenter.x,ourCenter.y,9,&black);
		tachFace->drawSelf();
		tachFace->setMask(NULL);
		
	}
}


void tach::drawNeedle(void) {

	float	angle;
	point	ptA;
	point	ptB;
	point	ptC;
	
	if (needleMapper &&  !isnan(value)) {	
		ptA.x = 0;
		ptA.y = -needleLen;
		ptB.x = -1;
		ptB.y = 0;
		ptC.x = +1;
		ptC.y = 0;

		angle = needleMapper->map(value);

		rotate(&ptA,angle);
		rotate(&ptB,angle);
		rotate(&ptC,angle);
	
		ptA.x += ourCenter.x;
		ptA.y += ourCenter.y;
		ptB.x += ourCenter.x;
		ptB.y += ourCenter.y;
		ptC.x += ourCenter.x;
		ptC.y += ourCenter.y;
		
		screen->drawLine(&ourCenter,&ptA,&red);
		screen->fillTriangle(&ptA,&ptB,&ptC,&red);
		screen->fillCircle(ourCenter.x,ourCenter.y,9,&red);
	}
} 


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
		ourTach->setNeedleLen(50);
		ourTach->setSwing(0,4000,-120,120);
		ourTach->setValue(10);
		addObj(ourTach);
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