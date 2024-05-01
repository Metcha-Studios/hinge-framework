# This file was automatically generated by SWIG (https://www.swig.org).
# Version 4.2.1
#
# Do not make changes to this file unless you know what you are doing - modify
# the SWIG interface file instead.

from sys import version_info as _swig_python_version_info
# Import the low-level C/C++ module
if __package__ or "." in __name__:
    from . import _hinge_framework
else:
    import _hinge_framework

try:
    import builtins as __builtin__
except ImportError:
    import __builtin__

def _swig_repr(self):
    try:
        strthis = "proxy of " + self.this.__repr__()
    except __builtin__.Exception:
        strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)


def _swig_setattr_nondynamic_instance_variable(set):
    def set_instance_attr(self, name, value):
        if name == "this":
            set(self, name, value)
        elif name == "thisown":
            self.this.own(value)
        elif hasattr(self, name) and isinstance(getattr(type(self), name), property):
            set(self, name, value)
        else:
            raise AttributeError("You cannot add instance attributes to %s" % self)
    return set_instance_attr


def _swig_setattr_nondynamic_class_variable(set):
    def set_class_attr(cls, name, value):
        if hasattr(cls, name) and not isinstance(getattr(cls, name), property):
            set(cls, name, value)
        else:
            raise AttributeError("You cannot add class attributes to %s" % cls)
    return set_class_attr


def _swig_add_metaclass(metaclass):
    """Class decorator for adding a metaclass to a SWIG wrapped class - a slimmed down version of six.add_metaclass"""
    def wrapper(cls):
        return metaclass(cls.__name__, cls.__bases__, cls.__dict__.copy())
    return wrapper


class _SwigNonDynamicMeta(type):
    """Meta class to enforce nondynamic attributes (no new attributes) for a class"""
    __setattr__ = _swig_setattr_nondynamic_class_variable(type.__setattr__)


class Key(object):
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc="The membership flag")
    __repr__ = _swig_repr
    id_ = property(_hinge_framework.Key_id__get, _hinge_framework.Key_id__set)
    key_ = property(_hinge_framework.Key_key__get, _hinge_framework.Key_key__set)

    def __init__(self):
        _hinge_framework.Key_swiginit(self, _hinge_framework.new_Key())
    __swig_destroy__ = _hinge_framework.delete_Key

# Register Key in _hinge_framework:
_hinge_framework.Key_swigregister(Key)
class KeyPair(object):
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc="The membership flag")
    __repr__ = _swig_repr
    id_ = property(_hinge_framework.KeyPair_id__get, _hinge_framework.KeyPair_id__set)
    public_key_ = property(_hinge_framework.KeyPair_public_key__get, _hinge_framework.KeyPair_public_key__set)
    private_key_ = property(_hinge_framework.KeyPair_private_key__get, _hinge_framework.KeyPair_private_key__set)

    def __init__(self):
        _hinge_framework.KeyPair_swiginit(self, _hinge_framework.new_KeyPair())
    __swig_destroy__ = _hinge_framework.delete_KeyPair

# Register KeyPair in _hinge_framework:
_hinge_framework.KeyPair_swigregister(KeyPair)
class Cipher(object):
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc="The membership flag")

    def __init__(self, *args, **kwargs):
        raise AttributeError("No constructor defined - class is abstract")
    __repr__ = _swig_repr
    __swig_destroy__ = _hinge_framework.delete_Cipher

    def setEntKeyPair(self, ent_key_pair):
        return _hinge_framework.Cipher_setEntKeyPair(self, ent_key_pair)

    def getEntKeyPair(self):
        return _hinge_framework.Cipher_getEntKeyPair(self)

    def isKeyExists(self, key_id, file_path):
        return _hinge_framework.Cipher_isKeyExists(self, key_id, file_path)

    def encrypt(self, key, plaintext):
        return _hinge_framework.Cipher_encrypt(self, key, plaintext)

    def decrypt(self, key, ciphertext):
        return _hinge_framework.Cipher_decrypt(self, key, ciphertext)

    def encryptFile(self, key, input_file_path, output_file_path):
        return _hinge_framework.Cipher_encryptFile(self, key, input_file_path, output_file_path)

    def decryptFile(self, key, input_file_path, output_file_path):
        return _hinge_framework.Cipher_decryptFile(self, key, input_file_path, output_file_path)

    def encryptDirectory(self, key, input_directory, output_directory):
        return _hinge_framework.Cipher_encryptDirectory(self, key, input_directory, output_directory)

    def decryptDirectory(self, key, input_directory, output_directory):
        return _hinge_framework.Cipher_decryptDirectory(self, key, input_directory, output_directory)

# Register Cipher in _hinge_framework:
_hinge_framework.Cipher_swigregister(Cipher)

def encodeBase64(plain_text):
    return _hinge_framework.encodeBase64(plain_text)

def decodeBase64(base64_text):
    return _hinge_framework.decodeBase64(base64_text)

def uuidV4Generator():
    return _hinge_framework.uuidV4Generator()
class Aes256Cipher(Cipher):
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc="The membership flag")
    __repr__ = _swig_repr

    def __init__(self):
        _hinge_framework.Aes256Cipher_swiginit(self, _hinge_framework.new_Aes256Cipher())
    __swig_destroy__ = _hinge_framework.delete_Aes256Cipher

    def generateKey(self, *args):
        return _hinge_framework.Aes256Cipher_generateKey(self, *args)

    def writeKeyToFile(self, key, file_path):
        return _hinge_framework.Aes256Cipher_writeKeyToFile(self, key, file_path)

    def readKeyFromFile(self, key_id, file_path):
        return _hinge_framework.Aes256Cipher_readKeyFromFile(self, key_id, file_path)

    def isKeyExists(self, key_id, file_path):
        return _hinge_framework.Aes256Cipher_isKeyExists(self, key_id, file_path)

    def encrypt(self, key, plaintext):
        return _hinge_framework.Aes256Cipher_encrypt(self, key, plaintext)

    def decrypt(self, key, ciphertext):
        return _hinge_framework.Aes256Cipher_decrypt(self, key, ciphertext)

    def encryptFile(self, key, input_file_path, output_file_path):
        return _hinge_framework.Aes256Cipher_encryptFile(self, key, input_file_path, output_file_path)

    def decryptFile(self, key, input_file_path, output_file_path):
        return _hinge_framework.Aes256Cipher_decryptFile(self, key, input_file_path, output_file_path)

    def encryptDirectory(self, key, input_directory, output_directory):
        return _hinge_framework.Aes256Cipher_encryptDirectory(self, key, input_directory, output_directory)

    def decryptDirectory(self, key, input_directory, output_directory):
        return _hinge_framework.Aes256Cipher_decryptDirectory(self, key, input_directory, output_directory)

# Register Aes256Cipher in _hinge_framework:
_hinge_framework.Aes256Cipher_swigregister(Aes256Cipher)
class RsaCipher(Cipher):
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc="The membership flag")
    __repr__ = _swig_repr

    def __init__(self):
        _hinge_framework.RsaCipher_swiginit(self, _hinge_framework.new_RsaCipher())
    __swig_destroy__ = _hinge_framework.delete_RsaCipher

    def generateKeyPair(self, *args):
        return _hinge_framework.RsaCipher_generateKeyPair(self, *args)

    def writeKeyToFile(self, key_pair, file_path):
        return _hinge_framework.RsaCipher_writeKeyToFile(self, key_pair, file_path)

    def readKeyFromFile(self, key_id, file_path):
        return _hinge_framework.RsaCipher_readKeyFromFile(self, key_id, file_path)

    def isKeyExists(self, key_id, file_path):
        return _hinge_framework.RsaCipher_isKeyExists(self, key_id, file_path)

    def encrypt(self, public_key, plaintext):
        return _hinge_framework.RsaCipher_encrypt(self, public_key, plaintext)

    def decrypt(self, private_key, ciphertext):
        return _hinge_framework.RsaCipher_decrypt(self, private_key, ciphertext)

    def encryptFile(self, public_key, input_file_path, output_file_path):
        return _hinge_framework.RsaCipher_encryptFile(self, public_key, input_file_path, output_file_path)

    def decryptFile(self, private_key, input_file_path, output_file_path):
        return _hinge_framework.RsaCipher_decryptFile(self, private_key, input_file_path, output_file_path)

    def encryptDirectory(self, public_key, input_directory, output_directory):
        return _hinge_framework.RsaCipher_encryptDirectory(self, public_key, input_directory, output_directory)

    def decryptDirectory(self, private_key, input_directory, output_directory):
        return _hinge_framework.RsaCipher_decryptDirectory(self, private_key, input_directory, output_directory)

# Register RsaCipher in _hinge_framework:
_hinge_framework.RsaCipher_swigregister(RsaCipher)

