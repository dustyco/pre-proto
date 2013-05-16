#include "OgreLogAdapter.h"
#include "logging.h"

using namespace Logging;
using namespace Ogre;

void OgreLogAdapter::messageLogged (const String& message, LogMessageLevel lvl, bool mDbg,
		const String& logName, bool& skip) {
	switch(lvl) {
		case LML_TRIVIAL:
			DEBUG("(%s): %s", logName.c_str(), message.c_str());
			break;
		case LML_NORMAL:
			INFO("(%s): %s", logName.c_str(), message.c_str());
			break;
		case LML_CRITICAL:
			CRITICAL("(%s): %s", logName.c_str(), message.c_str());
			break;
		default:
			CRITICAL("UNKNOWN TYPE ON MESSAGE. (%s): %s", logName.c_str(), message.c_str());
			break;
	};
}
