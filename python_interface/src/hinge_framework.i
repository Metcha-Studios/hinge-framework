#ifndef HINGE_API
#define HINGE_API __declspec(dllexport)
#endif

%include <windows.i>

%module hinge_framework

%{
#include "Cipher.h"
#include "include/HingeFramework/Base64Wrapper.h"
#include "include/HingeFramework/uuidV4.h"
#include "include/HingeFramework/Aes256Cipher.h"
#include "include/HingeFramework/RsaCipher.h"
%}

%include "Cipher.h"
%include "include/HingeFramework/Base64Wrapper.h"
%include "include/HingeFramework/uuidV4.h"
%include "include/HingeFramework/Aes256Cipher.h"
%include "include/HingeFramework/RsaCipher.h"
