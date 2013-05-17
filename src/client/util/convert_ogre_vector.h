#pragma once


#include "hmath/linear_algebra.h"
#include <OGRE/OgreVector2.h>
#include <OGRE/OgreVector3.h>
#include <OGRE/OgreVector4.h>


vec<2,float>  convert (const Ogre::Vector2& v) { return vec<2,float>(v.x, v.y); }
Ogre::Vector2 convert (const vec<2,float>& v)  { return Ogre::Vector2(v.x, v.y); }

vec<3,float>  convert (const Ogre::Vector3& v) { return vec<3,float>(v.x, v.y, v.z); }
Ogre::Vector3 convert (const vec<3,float>& v)  { return Ogre::Vector3(v.x, v.y, v.z); }

vec<4,float>  convert (const Ogre::Vector4& v) { return vec<4,float>(v.ptr()); }
Ogre::Vector4 convert (const vec<4,float>& v)  { return Ogre::Vector4(v.a); }
