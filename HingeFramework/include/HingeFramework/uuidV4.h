#pragma once
#ifndef UUIDV4_H
#define UUIDV4_H

#ifndef HINGE_API

#ifdef HINGE_EXPORTS
#define HINGE_API __declspec(dllexport)
#else
#define HINGE_API __declspec(dllimport)
#endif

#endif // HINGE_API

#include <string>

namespace hinge_framework {
    extern "C" HINGE_API const char* uuidV4Generator();
}

#endif // UUIDV4_H
