#ifndef FAN_DEVICE_HPP
#define FAN_DEVICE_HPP

#include <BaseDevice.hpp>
#include <hal/gpio_types.h>
#include <fanAccessory.hpp>
#include <esp_err.h>
#include <cstdint>
#include <esp_matter.h>

/**
 * @class FanDevice
 * @brief This class represents a Fan Device, inheriting from the BaseDevice class.
 *
 * The FanDevice class encapsulates the behavior of a fan accessory, providing
 * methods to interact with the hardware (fan and button) and the ESP Matter framework.
 */
class FanDevice : public BaseDevice
{
public:
    /**
     * @brief Constructor for FanDevice.
     *
     * This constructor initializes the FanDevice with specified parameters.
     *
     * @param device_name The name of the device.
     * @param fan_pin The GPIO pin connected to the fan. Default is GPIO_NUM_NC.
     * @param button_pin The GPIO pin connected to the button. Default is GPIO_NUM_NC.
     * @param aggregator The endpoint aggregator. Default is nullptr.
     */
    FanDevice(char *device_name = nullptr, gpio_num_t fan_pin = GPIO_NUM_NC, gpio_num_t button_pin = GPIO_NUM_NC, esp_matter::endpoint_t *aggregator = nullptr);

    /**
     * @brief Default destructor for FanDevice.
     */
    ~FanDevice() = default;

    /**
     * @brief Update the accessory state.
     *
     * This method updates the state of the fan accessory based on the endpoint's state.
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
     * @brief Identify the FanDevice.
     *
     * This method triggers the identification process for the FanDevice, typically involving
     * visual or audible signals from the fan accessory.
     *
     * @return esp_err_t Error code indicating success or failure.
     */
    esp_err_t identify() override;

private:
    /**
     * @brief Get the power state of the accessory.
     *
     * This method retrieves the current power state of the fan accessory.
     *
     * @return bool Power state of the accessory (true for on, false for off).
     */
    bool getAccessoryPowerState();

    /**
     * @brief Set the power state of the accessory.
     *
     * This method sets the power state of the fan accessory.
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

    esp_matter::endpoint_t *endpoint; /**< Pointer to the esp_matter endpoint. */
    FanAccessory *fanAccessory;       /**< Pointer to the FanAccessory instance. */
    char name[64];                    /**< Name of the device, TODO: change to a define. */
};

#endif // FAN_DEVICE_HPP
