#ifndef LIGHT_DEVICE_HPP
#define LIGHT_DEVICE_HPP

#include <esp_err.h>
#include <esp_matter.h>

#include <BaseDevice.hpp>
#include <LightAccessoryInterface.hpp>
#include <cstdint>

/**
 * @class LightDevice
 * @brief This class represents a Light Device, inheriting from the BaseDevice class.
 *
 * The LightDevice class encapsulates the behavior of a light accessory, providing
 * methods to interact with the hardware (light and button) and the ESP Matter framework.
 */
class LightDevice : public BaseDevice {
 public:
  /**
   * @brief Constructor for LightDevice.
   *
   * This constructor initializes the LightDevice with specified parameters.
   *
   * @param device_name The name of the device.
   * @param light_pin The GPIO pin connected to the light. Default is GPIO_NUM_NC.
   * @param button_pin The GPIO pin connected to the button. Default is GPIO_NUM_NC.
   * @param aggregator The endpoint aggregator. Default is nullptr.
   *
   * @details The constructor creates a LightAccessory instance with the specified light and button pins.
   * It also sets up the callback for reporting attributes.
   * If an aggregator is provided, it creates a bridged node endpoint with the specified name.
   * If no name is provided, it creates a bridged node endpoint with a default name.
   * If no aggregator is provided, it creates a standalone LightDevice.
   */
  LightDevice(const char *device_name = nullptr, LightAccessoryInterface *lightAccessory = nullptr,
              esp_matter::endpoint_t *aggregator = nullptr);

  /**
   * @brief Default destructor for LightDevice.
   */
  ~LightDevice() = default;

  /**
   * @brief Update the accessory state.
   *
   * This method updates the state of the light accessory based on the endpoint's state.
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
   * @brief Identify the LightDevice.
   *
   * This method triggers the identification process for the LightDevice, typically involving
   * visual or audible signals from the light accessory.
   *
   * @return esp_err_t Error code indicating success or failure.
   */
  esp_err_t identify() override;

 private:
  /**
   * @brief Get the power state of the accessory.
   *
   * This method retrieves the current power state of the light accessory.
   *
   * @return bool Power state of the accessory (true for on, false for off).
   */
  bool getAccessoryPowerState();

  /**
   * @brief Set the power state of the accessory.
   *
   * This method sets the power state of the light accessory.
   *
   * @param powerState Power state to set (true for on, false for off).
   */
  void setAccessoryPowerState(bool powerState);

  /**
   * @brief Get the power state of the endpoint.
   *
   * This method retrieves the current power state of the endpoint from the Matter framework.
   *
   * @return bool Power state of the endpoint (true for on, false for off).
   */
  bool getEndpointPowerState();

  /**
   * @brief Set the power state of the endpoint.
   *
   * This method sets the power state of the endpoint and reports it to the Matter framework.
   *
   * @param powerState Power state to set (true for on, false for off).
   */
  void setEndpointPowerState(bool powerState);

  esp_matter::endpoint_t *endpoint;        /**< Pointer to the esp_matter endpoint. */
  LightAccessoryInterface *lightAccessory; /**< Pointer to the LightAccessory instance. */
  char name[64];                           /**< Name of the device, TODO: change to a define. */
};

#endif  // LIGHT_DEVICE_HPP
