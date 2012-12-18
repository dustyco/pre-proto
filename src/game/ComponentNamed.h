#pragma once


#include <iostream>

#include "EntID.h"


class ComponentNamed {
public:
	void add (EntID id, std::string name);
	
	std::string get (EntID id);
	EntID       get (std::string& name);

private:
	std::map<EntID, std::string>      m_by_id;
	std::multimap<std::string, EntID> m_by_name;
};
