/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "L4Re Operating System Framework", "index.html", [
    [ "Overview", "index.html", null ],
    [ "Introduction", "l4re_intro.html", [
      [ "L4Re Microkernel", "l4re_intro.html#fiasco_intro", [
        [ "Communication", "l4re_intro.html#l4re_concepts_fiasco_ipc", null ],
        [ "Kernel Objects", "l4re_intro.html#l4re_concepts_fiasco_kobjects", null ]
      ] ],
      [ "L4Re System Structure", "l4re_intro.html#l4re_system_structure", null ],
      [ "L4Re Runtime Environment", "l4re_intro.html#main_l4re_sec", null ]
    ] ],
    [ "Tutorial", "l4re_tutorial.html", [
      [ "Customizations", "l4re_tutorial.html#customizations", null ]
    ] ],
    [ "Programming for L4Re", "l4re_concepts.html", [
      [ "L4 Inter-Process Communication (IPC)", "l4re_concepts_ipc.html", [
        [ "IPC mechanism", "l4re_concepts_ipc.html#l4re_concepts_ipc_mechanism", [
          [ "IPC Flags", "l4re_concepts_ipc.html#l4re_concepts_descr_flags", null ],
          [ "Partner capability selector", "l4re_concepts_ipc.html#l4re_concepts_descr_capidx", null ],
          [ "IPC Label", "l4re_concepts_ipc.html#l4re_concepts_descr_label", null ],
          [ "IPC Message Tag", "l4re_concepts_ipc.html#l4re_concepts_descr_msgtag", null ],
          [ "IPC Timeouts", "l4re_concepts_ipc.html#l4re_concepts_descr_timeouts", null ],
          [ "User-level Thread Control Block", "l4re_concepts_ipc.html#l4re_concepts_descr_utcb", [
            [ "IPC Message registers", "l4re_concepts_ipc.html#l4re_concepts_descr_mrs", null ],
            [ "IPC Buffer Registers", "l4re_concepts_ipc.html#l4re_concepts_descr_brs", null ],
            [ "IPC Buffer Descriptor Register", "l4re_concepts_ipc.html#l4re_concepts_descr_bdr", null ],
            [ "IPC Thread Control Registers", "l4re_concepts_ipc.html#l4re_concepts_descr_tcrs", null ]
          ] ],
          [ "Transfer of Typed Send Items", "l4re_concepts_ipc.html#l4re_concepts_descr_transfer", null ]
        ] ],
        [ "Examples", "l4re_concepts_ipc.html#l4re_concepts_ipc_examples", [
          [ "User Thread to Kernel Object", "l4re_concepts_ipc.html#l4re_concepts_ipc_examples_1", null ],
          [ "User Thread to User Thread", "l4re_concepts_ipc.html#l4re_concepts_ipc_examples_2", [
            [ "Simple Messages", "l4re_concepts_ipc.html#l4re_concepts_ipc_examples_2_1", null ],
            [ "Send Items", "l4re_concepts_ipc.html#l4re_concepts_ipc_examples_2_2", null ]
          ] ],
          [ "User Thread to User Object", "l4re_concepts_ipc.html#l4re_concepts_ipc_examples_3", null ]
        ] ]
      ] ],
      [ "Capabilities and Naming", "l4re_concepts_naming.html", null ],
      [ "Spaces and Mappings", "l4re_concepts_mapping.html", null ],
      [ "Initial Environment and Application Bootstrapping", "l4re_concepts_env_and_start.html", [
        [ "Configuring an application before startup", "l4re_concepts_env_and_start.html#l4re_ns_config", null ],
        [ "Connecting clients and servers", "l4re_concepts_env_and_start.html#l4re_config_connection", null ]
      ] ],
      [ "Memory management - Data Spaces and the Region Map", "l4re_concepts_ds_rm.html", [
        [ "User-level paging", "l4re_concepts_ds_rm.html#l4re_concept_pagers", [
          [ "Data spaces", "l4re_concepts_ds_rm.html#l4re_concept_data_spaces", null ],
          [ "Virtual Memory Handling", "l4re_concepts_ds_rm.html#l4re_concept_regions", null ],
          [ "Memory Allocation", "l4re_concepts_ds_rm.html#l4re_concept_mem_alloc", null ]
        ] ]
      ] ],
      [ "Program Input and Output", "l4re_concepts_stdio.html", null ],
      [ "Initial Memory Allocator and Factory", "l4re_concepts_memalloc.html", null ],
      [ "Application and Server Building Blocks", "l4re_concepts_apps_svr.html", [
        [ "Creating Additional Application Threads", "l4re_concepts_apps_svr.html#l4re_concepts_app_thread", null ],
        [ "Providing a Service", "l4re_concepts_apps_svr.html#l4re_concepts_service", null ]
      ] ],
      [ "Pthread Support", "l4re_pthreads.html", null ],
      [ "Interface Definition Language", "l4_cxx_ipc_iface.html", [
        [ "Parameter types for RPC", "l4_cxx_ipc_iface.html#l4_cxx_ipc_iface_types", null ],
        [ "RPC Return Types", "l4_cxx_ipc_iface.html#l4_cxx_ipc_iface_return_types", null ],
        [ "RPC Method Declaration", "l4_cxx_ipc_iface.html#l4_cxx_ipc_iface_members", null ]
      ] ],
      [ "L4Re Build System", "l4re_build_system.html", [
        [ "Building L4Re", "l4re_build_system.html#l4re_build_system_using", null ],
        [ "Writing BID Make Files", "l4re_build_system.html#l4re_build_system_writing", null ],
        [ "prog.mk - Application Role", "bid_role_prog.html", null ],
        [ "include.mk - Header File Role", "bid_role_include.html", null ],
        [ "test.mk - Test Application Role", "bid_role_test.html", null ]
      ] ],
      [ "Kernel Factory", "l4re_concepts_kernel_factory.html", [
        [ "Passing parameters for the create stream", "l4re_concepts_kernel_factory.html#kernel_factory_create", null ]
      ] ]
    ] ],
    [ "L4Re Servers", "l4re_servers.html", [
      [ "Sigma0, the Root-Pager", "l4re_servers_sigma0.html", [
        [ "Factory", "l4re_servers_sigma0.html#Factory", null ]
      ] ],
      [ "Moe, the Root-Task", "l4re_servers_moe.html", [
        [ "Moe objects", "l4re_servers_moe.html#l4re_moe_objects", [
          [ "Factory", "l4re_servers_moe.html#l4re_moe_factory", [
            [ "Passing parameters to the create stream", "l4re_servers_moe.html#l4re_moe_memory_alloc_factory", null ]
          ] ],
          [ "Namespace", "l4re_servers_moe.html#l4re_moe_names", [
            [ "Boot FS", "l4re_servers_moe.html#l4re_moe_bootfs", null ]
          ] ],
          [ "Dataspace", "l4re_servers_moe.html#l4re_moe_dataspace", null ],
          [ "Log Subsystem", "l4re_servers_moe.html#l4re_moe_log", null ],
          [ "DMA Space", "l4re_servers_moe.html#l4re_moe_dma_space", null ],
          [ "Scheduler subsystem", "l4re_servers_moe.html#l4re_moe_scheduler", null ],
          [ "Region Map", "l4re_servers_moe.html#l4re_moe_rm", null ]
        ] ],
        [ "Command Line Options", "l4re_servers_moe.html#l4re_moe_options", null ]
      ] ],
      [ "Ned, the Init Process", "l4re_servers_ned.html", [
        [ "Lua Bindings for L4Re", "l4re_servers_ned.html#l4re_ned_lua_l4re", [
          [ "Capabilities in Lua", "l4re_servers_ned.html#l4re_ned_lua_caps", null ],
          [ "Access to L4Re::Env Capabilities", "l4re_servers_ned.html#l4re_ned_lua_l4re_env", null ],
          [ "Constants", "l4re_servers_ned.html#l4re_ned_lua_consts", null ],
          [ "Application Startup Details", "l4re_servers_ned.html#l4re_ned_startup", null ],
          [ "Reacting on task termination", "l4re_servers_ned.html#l4re_ned_exit_handler", null ],
          [ "Access to the kernel debugger", "l4re_servers_ned.html#l4re_ned_jdb", null ],
          [ "Using the interactive ned prompt", "l4re_servers_ned.html#l4re_ned_interactive", null ]
        ] ],
        [ "Command Line Options", "l4re_servers_ned.html#l4re_ned_options", null ]
      ] ],
      [ "Io, the Io Server", "io.html", null ],
      [ "Cons, the Console Multiplexer", "l4re_servers_cons.html", null ],
      [ "Mag, the GUI Multiplexer", "l4re_servers_mag.html", null ],
      [ "Uvmm, the virtual machine monitor", "l4re_servers_uvmm.html", [
        [ "RAM configuration", "l4re_servers_uvmm_ram_details.html", null ]
      ] ],
      [ "l4vio_net_p2p, a virtual network point-to-point link", "l4re_servers_virtio_net_p2p.html", null ]
    ] ],
    [ "Bootstrap, the L4 kernel bootstrapper", "bootstrap.html", null ],
    [ "Deprecated List", "deprecated.html", null ],
    [ "Topics", "topics.html", "topics" ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ],
        [ "Enumerator", "namespacemembers_eval.html", null ]
      ] ]
    ] ],
    [ "Data Structures", "annotated.html", [
      [ "Data Structures", "annotated.html", "annotated_dup" ],
      [ "Data Structure Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Data Fields", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", "functions_vars" ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Enumerator", "functions_eval.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "Globals", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", "globals_func" ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", "globals_eval" ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ],
    [ "Examples", "examples.html", "examples" ]
  ] ]
];

var NAVTREEINDEX =
[
"ARCH-amd64_2asm__access_8h_source.html",
"classL4Re_1_1Env.html#a4ee984ef97add6179043b15413470ec7",
"classL4Re_1_1Util_1_1Registry__server.html#a694a009623e0bce2faf1d6699cba8720",
"classL4_1_1Base__exception.html",
"classL4_1_1Ipc_1_1Istream.html#ad33f3a887fbd32d7d04c4fb3fd9c0fcb",
"classL4_1_1Poll__timeout__kipclock.html#a6783096f90ee37e1b3dfda705c4cd0bb",
"classL4vbus_1_1Device.html#a63dcb7dba875e85f09f7ab755836bf2e",
"classL4virtio_1_1Svr_1_1Driver__mem__region__t.html#a0d0c98a19bb3156a39ec6fe4d5368c30",
"classcxx_1_1Bits_1_1Base__avl__set.html#a6b1494f45a5e96e9387c737be16f99d4",
"classcxx_1_1String.html#a330429e4e2b79ce768f6894d698c0cc7",
"functions_func_a.html",
"group__api__l4re__c__video.html#ga5cd6b0b0c57acc904bfaa677cb057cd1",
"group__l4__error__api.html#ggab4c8abc71ffa221a25c1ca5840a354f9af9925caf2489bbd5c97705fe0ee7b698",
"group__l4__memory__api.html#ga5cbba3eb068bbd8339e70dfc40d018a9",
"group__l4__utcb__api__x86.html#ggac03d87ea12ff53e0a7037eb3f9408af8aa5516052180335a4ae8ecb83bade036a",
"group__l4util__atomic.html#ga3bc077b2c3afe0bc32082c76bc190899",
"group__l4util__elf.html#gga479a6103f148a70889af697c3fa0ecffabff26ad8be32d1fbd663e7a8fdf4d9cb",
"group__l4util__portio.html#ga03c82082d9bbe894b95978cf887907ec",
"ipc__types.html#a3f3e35f220d7a35dd2ca3d012ef05008",
"namespaceL4Re_1_1Util.html#a576bd39bbb4e90efa19b3e41d4e23313",
"structElf64__Phdr.html#af50e5756da2acda5ccb02ebaa3367092",
"structL4_1_1Ipc_1_1Msg_1_1Do__in__data.html",
"structL4virtio_1_1Svr_1_1Console_1_1Port.html#a5163c0cac00845a5ddb1df7b99d69e15",
"structl4__exc__regs__t.html#a416000fd7366f3aea4b67c66a8bf5a54",
"structl4re__env__cap__entry__t.html#a2027e219842d2e7f2bca77b5839d07b4",
"uart__lpuart_8h_source.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';