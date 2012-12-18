

void ComponentRenderable::add (EntID id, std::string name) {
	m_by_id[id] = name;
	m_by_name.insert(std::pair<std::string, EntID>(name, id));
}

std::string ComponentRenderable::get (EntID id) {
	return m_by_id[id];
}
EntID ComponentRenderable::get (std::string& name) {
	
}
