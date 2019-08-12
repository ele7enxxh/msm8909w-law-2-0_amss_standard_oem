#if defined(__GNUC__)
#pragma message "UNSUPPORTED & REMOVED API, remove the false dependency to GERAN_VARIATION.H"
#elif defined(__ARMCC_VERSION)
#warning "UNSUPPORTED & REMOVED API, remove the false dependency to GERAN_VARIATION.H"
#else
// ALL OTHERS : NO MESSAGE
#error
#endif
