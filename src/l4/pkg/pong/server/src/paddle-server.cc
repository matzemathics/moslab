#include "paddle-server.h"

#include <l4/cxx/iostream>
#include <l4/cxx/l4iostream>


static Paddle_server *the_paddle_server;

template <typename T>
T
extract(L4::Ipc::Istream &i)
{
  T v;
  i >> v;
  return v;
}


int
Paddle_so::dispatch(l4_umword_t obj, L4::Ipc::Iostream &ios)
{
  unsigned long op;
  ios >> op;
  switch (op)
    {
    case 1:
      move_to(extract<int>(ios));
      return L4_EOK;
    case 2:
      set_lifes(extract<int>(ios));
      return L4_EOK;
    case 3:
      ios << get_lifes();
      return L4_EOK;
    default:
      L4::cout << "Invalid request for " << obj << '\n';
      return -L4_ENOREPLY;
    }
}

int
Paddle_server::dispatch(l4_umword_t obj, L4::Ipc::Iostream &ios)
{
  unsigned long op;
  ios >> op;
  switch (op)
    {
    case 1:
      ios << connect();
      return L4_EOK;
    default:
      L4::cout << "Invalid request for " << obj << '\n';
      return -L4_ENOREPLY;
    }
}

Paddle_server::Paddle_server(Env &env, Paddle paddles[4])
: L4::PosixThread(), env(env)
{
  _pad[0].set_paddle(&paddles[0]);
  _pad[1].set_paddle(&paddles[1]);

  _pad[0].paddle()->add(0, 60);
  _pad[0].paddle()->add(16, 60);
  _pad[0].paddle()->add(18, 55);
  _pad[0].paddle()->add(20, 50);
  _pad[0].paddle()->add(21, 40);
  _pad[0].paddle()->add(21, 20);
  _pad[0].paddle()->add(20, 10);
  _pad[0].paddle()->add(18, 5);
  _pad[0].paddle()->add(16, 0);
  _pad[0].paddle()->add(0, 0);

  _pad[1].paddle()->add(20, 60);
  _pad[1].paddle()->add(20, 0);
  _pad[1].paddle()->add(4, 0);
  _pad[1].paddle()->add(2, 5);
  _pad[1].paddle()->add(0, 10);
  _pad[1].paddle()->add(-1, 20);
  _pad[1].paddle()->add(-1, 40);
  _pad[1].paddle()->add(0, 50);
  _pad[1].paddle()->add(2, 55);
  _pad[1].paddle()->add(4, 60);
}

void
Paddle_server::run()
{
  L4::cerr << "------------------------------------------------------\n";
  Server server(self(), L4Re::Env::env()->factory());
  server.registry()->register_obj(this, "PongServer");
  server.registry()->register_obj(&_pad[0]);
  server.registry()->register_obj(&_pad[1]);
  the_paddle_server = this;

  L4::cerr << "Hello from svrloop\n";
  server.loop(l4_utcb());
}

void
Paddle_server::handle_collision(Obstacle *o)
{
  for (unsigned i = 0; i < 2; ++i)
    if (_pad[i].connected() && o == _pad[i].paddle()->wall())
      _pad[i].dec_lifes();
}

L4::Cap<void>
Paddle_server::connect()
{
  for (unsigned i = 0; i < 2; ++i)
    {
      if (!_pad[i].connected())
        {
          _pad[i].connect(true);
          env.add(_pad[i].paddle());

          L4::cout << "MAP cap: " << _pad[i].obj_cap() << "\n";

          return _pad[i].obj_cap();
        }
    }

  return L4::Cap<void>::Invalid;
}

void
Paddle_so::move_to(int pos)
{
  if (pos < 0 || pos > 1023)
    return;

  paddle()->move(0, pos * 420 / 1024);
}


int *
Paddle_so::lifes()
{
  return &_points;
}

void
Paddle_so::set_lifes(int _lifes)
{
  int *l = lifes();
  if (l)
    *l = _lifes;
}

int
Paddle_so::get_lifes()
{
  int *l = lifes();
  if (l)
    return *l;

  return -5000;
}
