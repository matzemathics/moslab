#include <cstdio>
#include <cstring>
#include <string>

#include <l4/sys/cxx/ipc_epiface>
#include <l4/re/util/object_registry>
#include <l4/cxx/exceptions>

#include <l4/logging-server/logging.h>
#include "terminal.h"

#define MAX_TAG_LEN 20

static L4Re::Util::Registry_server<> server;
static Terminal *terminal;

struct LoggingServer: L4::Epiface_t<LoggingServer, Logger> {
    LoggingServer(const char *name) : name(name) {}

    int op_log(Logger::Rights rights, LoggingBuffer &args)
    {
        (void)rights;

        buffer.append((const char*)args.msg, args.len);

        if (args.len >= LOG_MAX_LEN) {
            return 0;
        }

        terminal->print_str("[", ~0U);
        terminal->print_str(name.c_str(), ~0U);
        terminal->print_str("]: ", ~0U);
        terminal->print_str(buffer.c_str(), ~0U);
        terminal->print_str("\n", ~0U);
        buffer.clear();

        return 0;
    }

private:
    std::string name;
    std::string buffer;
};

struct SessionServer: L4::Epiface_t<SessionServer, L4::Factory> {
    int op_create (
        L4::Factory::Rights rights,
        L4::Ipc::Cap<void> &res,
        l4_mword_t type,
        L4::Ipc::Varg_list<> args
    ) {
        (void)rights;

        if (type != 0)
            return - L4_ENODEV;

        L4::Ipc::Varg tag = args.pop_front();

        if (! tag.is_of<char const *>())
            return - L4_EINVAL;

        if (strlen(tag.value<char const*>()) > MAX_TAG_LEN)
            return - L4_EINVAL;

        auto instance = new LoggingServer(tag.value<char const*>());
        server.registry()->register_obj(instance);

        res = L4::Ipc::make_cap_rw(instance->obj_cap());
        return L4_EOK;
    }
};

int main(void) {
    if (Terminal::init(&terminal)) {
	    return 1;
    }

    // register session server
    if(!server.registry()->register_obj(new SessionServer(), "logging_server").is_valid()) {
        printf("Register of 'session_server' failed\n");
        return 1;
    }

    printf("Logging Server is running.\n");

    server.loop();
    return 0;
}

