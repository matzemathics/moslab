#pragma once

#include <l4/sys/capability>
#include <l4/sys/cxx/ipc_iface>

#include <l4/sys/utcb.h>

#define LOG_MAX_LEN (L4_UTCB_GENERIC_DATA_SIZE - 1) * sizeof(l4_uint32_t)

struct LoggingBuffer
{
    l4_uint32_t len;
    unsigned char msg[LOG_MAX_LEN];
};

struct Logger : L4::Kobject_t<Logger, L4::Kobject>
{
    L4_INLINE_RPC(int, log, (LoggingBuffer));
    typedef L4::Typeid::Rpcs<log_t> Rpcs;
};

