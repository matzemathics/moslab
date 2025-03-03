#include <cstdio>
#include <cstring>

#include <l4/re/env>
#include <l4/re/util/cap_alloc>
#include <l4/vbus/vbus>
#include <l4/io/io.h>
#include <l4/util/port_io.h>
#include <l4/irq/irq.h>
#include <l4/util/util.h>
#include <l4/sys/exception>

#include <pthread.h>
#include <stdio.h>

void *hello_thread(void *_input) {
    printf("Hello World!\n");
    return nullptr;
}

int main(int argc, const char *argv[])
{
    printf("Starting keyboard server...\n");

    pthread_t thread;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    printf("Starting a hello thread...\n");
    pthread_create(&thread, &attr, &hello_thread, NULL);

    auto vbus = L4Re::Env::env()->get_cap<L4vbus::Vbus>("vbus");
    auto icu = L4Re::Env::env()->get_cap<L4::Icu>("icu");

    if (!vbus.is_valid()) {
        printf("Could not get vbus cap\n");
        return 1;
    }

    // printf("mask: %d\n", l4_ipc_error(vbus->mask(0), l4_utcb()));

    auto device = vbus->root();
    auto child = L4vbus::Device();
    l4vbus_device_t info;

    while (device.next_device(&child, L4VBUS_MAX_DEPTH, &info) == 0) {
        printf("Found device `%s'\n", info.name);
    }

    if (l4io_request_ioport(0x60, 1) != 0) {
        printf("request_ioport(0x60) failed\n");
    }

    if (l4io_request_ioport(0x64, 1) != 0) {
        printf("request_ioport(0x64) failed\n");
    }

#ifdef CONFIGURE_IO_PORT

    // disable PS/2 devices
    while (l4util_in8(0x64) & 0x02)
        printf("waiting for keyboard controller...\n");
    l4util_out8(0xAD, 0x64);

    // disable PS/2 devices
    while (l4util_in8(0x64) & 0x02)
        printf("waiting for keyboard controller...\n");
    l4util_out8(0xA7, 0x64);

    while (l4util_in8(0x64) & 0x02)
        printf("waiting for keyboard controller...\n");

    // read configuration bytes
    l4util_out8(0x20, 0x64);
    auto conf_byte = l4util_in8(0x60);

    printf("configuration byte: %x\n", conf_byte);

    while (l4util_in8(0x64) & 0x02)
        printf("waiting for keyboard controller...\n");

    // write configuration bytes
    l4util_out8(0x60, 0x64);
    l4util_out8(0x07, 0x60);
    conf_byte = l4util_in8(0x60);

    printf("ack byte: %x\n", conf_byte);

    while (l4util_in8(0x64) & 0x02)
        printf("waiting for keyboard controller...\n");

    // read configuration bytes
    l4util_out8(0x20, 0x64);
    conf_byte = l4util_in8(0x60);

    printf("configuration byte: %x\n", conf_byte);

    while (l4util_in8(0x64) & 0x02)
        printf("waiting for keyboard controller...\n");

    // test the controller
    l4util_out8(0xAA, 0x64);
    while (!(l4util_in8(0x64) & 0x01))
        printf("waiting for keyboard controller...\n");
    conf_byte = l4util_in8(0x60);
    printf("result: %x\n", conf_byte);

    // test the device
    l4util_out8(0xAB, 0x64);
    while (!(l4util_in8(0x64) & 0x01))
        printf("waiting for keyboard controller...\n");
    conf_byte = l4util_in8(0x60);
    printf("result: %x\n", conf_byte);

    // enable the device
    l4util_out8(0xAE, 0x64);

    // reset the device
    while (l4util_in8(0x64) & 0x01)
        printf("waiting for keyboard controller...\n");
    l4util_out8(0xFF, 0x60);

    while (l4util_in8(0x64) & 0x01)
        l4util_in8(0x60);

    printf("Press a key\n");
    while ((l4util_in8(0x64) & 0x01) == 0)
        ;
    l4util_in8(0x60);

#endif

    printf("Initialisation done...\n");

    auto irq_cap = L4Re::Util::cap_alloc.alloc<L4::Irq>();
    l4_msgtag_t res = L4Re::Env::env()->factory()->create(irq_cap);
    printf("Factory::create(): %d\n", l4_error(res));

    if (!irq_cap.is_valid()) {
        printf("Could not create irq cap\n");
        return 1;
    }

    res = irq_cap->bind_thread(L4Re::Env::env()->main_thread(), 0x4320);
    printf("Irq::bind_thread: %d\n", l4_error(res));
    if (res.label() < 0) {
        printf("Irq::bind_thread faild\n");
        return 1;
    }

    l4_icu_info_t icu_info;
    res = L4::Cap<L4::Icu>(icu)->info(&icu_info);
    printf("Icu::info = { features = %x, nr_irqs = %d, nr_msis = %d }\n",
            icu_info.features, icu_info.nr_irqs, icu_info.nr_msis);

    res = icu->bind(0x1, irq_cap);
	printf("Icu::bind: %d\n", l4_error(res));

    if (l4_error(res) < 0) {
        printf("Icu::bind faild\n");
        return 1;
    }

    printf("now listnening to keyboard events\n");

    while (true) {
        l4_umword_t label;
        res = irq_cap->wait(&label);
        while (l4util_in8(0x64) & 0x01) {
            char code = l4util_in8(0x60);
            if (code == 0x0f) {
                printf("tab read.\n");
            }
            else {
                printf("code = %x\n", code);
            }
        }
    }

    return 0;
}
