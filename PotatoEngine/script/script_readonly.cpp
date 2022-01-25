#include "script_readonly.hpp"

const int* Readonly::allocateInt(int value) { return Stack::allocateInt(value); }

const float* Readonly::allocateFloat(float value) { return Stack::allocateFloat(value); }

const char* Readonly::allocateString(const char* value) { return Stack::allocateString(value); }
