#include "Ctrl.h"

using namespace ctrl;

Encoder::Encoder(uint8_t stable_state, State_Reader reader) {
  this->stable_state = stable_state;
  this->prev_unstable_state = stable_state;
  this->reader = reader;
  this->unstable_signal = 0;
  this->is_stable_state = true;
  this->is_new_unstable_state = false;
  this->tick = false;
  this->positive_tick = false;
  this->negative_tick = false;
}

void Encoder::listen() {
  static Encoder_Event event;

  this->read();

  if (this->tick) {
    event.positive_tick = this->positive_tick;
    event.negative_tick = this->negative_tick;

    if (this->handlers.rotate) {
      this->handlers.rotate(&event);
    }
  }
}

void Encoder::on(const char *event_name, Encoder_Handler callback) {
  if (strcmp(event_name, "rotate") == 0) {
    this->handlers.rotate = callback;

    return;
  }
}

void Encoder::reset_stable_state() {
  if (this->is_stable_state) {
    this->is_stable_state = false;
  }
}

void Encoder::set_unstable_signal(uint8_t state) {
  if (!this->is_stable_state && state != this->prev_unstable_state) {
    this->prev_unstable_state = state;
    this->unstable_signal <<= 2;
    this->unstable_signal += state;
  }
}

void Encoder::reset_tick() {
  this->tick = false;
  this->prev_unstable_state = this->stable_state;
  this->unstable_signal = 0;
  this->positive_tick = false;
  this->negative_tick = false;
}

void Encoder::set_tick() {
  this->tick = true;
  this->is_stable_state = true;

  if (this->unstable_signal == 0x12) {
    this->negative_tick = false;
    this->positive_tick = true;
  }

  if (this->unstable_signal == 0x21) {
    this->positive_tick = false;
    this->negative_tick = true;
  }
}

bool Encoder::is_unstable_state(uint8_t state) {
  return state != this->stable_state;
}

bool Encoder::is_new_tick(uint8_t state) {
  return !this->is_stable_state && state == this->stable_state;
}

bool Encoder::is_same_tick(uint8_t state) {
  return this->is_stable_state && state == this->stable_state && this->tick;
}

void Encoder::read() {
  const uint8_t state = this->reader();

  if (this->is_unstable_state(state)) {
    this->reset_stable_state();
    this->set_unstable_signal(state);

    return;
  }

  if (this->is_same_tick(state)) {
    this->reset_tick();

    return;
  }

  if (this->is_new_tick(state)) {
    this->set_tick();

    return;
  }
}
