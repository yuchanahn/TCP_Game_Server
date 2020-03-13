#pragma once
#define PROP(T, val , getF, setF)						      \
T _##val;													  \
T get_##val() getF											  \
void set_##val(T value) setF								  \
_declspec(property(get = get_##val, put = set_##val))	T val \

#define PROP_G(T, val , getF)						          \
T _##val;													  \
T get_##val() getF											  \
_declspec(property(get = get_##val))	T val				  \

#define PROP_S(T, val , setF)								  \
T _##val;													  \
void set_##val(T value) setF								  \
_declspec(property(put = set_##val))	T val				  \