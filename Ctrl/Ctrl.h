#include <stdint.h>
#include <string.h>

#ifndef CTRL
#define CTRL

namespace ctrl {
struct Encoder_Event {
  bool positive_tick;
  bool negative_tick;
};

typedef void (*Encoder_Handler)(Encoder_Event const *event);

struct Encoder_Handlers {
  Encoder_Handler rotate;
};

typedef uint8_t (*State_Reader)();

class Encoder {
private:
  uint8_t stable_state, prev_unstable_state, unstable_signal;
  bool is_stable_state, is_new_unstable_state, tick, positive_tick,
      negative_tick;
  Encoder_Handlers handlers;
  State_Reader reader;

  void reset_stable_state();
  void set_unstable_signal(uint8_t);
  void reset_tick();
  void set_tick();
  bool is_unstable_state(uint8_t);
  bool is_new_tick(uint8_t);
  bool is_same_tick(uint8_t);
  void read();

public:
  Encoder(uint8_t, State_Reader);

  void listen();
  void on(const char *event_name, Encoder_Handler);
};
} // namespace ctrl

#endif
