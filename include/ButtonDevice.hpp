#ifndef BUTTON_DEVICE_HPP
#define BUTTON_DEVICE_HPP

#include <esp_err.h>
#include <esp_matter.h>
#include <hal/gpio_types.h>

#include <BaseDevice.hpp>
#include <StatelessButtonAccessoryInterface.hpp>
#include <cstdint>

/**
 * @class ButtonDevice
 * @brief This class represents a Button Device, inheriting from the BaseDevice class.
 *
 * The ButtonDevice class encapsulates the behavior of a button accessory, providing
 * methods to interact with the hardware (button) and the ESP Matter framework.
 */
class ButtonDevice : public BaseDevice {
 public:
  /**
   * @brief Constructor for ButtonDevice.
   *
   * This constructor initializes the ButtonDevice with specified parameters.
   *
   * @param device_name The name of the device.
   * @param button_pin The GPIO pin connected to the button. Default is GPIO_NUM_NC.
   * @param aggregator The endpoint aggregator. Default is nullptr.
   *
   * @details The constructor creates a SwitchButtonAccessory instance with the specified button pin.
   * It also sets up the callback for reporting attributes.
   * If an aggregator is provided, it creates a bridged node endpoint with the specified name.
   * If no name is provided, it creates a bridged node endpoint with a default name.
   * If no aggregator is provided, it creates a standalone ButtonDevice.
   */
  ButtonDevice(const char *device_name = nullptr, StatelessButtonAccessoryInterface *buttonAccessory = nullptr,
               esp_matter::endpoint_t *aggregator = nullptr);

  /**
   * @brief Default destructor for ButtonDevice.
   */
  ~ButtonDevice() = default;

  /**
   * @brief Update the accessory state.
   *
   * This method updates the state of the button accessory based on the endpoint's state.
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
   * @brief Identify the accessory.
   *
   * @return esp_err_t Error code indicating success or failure.
   */
  esp_err_t identify() override;

 private:
  // bool getAccessoryPowerState();
  // void setAccessoryPowerState(bool powerState);
  // bool getEndpointPowerState();
  void setEndpointSwitchPressEvent(StatelessButtonAccessoryInterface::PressType pressType);

  esp_matter::endpoint_t *endpoint; /**< Pointer to the esp_matter endpoint. */
  StatelessButtonAccessoryInterface
      *switchButtonAccessory; /**< Pointer to the SwitchButtonAccessory instance. */
  char name[64];              /**< Name of the device, TODO: change to a define. */
};

#endif  // BUTTON_DEVICE_HPP