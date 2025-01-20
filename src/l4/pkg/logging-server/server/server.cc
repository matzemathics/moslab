#include <stdio.h>

// #include <l4/re/env>
// #include <l4/re/util/cap_alloc>
// #include <l4/re/util/br_manager>

#include <l4/re/util/object_registry>
#include <l4/sys/cxx/ipc_epiface>

#include <l4/logging-server/logging.h>

static L4Re::Util::Registry_server<> server;

struct LoggingServer: L4::Epiface_t<LoggingServer, Logger> {
    int op_log(
            Logger::Rights rights,
		    LoggingBuffer &args
    ) {
        (void)rights;
        printf("in op_log\n");

        if (args.len >= LOG_MAX_LEN) {
            printf("len to big\n");
            // todo: buffer
            return 1;
        }

        args.msg[args.len] = '\0';

        printf("%s: %s\n", name, args.msg);
        return 0;
    }

    void set_name(char const* new_name) {
        this->name = new_name;
    }

private:
    char const *name = "[unnamed]";
};

#define NUM_INSTANCES 5

static LoggingServer instances[NUM_INSTANCES];
static size_t next_instance = 0;

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

        if(next_instance >= NUM_INSTANCES)
            return - L4_EINVAL;

        auto instance = &instances[next_instance++];

        instance->set_name(tag.value<char const*>());
        server.registry()->register_obj(instance);

        res = L4::Ipc::make_cap_rw(instance->obj_cap());
        return L4_EOK;
    }
};

int main(void) {
    static SessionServer session;

    printf("Starting up session server.\n");

    // register session server
    if(!server.registry()->register_obj(&session, "logging_server").is_valid()) {
        printf("Register of 'session_server' failed\n");
        return 1;
    }

    printf("Logging Server is running.\n");

    server.loop();
    return 0;
}

