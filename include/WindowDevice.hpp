#ifndef WINDOW_DEVICE_HPP
#define WINDOW_DEVICE_HPP

#include <esp_err.h>
#include <esp_matter.h>
#include <hal/gpio_types.h>

#include <BaseDevice.hpp>
#include <BlindAccessory.hpp>
#include <cstdint>

/**
 * @class WindowDevice
 * @brief This class represents a Window Device, inheriting from the BaseDevice class.
 *
 * The WindowDevice class encapsulates the behavior of a window accessory, providing
 * methods to interact with the hardware (window and button) and the ESP Matter framework.
 */
class WindowDevice : public BaseDevice {
 public:
  /**
   * @brief Constructor for WindowDevice.
   *
   * This constructor initializes the WindowDevice with specified parameters.
   *
   * @param device_name The name of the device.
   * @param motor_open_pin The GPIO pin connected to the window open motor. Default is GPIO_NUM_NC.
   * @param motor_close_pin The GPIO pin connected to the window close motor. Default is GPIO_NUM_NC.
   * @param button__open_pin The GPIO pin connected to the open button. Default is GPIO_NUM_NC.
   * @param button_close_pin The GPIO pin connected to the close button. Default is GPIO_NUM_NC.
   * @param time_to_open The time it takes to open the window in seconds. Default is 30.
   * @param time_to_close The time it takes to close the window in seconds. Default is 30.
   * @param aggregator The endpoint aggregator. Default is nullptr.
   *
   * @details The constructor creates a WindowAccessory instance with the specified window and button pins.
   * It also sets up the callback for reporting attributes.
   * If an aggregator is provided, it creates a bridged node endpoint with the specified name.
   * If no name is provided, it creates a bridged node endpoint with a default name.
   * If no aggregator is provided, it creates a standalone WindowDevice.
   */
  WindowDevice(const char *device_name = nullptr, gpio_num_t motor_open_pin = GPIO_NUM_NC,
               gpio_num_t motor_close_pin = GPIO_NUM_NC, gpio_num_t button_open_pin = GPIO_NUM_NC,
               gpio_num_t button_close_pin = GPIO_NUM_NC, uint16_t time_to_open = 30,
               uint16_t time_to_close = 30, esp_matter::endpoint_t *aggregator = nullptr);

  /**
   * @brief Default destructor for WindowDevice.
   */
  ~WindowDevice() = default;

  /**
   * @brief Update the accessory state.
   *
   * This method updates the state of the window accessory based on the endpoint's state.
   *
   * @return esp_err_t Error code indicating success or failure.
   */
  esp_err_t updateAccessory() override;

  /**
   * @brief Report the endpoint state.
   *
   * This method reports the current state of the endpoint to the Matter framework.
   *
   * @return esp_err_t Error code indicating success or failure.
   */
  esp_err_t reportEndpoint() override;

  /**
   * @brief Identify the device.
   *
   * This method identifies the device by opening and closing the window.
   *
   * @return esp_err_t Error code indicating success or failure.
   */
  esp_err_t identify() override;

 private:
  uint16_t getAccessoryCurrentPosition();
  uint16_t getAccessoryTargetPosition();
  void setAccessoryTargetPosition(uint16_t position);
  uint16_t getEndpointTargetPosition();
  void setEndpointTargetPosition(uint16_t position);
  void setEndpointCurrentPosition(uint16_t position);

  esp_matter::endpoint_t *endpoint; /**< Pointer to the esp_matter endpoint. */
  WindowAccessory *BlindAccessory;  /**< Window accessory instance. */
  uint16_t time_to_open;            /**< Time it takes to open the window in seconds. */
  uint16_t time_to_close;           /**< Time it takes to close the window in seconds. */
  char name[64];                    /**< Name of the device. TODO: Change to a define. */
};
#endif  // WINDOW_DEVICE_HPP