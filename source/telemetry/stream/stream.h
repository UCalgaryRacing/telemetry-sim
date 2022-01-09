/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include "../thing/sensor.h"
#include "channel.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <mutex>
#include <functional>
#include <chrono>
#include <thread>

/**
 * @brief Data publication callback; used to receive variable frequency
 * channel data from a Stream object. Subscribe and unsubscribe callbacks
 * with Stream::subscribe, Stream::subscribe. Passes a timestamp to specify
 * the time of the data. 
 */
using ReadCallback = std::function<void(unsigned long long, std::vector<SensorVariantPair>)>;

/**
 * @brief TODO - write stuff
 */
class Stream {
private:
  std::unordered_map<unsigned int, AbstractChannel*> _channels;
  std::unordered_map<unsigned int, SensorDataVariant> _stream_buffer;
  std::unordered_map<unsigned int, ReadCallback> _callbacks;
  volatile unsigned long long _timestamp = 0;
  unsigned int _frequency = 0;

  std::thread _read_thread;
  std::mutex _lock;
  bool _paused = true;
  bool _closed = true;

  /**
   * @brief An internal method that reads from channels at the highest 
   * channel frequency and publishes data to subscribers. Run via 
   * internal _read_thread. 
   * The function uses a spinlock and is started when Stream::open 
   * is called. The function is exited when Stream::close is called. 
   * The stream is paused and unpaused via Stream::pause, Stream::unpause. 
   */
  void _tap_channels() noexcept;

public: 
  Stream(std::unordered_map<unsigned char, Sensor> &sensors);
  ~Stream();

  /**
   * @brief Subscribe to the stream with a callback using an id. 
   * Returns false if the given id already has a subscriber. 
   * Returns true if the callback is bound successfully. 
   */
  bool subscribe(unsigned int id, ReadCallback callback) noexcept;

  /**
   * @brief Unsubscribe the callback associated with the provided ID.
   * Returns false if the provided id does not match with a callback. 
   * Returns true if the provided id's callback was unbound.
   */
  bool unsubscribe(unsigned int id) noexcept;

  /**
   * @brief Opens the stream to start reading from channel. 
   * Sets the pause state of the stream to false. 
   */
  void open() noexcept;

  /**
   * @brief Closes the stream and stops reading from channels. 
   * Sets the pause state of the stream to true.
   */
  void close() noexcept;

  /**
   * @brief Pauses the stream if it is open. 
   * Channels stop updating their data. 
   */
  void pause() noexcept;

  /**
   * @brief Unpauses the stream if it is open. 
   * Channels start where they left off. 
   */
  void unpause() noexcept;
};