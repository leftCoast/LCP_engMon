#ifndef engMon_h
#define engMon_h



#include	<lilOS.h>


//****************************************************************************************
// gauge
//****************************************************************************************


class gauge :	public drawObj {

	public:
				gauge(rect* inRect);
	virtual	~gauge(void);
	
	virtual	bool	isDifferent(float inVal);
	virtual	void	setValue(float inVal);
	virtual	void	drawBackGround(void);
	virtual	void	eraseNeedle(void);
	virtual	void	drawNeedle(void);
	virtual	void	drawSelf(void);
	
				bool	needBackGnd;
				float	value;
				float	oldValue;
};



//****************************************************************************************
// tach
//****************************************************************************************

class tach :	public gauge {

	public:
				tach(rect* inRect,const char* inBGPath);
	virtual	~tach(void);
	
				void	setNeedleLen(int inLen);
				void	setSwing(float startVal,float endVal,float startAngle,float endAngle);
	virtual	bool	isDifferent(float inVal);
	virtual	void	drawBackGround(void);
	virtual	void	eraseNeedle(void);
	virtual	void	drawNeedle(void);
				
				bmpObj*	tachFace;
				point		ourCenter;
				int		needleLen;
				mapper*	needleMapper;
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
				
				tach*	ourTach;
};
	

#endif

