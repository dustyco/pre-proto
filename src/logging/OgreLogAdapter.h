#pragma once
#include <OGRE/Ogre.h>

namespace Logging {
	class OgreLogAdapter : public Ogre::LogListener {
	public:
		void messageLogged (const Ogre::String& message, Ogre::LogMessageLevel lvl, bool mDbg,
				const Ogre::String& logName, bool& skip);
	};
};
