#pragma once

#include <l4/sys/capability>
#include <l4/sys/cxx/ipc_iface>

#include <l4/sys/utcb.h>

#define LOG_MAX_LEN ((L4_UTCB_GENERIC_DATA_SIZE - 1) * sizeof(l4_uint32_t))

struct LoggingBuffer
{
    l4_uint32_t len;
    unsigned char msg[LOG_MAX_LEN];
};

struct Logger : L4::Kobject_t<Logger, L4::Kobject>
{
    L4_INLINE_RPC(int, log, (LoggingBuffer));
    typedef L4::Typeid::Rpcs<log_t> Rpcs;

    inline int log_all(const char *str) {
        LoggingBuffer buf;

        while (true) {
            unsigned counter = 0;
            unsigned char *target = buf.msg;

            while (*str && counter++ < LOG_MAX_LEN)
                *target++ = *str++;

            if (counter < LOG_MAX_LEN) {
                *target = 0;
                buf.len = counter;
                return log(buf);
            }
            else {
                buf.len = LOG_MAX_LEN;
                int result = log(buf);
                if (result) return result;
            }
        }
    }
};

