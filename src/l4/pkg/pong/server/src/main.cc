#include <l4/pong/logging.h>
#include <l4/sys/factory.h>
#include <l4/sys/kip.h>
#include <l4/util/util.h>

#include "ball.h"
#include "env.h"
#include "gfx-drv.h"
#include "paddle-server.h"

#include <l4/cxx/iostream>
#include <l4/cxx/ipc_server>
#include <l4/cxx/l4iostream>

#include <iostream>

// offscreen buffer for drawing operations
Gfx<Screen_buffer> screen;
// real frame buffer
Gfx<Screen> real_screen;

// Instanciation of the ball
Ball ball(screen.color(0xff, 0xf0, 0x40), 0);
// Instanciation of the play field
Env env(screen, real_screen, ball);

// The playfield boundaries
Line ll(10, screen.height() - 2, 10, 2, false);
Line lt(2, 2, screen.width() - 2, 2, false);
Line lr(screen.width() - 10, 2, screen.width() - 10, screen.height() - 2,
        false);
Line lb(screen.width() - 2, screen.height() - 2, 2, screen.height() - 2, false);

Paddle paddles[] = {Paddle(0, 0, &ll), Paddle(620, 0, &lr), Paddle(0, 0, &lt),
                    Paddle(0, 460, &lb)};

// Instanciation of the paddle server (own thread)
Paddle_server paddle_server(env, paddles);

// Main server function
int
main()
{
  redirect_to_log(std::cout);
  redirect_to_log(std::cerr);

  L4::cout << "PS Hello here am I\n";

  L4::cout << "KIP @ " << l4re_kip() << ": " << L4::n_hex(l4re_kip()->magic)
           << '\n';

  // add the boundaries to the playfield
  env.add(&ll);
  env.add(&lt);
  env.add(&lr);
  env.add(&lb);

  // start the paddle server (thread)
  paddle_server.start();

  Obstacle *o;

  // main loop
  while (1)
    {
      // draw the complete stuff (becomes visible on frame buffer)
      env.draw();

      // handle collisions
      o = env.collide();

      if (o)
        paddle_server.handle_collision(o);
#if 0
      if (o==&lpad)
  L4::cout << "Left Points: " << ++lpp << '\n';
      if (o==&rpad)
  L4::cout << "Right Points: " << ++rpp << '\n';
#endif
      // move the ball one step
      ball.move();
      l4_sleep(8);
    };
}
