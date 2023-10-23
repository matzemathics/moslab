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
  [ "L4Re Operating System Framework – Interface and Usage Documentation", "index.html", [
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
      [ "l4vio_net_p2p, a virtual network point-to-point link", "l4re_servers_virtio_net_p2p.html", null ],
      [ "Uvmm, the virtual machine monitor", "l4re_servers_uvmm.html", [
        [ "RAM configuration", "l4re_servers_uvmm_ram_details.html", null ]
      ] ],
      [ "Mag, the GUI Multiplexer", "l4re_servers_mag.html", null ],
      [ "Cons, the Console Multiplexer", "l4re_servers_cons.html", null ]
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
"classL4Re_1_1Util_1_1Smart__count__cap.html",
"classL4_1_1Basic__registry.html",
"classL4_1_1Ipc_1_1Istream.html#af4d28aa64235c822f624891154109a0c",
"classL4_1_1Registry__iface.html#a97645d010b6d832e2914b65061e55b71",
"classL4vbus_1_1Icu.html#a836655616e1572d6e5453a566c6d6372",
"classL4virtio_1_1Virtqueue.html#ad569bf985c1849caadd7fd7c15d75cec",
"classcxx_1_1Bits_1_1Bst.html#ab86f783914505fd36cf3abddda981d4a",
"dir_2ac49572ec7562e26d3ea1810f9b7733.html",
"globals_n.html",
"group__api__l4shmc__signal.html",
"group__l4__ipc__api.html#ggaecfdf101783a02772b67b94793f3a915a57ba98e147a7c3a2fc1a8c965318c7dd",
"group__l4__protocol__ops.html#gga98d28c4c873ee732f4217c8eb1ba5920a8849ab6238f6334ae907c8a7e5bfb9c3",
"group__l4__vm__vmx__api.html#ggac5a7a61aec9e350c19bd87420f37e0acaf7a9ade10e34f67119a63fbacc3162fb",
"group__l4util__elf.html#gga1abf4ecfed17127c9af06412e31aea0eadf7e5d3a23da6b83e744ef69c1fd263f",
"group__l4util__elf.html#ggac8d0eceebc222b45f2406bd1273aae84a2251f476f0fb75667072523c0e13f7ca",
"group__l4virtio__transport.html#gga4783b17d00ec949f63c11c7bda0eed07a8927996ae9cb218bf390cb0fb058902f",
"l4re_servers_ned.html#l4re_ned_options",
"scheduler_8h.html",
"structL4Re_1_1Video_1_1Goos_1_1Info.html#aa67aae51970e347752970cc42851b0ee",
"structL4_1_1Typeid_1_1Detail_1_1__Rpcs_3_01OPCODE_00_01O_00_01R_00_01X_8_8_8_01_4.html",
"structl4__exc__regs__t.html#a3a0d58ca8a29bc67e3f8aab588a01e80",
"structl4re__env__cap__entry__t.html#a2027e219842d2e7f2bca77b5839d07b4",
"uart__imx_8h_source.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';