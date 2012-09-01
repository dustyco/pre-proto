


			#ifndef __TutorialApplication_h_
			#define __TutorialApplication_h_

#include "BaseApplication.h"

class TutorialApplication :
	public BaseApplication
{
public:
	TutorialApplication (void);
	virtual ~TutorialApplication (void);
	
	void createCamera (void);

protected:
	virtual void createScene (void);
	bool frameStarted (const Ogre::FrameEvent& evt);
};

			#endif // #ifndef __TutorialApplication_h_
