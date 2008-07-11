#ifndef __SSE_WRAPPER_H__
#define __SSE_WRAPPER_H__

/* GCC breaks when using SSE intrinsics on a stack which isn't 16-byte
 * aligned, on x86-32. This attribute forces re-alignment of the stack
 * On OSX, the ABI requires 16 byte alignment, so we don't need to do this.
 */
#if defined(__GNUC__) && defined(__i386__) && !defined(__APPLE__)
#if (__GNUC__ < 4) || (__GNUC_MINOR__ < 2)
#error "SSE broken on this compiler, please upgrade"
#else
#define SSE_FUNCTION __attribute__((force_align_arg_pointer))
#endif
#else
#define SSE_FUNCTION
#endif 

/* No longer used */
#define OIL_DEFINE_IMPL_FULL_WRAPPER(func,klass,flags) \
OIL_DEFINE_IMPL_FULL(func, klass, flags)

#endif

