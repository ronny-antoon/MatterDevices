#ifndef PLUG_IN_DEVICE_HPP
#define PLUG_IN_DEVICE_HPP

#include <esp_err.h>
#include <esp_matter.h>
#include <hal/gpio_types.h>

#include <BaseDevice.hpp>
#include <cstdint>
#include <PluginAccessoryInterface.hpp>

/**
 * @class PlugInDevice
 * @brief This class represents a PlugIn Device, inheriting from the BaseDevice class.
 *
 * The PlugInDevice class encapsulates the behavior of a plug-in accessory, providing
 * methods to interact with the hardware (relay and button) and the ESP Matter framework.
 */
class PlugInDevice : public BaseDevice {
 public:
  /**
   * @brief Constructor for PlugInDevice.
   *
   * This constructor initializes the PlugInDevice with specified parameters.
   *
   * @param device_name The name of the device.
   * @param relay_pin The GPIO pin connected to the relay. Default is GPIO_NUM_NC.
   * @param button_pin The GPIO pin connected to the button. Default is GPIO_NUM_NC.
   * @param aggregator The endpoint aggregator. Default is nullptr.
   *
   * @details The constructor creates a PlugInAccessory instance with the specified relay and button pins.
   * It also sets up the callback for reporting attributes.
   * If an aggregator is provided, it creates a bridged node endpoint with the specified name.
   * If no name is provided, it creates a bridged node endpoint with a default name.
   * If no aggregator is provided, it creates a standalone PlugInDevice.
   */
  PlugInDevice(const char *device_name = nullptr, PluginAccessoryInterface *plugInAccessory = nullptr,
               esp_matter::endpoint_t *aggregator = nullptr);

  /**
   * @brief Default destructor for PlugInDevice.
   */
  ~PlugInDevice() = default;

  /**
   * @brief Update the accessory state.
   *
   * This method updates the state of the plug-in accessory based on the endpoint's state.
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
   * This method identifies the plug-in accessory by toggling the relay for a short duration.
   *
   * @return esp_err_t Error code indicating success or failure.
   */
  esp_err_t identify() override;

 private:
  bool getAccessoryPowerState();
  void setAccessoryPowerState(bool powerState);
  bool getEndpointPowerState();
  void setEndpointPowerState(bool powerState);

  esp_matter::endpoint_t *endpoint;    /**< Pointer to the esp_matter endpoint. */
  PluginAccessoryInterface *accessory; /**< Pointer to the PlugInAccessory instance. */
  char name[64];                       /**< Name of the device, TODO: change to a define. */
};

#endif  // PLUG_IN_DEVICE_HPP