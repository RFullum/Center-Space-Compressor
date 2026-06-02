/*
  ==============================================================================

    ShutdownTrace.h

    Optional trace for plugin shutdown. Used to diagnose host
    Diagnose shutdown beachballing (e.g. Ableton Live taking seconds to quit with
    many Center Space instances loaded).

    Enable by configuring CMake with -DCENTERSPACE_TRACE_SHUTDOWN=ON.
    Disabled in shipping builds.

    When enabled, every CS_TRACE(...) call appends a millisecond-timestamped
    line to ~/Library/Logs/CenterSpace/shutdown.log. Watch it live with:
        tail -f ~/Library/Logs/CenterSpace/shutdown.log

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#if CS_TRACE_SHUTDOWN

namespace CenterSpace
{
    inline void InstallShutdownLogger()
    {
        static std::once_flag flag;
        std::call_once(flag, []
        {
            // Intentionally leaked: must outlive every plugin instance, including
            // ones destructed during process teardown. Process exit reclaims it.
            auto *logger = juce::FileLogger::createDefaultAppLogger
                ("CenterSpace", "shutdown.log", "Center Space shutdown trace");
            juce::Logger::setCurrentLogger(logger);
        });
    }

    inline void TraceShutdown(const juce::String &msg)
    {
        const auto now = juce::Time::getCurrentTime();
        const auto stamp = now.toString(true, true, true, true)
                         + juce::String::formatted(".%03d", now.getMilliseconds());
        juce::Logger::writeToLog("[" + stamp + "] " + msg);
    }
}

#define CS_TRACE_INSTALL() ::CenterSpace::InstallShutdownLogger()
#define CS_TRACE(msg)      ::CenterSpace::TraceShutdown(msg)

#else

#define CS_TRACE_INSTALL() ((void)0)
#define CS_TRACE(msg)      ((void)0)

#endif
