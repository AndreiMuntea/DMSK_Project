#ifndef __TRACE_HPP__ 
#define __TRACE_HPP__ 

// MyDricerTraceGuid GUID provider: {15049EC9-5F92-46ED-B4D9-614004DC3FF4}
#define WPP_CONTROL_GUIDS \
    WPP_DEFINE_CONTROL_GUID( \
        MyDricerTraceGuid, (15049EC9,5F92,46ED,B4D9,614004DC3FF4),  \
            WPP_DEFINE_BIT(TRACE_FLAG_MY_DRIVER)        /* bit  0 = 0x00000001 */ \
    )

#define WPP_LEVEL_FLAGS_LOGGER(level, flags) WPP_LEVEL_LOGGER(flags)
#define WPP_LEVEL_FLAGS_ENABLED(level, flags) (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= level)

// begin_wpp config
//
// FUNC MyDriverLogTrace{LEVEL=TRACE_LEVEL_VERBOSE, FLAGS=TRACE_FLAG_MY_DRIVER} (MSG, ...);
// FUNC MyDriverLogInfo{LEVEL=TRACE_LEVEL_INFORMATION, FLAGS=TRACE_FLAG_MY_DRIVER}(MSG, ...);
// FUNC MyDriverLogWarning{LEVEL=TRACE_LEVEL_WARNING, FLAGS=TRACE_FLAG_MY_DRIVER}(MSG, ...);
// FUNC MyDriverLogError{LEVEL=TRACE_LEVEL_ERROR, FLAGS=TRACE_FLAG_MY_DRIVER}(MSG, ...);
// FUNC MyDriverLogCritical{LEVEL=TRACE_LEVEL_CRITICAL, FLAGS=TRACE_FLAG_MY_DRIVER}(MSG, ...);
//
// end_wpp
//


#endif //__TRACE_HPP__