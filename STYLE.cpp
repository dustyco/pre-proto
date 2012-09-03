

/******************************************************************************
******************************* MAJOR HEADER **********************************
******************************************************************************/
// or
// MAJOR HEADER (80 COLS WIDE) ////////////////////////////////////////////////


// ******** MINOR HEADER ********
// or
// MINOR HEADER

// Normal comment (starts with a capital)
// Tab indentation, displayed as 4 cols wide


// First level of a namespace block indents
// Nesting doesn't
namespace Bacon {
namespace Eggs {
	class Ham {
	};
};
};

class UpperCamel : Parent, Class {
public:
	UpperCamel (float f, int i);
	void lowerCamel (int a, int b);
	
	// Member variables are prefixed with m_
	float m_f;
	int   m_i;
};

// Lots of parent classes should be listed on the following lines like so:
class UpperCamel :
	Large,
	List,
	Of,
	Parent,
	Classes
{
};

// Same rule applies for lots of initialization expressions
// For short or empty functions, brackets start on the first line
UpperCamel::UpperCamel (float f, int i) : m_f(f), m_i(i) {}

// Otherwise both brackets are against the left margin (function declaration only)
// Space after function identifier (also declaration only, not while calling)
void UpperCamel::lowerCamel (int a, int b)
{
	// Deactivated code is distinguished by either:
	// No preceeding space
	//a=b;
	// Comments against the left margin
//  a = b;
	
	// Space after flow control keywords
	for (int i=0;i<100;i++) /**/;
	for (int i=0;i<100;i++) {
		//
		//
		//
	}
	// else may be on the next line depending on length
	if (a>b) /**/; else /**/;
	if (a>b) {
		//
		//
		//
	} else {  // May or may not put extra keywords on the same line as the closing bracket
		//
		//
		//
	}
}


