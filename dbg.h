#pragma once

/**
 * A little hack: cantcoap library uses DebugLib package. But it has names that
 * conflicts with mbed-os's functions... Use this empty macros instead of
 * DebugLib.
 */
#define DBG(...)
#define DBGX(...)
#define INFO(...)
