/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM totem_mozilla_scripting.idl
 */

#ifndef __gen_totem_mozilla_scripting_h__
#define __gen_totem_mozilla_scripting_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    totemMozillaScript */
#define TOTEMMOZILLASCRIPT_IID_STR "862af69e-16e2-4016-ad18-77fa88a1e488"

#define TOTEMMOZILLASCRIPT_IID \
  {0x862af69e, 0x16e2, 0x4016, \
    { 0xad, 0x18, 0x77, 0xfa, 0x88, 0xa1, 0xe4, 0x88 }}

class NS_NO_VTABLE totemMozillaScript : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(TOTEMMOZILLASCRIPT_IID)

  /* void Play (); */
  NS_IMETHOD Play(void) = 0;

  /* void Rewind (); */
  NS_IMETHOD Rewind(void) = 0;

  /* void Stop (); */
  NS_IMETHOD Stop(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_TOTEMMOZILLASCRIPT \
  NS_IMETHOD Play(void); \
  NS_IMETHOD Rewind(void); \
  NS_IMETHOD Stop(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_TOTEMMOZILLASCRIPT(_to) \
  NS_IMETHOD Play(void) { return _to Play(); } \
  NS_IMETHOD Rewind(void) { return _to Rewind(); } \
  NS_IMETHOD Stop(void) { return _to Stop(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_TOTEMMOZILLASCRIPT(_to) \
  NS_IMETHOD Play(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Play(); } \
  NS_IMETHOD Rewind(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Rewind(); } \
  NS_IMETHOD Stop(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Stop(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public totemMozillaScript
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_TOTEMMOZILLASCRIPT

  _MYCLASS_();

private:
  ~_MYCLASS_();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, totemMozillaScript)

_MYCLASS_::_MYCLASS_()
{
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* void Play (); */
NS_IMETHODIMP _MYCLASS_::Play()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void Rewind (); */
NS_IMETHODIMP _MYCLASS_::Rewind()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void Stop (); */
NS_IMETHODIMP _MYCLASS_::Stop()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_totem_mozilla_scripting_h__ */
