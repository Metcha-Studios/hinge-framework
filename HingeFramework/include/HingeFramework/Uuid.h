#pragma once
#ifndef UUID_H
#define UUID_H

#ifndef HINGE_API

#ifdef HINGE_EXPORTS
#define HINGE_API __declspec(dllexport)
#else
#define HINGE_API __declspec(dllimport)
#endif // HINGE_EXPORTS

#endif // HINGE_API

//#include <string>

namespace hinge_framework {
    extern "C" HINGE_API const char* uuidV4Generator();
}

#endif // UUID_H
