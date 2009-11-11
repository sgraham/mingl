#if WANT_DEBUG_TEST
    #include "mingl_debug.h"
#else
    #include "mingl.h"
#endif
// this is just a test to make sure that we're not doing anything that breaks
// having multiple inclusions of mingl in different translation units
