#ifndef engMon_h
#define engMon_h


#include	<lilOS.h>
#include <bmpFlasher.h>
#include <handlers.h>


//****************************************************************************************
// gauge
//****************************************************************************************


class gauge :	public drawObj {

	public:
				gauge(rect* inRect);
	virtual	~gauge(void);
	
	virtual	void	setValue(float inVal);
	virtual	void	drawBackGround(void);
	virtual	void	eraseNeedle(void);
	virtual	void	drawNeedle(void);
	virtual	void	drawSelf(void);
	
	protected:
	
	virtual	bool	isDifferent(float inVal);
	
				bool	needBackGnd;
				float	value;
				float	oldValue;
};



//****************************************************************************************
// NMEAgauge
//****************************************************************************************


class NMEAgauge :	public gauge,
						public idler {

	public:
				NMEAgauge(rect* inRect);
	virtual	~NMEAgauge(void);
	
	virtual	void	setHandler(msgHandler* inHandler);
	virtual	void	setWaitTime(float inMs);
				
				msgHandler*	ourHandler;
				timeObj		ourTimer;
};
	
	
				
//****************************************************************************************
// guage128x128x240Deg
//****************************************************************************************


#define DEF_NEEDLE_LEN	50


class guage128x128x240Deg :	public NMEAgauge {

	public:
				guage128x128x240Deg(rect* inRect,const char* inBGPath);
	virtual	~guage128x128x240Deg(void);

	virtual	void	setNeedleLen(int inLen);
	virtual	void	setSwing(float startVal,float endVal,float startAngle=-120,float endAngle=120);
	virtual	void	eraseNeedle(void);
	virtual	void	drawNeedle(void);
	
	protected:
	
	virtual	bool	isDifferent(float inVal);
	virtual	void	drawBackGround(void);
	virtual	float	setupNeedle(point* ptA,point* ptB,point* ptC,float inValue);
	
				bmpObj*	tachFace;
				point		ourCenter;
				int		needleLen;
				mapper*	needleMapper;
};
	
	
	
//****************************************************************************************
// tach
//****************************************************************************************

class tach :	public guage128x128x240Deg {

	public:
				tach(rect* inRect,const char* inBGPath);
	virtual	~tach(void);
	
	virtual	void	idle(void);
	
	protected:
	
	virtual	bool	isDifferent(float inVal);
};



//****************************************************************************************
// fuelGuage
//****************************************************************************************


class fuelGuage :	public guage128x128x240Deg {

	public:
				fuelGuage(rect* inRect,const char* inBGPath);
	virtual	~fuelGuage(void);
	
	virtual	void	idle(void);	
};


//****************************************************************************************
// knotGauge
//****************************************************************************************


class knotGauge :	public guage128x128x240Deg {

	public:
				knotGauge(rect* inRect,const char* inBGPath);
	virtual	~knotGauge(void);
	
	virtual	bool	isDifferent(float inVal);
	
	virtual	void	idle(void);	
};


//****************************************************************************************
// engMon
//****************************************************************************************				


class engMon :	public panel {

	public:
				engMon(int ourAppID);
	virtual	~engMon(void);

	virtual	void	setup(void);
	virtual	void	drawSelf(void);
				void	loop(void);
				
				timeObj*		alarmTimer;
				tach*			ourTach;
				fuelGuage*	ourFuelGuage;
				knotGauge*	ourKnotGauge;
				bmpFlasher*	lowOilPsi;
				bmpFlasher*	overheat;
};


	

#endif

