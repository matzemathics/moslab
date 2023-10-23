#ifndef PADDLE_THREAD_H__
#define PADDLE_THREAD_H__

#include "env.h"
#include "obj_reg.h"
#include "paddle.h"

#include <l4/cxx/ipc_server>
#include <l4/sys/capability>

#include "PosixThread.h"

struct Paddle_if : L4::Kobject_t<Paddle_if, L4::Kobject>
{
};
class Paddle_so : public L4::Server_object_t<Paddle_if>
{
public:
  explicit Paddle_so(Paddle *_pad = 0)
  : _points(0), _py(0), _paddle(_pad), _connected(0)
  {
  }
  int dispatch(l4_umword_t obj, L4::Ipc::Iostream &ios);

  void move_to(int pos);
  void set_lifes(int lifes);
  int get_lifes();

  void
  set_paddle(Paddle *pad)
  {
    _paddle = pad;
  }
  Paddle *
  paddle() const
  {
    return _paddle;
  }

  bool
  connected() const
  {
    return _connected;
  }
  void
  connect(bool c = true)
  {
    _connected = c;
  }

  void
  dec_lifes()
  {
    --_points;
  }

private:
  int *lifes();

  int _points;
  int _py;
  Paddle *_paddle;
  bool _connected;
};


struct Paddle_server_if : L4::Kobject_t<Paddle_server_if, L4::Kobject>
{
};
class Paddle_server : public L4::PosixThread,
                      public L4::Server_object_t<Paddle_server_if>
{
private:
  typedef L4Re::Util::Registry_server<> Server;

public:
  Paddle_server(Env &env, Paddle paddles[4]);

  void run();

  int dispatch(l4_umword_t obj, L4::Ipc::Iostream &ios);

  L4::Cap<void> connect();

  void handle_collision(Obstacle *o);

private:
  char str[60];

  Paddle_so _pad[2];

  Env &env;
};


#endif // PADDLE_THREAD_H__
