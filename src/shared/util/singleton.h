

// See below for usage ////////////////////////////////////////////////////////
template<class T> class _singleton {
public:
	inline static T&  set (T* ptr) { m_ptr = ptr; return *m_ptr; }
	inline static T&  ref ()       { return *m_ptr; }
	inline static T*& ptr ()       { return m_ptr; }
	
private:
	static T* m_ptr;
	_singleton () {}
};
template<class T> T* _singleton<T>::m_ptr = 0;


// Usage functions ////////////////////////////////////////////////////////////
// Give the singleton an object - first one uses the object's default constructor
template<class T> inline T& set ()       { _singleton<T>::set(new T); return _singleton<T>::ref(); }
template<class T> inline T& set (T* ptr) { _singleton<T>::set(ptr);   return _singleton<T>::ref(); }

// WARNING: MUST INIT BEFORE USE
// Returns a reference to the object
//     example: ref<FooManager>()
template<class T> inline T& ref () { return _singleton<T>::ref(); }

// WARNING: MUST INIT BEFORE USE
// Returns a reference to the pointer
//     example: ptr<FooManager>()
template<class T> inline T*& ptr () { return _singleton<T>::ptr(); }


// Convenience macros that declare a reference variable with less typing //////
// May or may not be a good idea to use these since they obscure so much
//        example: REF(FooManager, foo)
//     expands to: FooManager& foo = ref<FooManager>();
#define REF(class_name, variable_name) class_name& variable_name = ref<class_name>()
#define PTR(class_name, variable_name) class_name* variable_name = ptr<class_name>()
