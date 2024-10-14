var group__cxx__ipc__server =
[
    [ "L4::Ipc_svr", "namespaceL4_1_1Ipc__svr.html", null ],
    [ "L4::Server_object", "classL4_1_1Server__object.html", [
      [ "dispatch", "classL4_1_1Server__object.html#aae08d8562484fc0f1ad5685cdd3efecd", null ],
      [ "dispatch", "classL4_1_1Server__object.html#a505f2d9c6fc2794613243764095b0cbe", null ]
    ] ],
    [ "L4::Server_object_t< IFACE, BASE >", "structL4_1_1Server__object__t.html", [
      [ "Interface", "structL4_1_1Server__object__t.html#a5248a0aa7fc0d96f51b65bb0eed7334c", null ],
      [ "dispatch_meta_request", "structL4_1_1Server__object__t.html#a19c29c3b617648ab2e10bdf9c1d13604", null ],
      [ "get_buffer_demand", "structL4_1_1Server__object__t.html#abbba74affbdf1da97e8992524976073d", null ]
    ] ],
    [ "L4::Server_object_x< Derived, IFACE, BASE >", "structL4_1_1Server__object__x.html", [
      [ "dispatch", "structL4_1_1Server__object__x.html#a8ef4af4e10f927a2957b4f61e6c2b3fd", null ]
    ] ],
    [ "L4::Irq_handler_object", "structL4_1_1Irq__handler__object.html", null ],
    [ "L4::Ipc_svr::Timeout", "classL4_1_1Ipc__svr_1_1Timeout.html", [
      [ "Timeout", "classL4_1_1Ipc__svr_1_1Timeout.html#a75c6f0ec66dd0453fe973508852366de", null ],
      [ "~Timeout", "classL4_1_1Ipc__svr_1_1Timeout.html#a5d9df4a6e0c9645bf4d0f194fa57001a", null ],
      [ "expired", "classL4_1_1Ipc__svr_1_1Timeout.html#a0309d30e94f93df7a08ab855d59c61fe", null ],
      [ "timeout", "classL4_1_1Ipc__svr_1_1Timeout.html#aa4167e97cc10583fe7107ff1e9c3f2aa", null ]
    ] ],
    [ "L4::Ipc_svr::Timeout_queue", "classL4_1_1Ipc__svr_1_1Timeout__queue.html", [
      [ "Timeout", "classL4_1_1Ipc__svr_1_1Timeout__queue.html#a2c17bae1e09ef3a565a994acb96fd165", null ],
      [ "add", "classL4_1_1Ipc__svr_1_1Timeout__queue.html#ae31fed715901774759e1786b5e633a07", null ],
      [ "handle_expired_timeouts", "classL4_1_1Ipc__svr_1_1Timeout__queue.html#a773004f67897fe95749d801eb19a95e5", null ],
      [ "next_timeout", "classL4_1_1Ipc__svr_1_1Timeout__queue.html#a4d3404d1f551f06f9ef98121bd150569", null ],
      [ "remove", "classL4_1_1Ipc__svr_1_1Timeout__queue.html#a172e0ca0f7fcb845b75939c4f30ceebb", null ],
      [ "timeout_expired", "classL4_1_1Ipc__svr_1_1Timeout__queue.html#aaac5ada3be0b3cd1749b89c79bb22342", null ]
    ] ],
    [ "L4::Ipc_svr::Timeout_queue_hooks< HOOKS, BR_MAN >", "classL4_1_1Ipc__svr_1_1Timeout__queue__hooks.html", [
      [ "add_timeout", "classL4_1_1Ipc__svr_1_1Timeout__queue__hooks.html#a07277e39d7a77e66fbd6556f00e3e8aa", null ],
      [ "before_reply", "classL4_1_1Ipc__svr_1_1Timeout__queue__hooks.html#a27b6731396dc52f3fd5bf35f6788e27b", null ],
      [ "remove_timeout", "classL4_1_1Ipc__svr_1_1Timeout__queue__hooks.html#a559c5881cc36b16338904749c4578ae9", null ],
      [ "setup_wait", "classL4_1_1Ipc__svr_1_1Timeout__queue__hooks.html#aa2441ebaedd9d9a3c3e7066d2696ba2f", null ],
      [ "timeout", "classL4_1_1Ipc__svr_1_1Timeout__queue__hooks.html#ae4a6bd9781bf97a37dbff039db58d47d", null ],
      [ "queue", "classL4_1_1Ipc__svr_1_1Timeout__queue__hooks.html#aedb498336d49a3407de48084ce24f029", null ]
    ] ],
    [ "L4::Ipc_svr::Server_iface", "classL4_1_1Ipc__svr_1_1Server__iface.html", [
      [ "Demand", "classL4_1_1Ipc__svr_1_1Server__iface.html#a06e47dba2ba71662337758f0cbf37053", null ],
      [ "Server_iface", "classL4_1_1Ipc__svr_1_1Server__iface.html#aa17aa10ace494b229a3e0879599e23b9", null ],
      [ "add_timeout", "classL4_1_1Ipc__svr_1_1Server__iface.html#a7bd56bcdfd81c0be0654492e3016d7eb", null ],
      [ "alloc_buffer_demand", "classL4_1_1Ipc__svr_1_1Server__iface.html#ae37a0cf49bd7a6731bfba6140a0a30b8", null ],
      [ "get_rcv_cap", "classL4_1_1Ipc__svr_1_1Server__iface.html#ac05a332b8952f8fa6af173f3c90eb3d2", null ],
      [ "rcv_cap", "classL4_1_1Ipc__svr_1_1Server__iface.html#ade41083183a509c9cefe6663a2af7b97", null ],
      [ "rcv_cap", "classL4_1_1Ipc__svr_1_1Server__iface.html#adbf5b2414f63b348de3a4005c4164e47", null ],
      [ "realloc_rcv_cap", "classL4_1_1Ipc__svr_1_1Server__iface.html#a188d4ff607f72e84f3149de3351aea5c", null ],
      [ "remove_timeout", "classL4_1_1Ipc__svr_1_1Server__iface.html#a8eb77524e90f6561eca9ccc29a97095a", null ]
    ] ],
    [ "L4::Basic_registry", "classL4_1_1Basic__registry.html", null ],
    [ "L4::Ipc_svr::Ignore_errors", "structL4_1_1Ipc__svr_1_1Ignore__errors.html", null ],
    [ "L4::Ipc_svr::Default_timeout", "structL4_1_1Ipc__svr_1_1Default__timeout.html", null ],
    [ "L4::Ipc_svr::Compound_reply", "structL4_1_1Ipc__svr_1_1Compound__reply.html", null ],
    [ "L4::Ipc_svr::Default_setup_wait", "structL4_1_1Ipc__svr_1_1Default__setup__wait.html", null ],
    [ "L4::Ipc_svr::Br_manager_no_buffers", "classL4_1_1Ipc__svr_1_1Br__manager__no__buffers.html", [
      [ "add_timeout", "classL4_1_1Ipc__svr_1_1Br__manager__no__buffers.html#a92ee65f7a387ad6c3b335904c6f4d7cb", null ],
      [ "alloc_buffer_demand", "classL4_1_1Ipc__svr_1_1Br__manager__no__buffers.html#a48d7b04f600248f991ec2967bfc85138", null ],
      [ "first_free_br", "classL4_1_1Ipc__svr_1_1Br__manager__no__buffers.html#a6a21ac56918c605ca9796c99edfd8ec0", null ],
      [ "get_rcv_cap", "classL4_1_1Ipc__svr_1_1Br__manager__no__buffers.html#a06b75be0f30872bcb6d2e5d43e9e53c4", null ],
      [ "realloc_rcv_cap", "classL4_1_1Ipc__svr_1_1Br__manager__no__buffers.html#a8253d0dd0fb677174c6747383713f37f", null ],
      [ "remove_timeout", "classL4_1_1Ipc__svr_1_1Br__manager__no__buffers.html#a212e3c2e826b36125d0311ff575de5fc", null ],
      [ "setup_wait", "classL4_1_1Ipc__svr_1_1Br__manager__no__buffers.html#ae41eef8a74be7b2d9ce4472bf0204204", null ]
    ] ],
    [ "L4::Ipc_svr::Default_loop_hooks", "structL4_1_1Ipc__svr_1_1Default__loop__hooks.html", null ],
    [ "L4::Server< LOOP_HOOKS >", "classL4_1_1Server.html", [
      [ "Server", "classL4_1_1Server.html#aefd92e70b235771898428fc98675b96e", null ],
      [ "Server", "classL4_1_1Server.html#a4d2db3d761228ed6ed282f20f6d4da9d", null ],
      [ "internal_loop", "classL4_1_1Server.html#a59c8957072a390acf8d8557021c67059", null ],
      [ "loop", "classL4_1_1Server.html#a81de12837c9c5bb6e0a11f2338341934", null ],
      [ "loop_noexc", "classL4_1_1Server.html#ae30bfa9ee9facb019e348bdac2779eea", null ],
      [ "reply_n_wait", "classL4_1_1Server.html#a21578e92d497db8ab2a2a88b356fa427", null ]
    ] ],
    [ "L4::Ipc_svr::Reply_mode", "group__cxx__ipc__server.html#ga53acccae8e5dc9803ebb8583e1e9de30", [
      [ "L4::Ipc_svr::Reply_compound", "group__cxx__ipc__server.html#gga53acccae8e5dc9803ebb8583e1e9de30afec233db6432949dbc89dfda6cb03294", null ],
      [ "L4::Ipc_svr::Reply_separate", "group__cxx__ipc__server.html#gga53acccae8e5dc9803ebb8583e1e9de30a591e182e116c067db8a166dd8ec5b2d3", null ]
    ] ]
];