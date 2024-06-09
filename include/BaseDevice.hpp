#ifndef BASE_DEVICE_HPP
#define BASE_DEVICE_HPP

#include <esp_err.h>
#include <cstdint>

/**
 * @class BaseDevice
 * @brief Abstract base class for all device types.
 *
 * The BaseDevice class defines the interface that all device classes must implement.
 * It provides pure virtual methods for updating the accessory state, reporting the
 * endpoint state, and identifying the device.
 */
class BaseDevice
{
public:
    /**
     * @brief Virtual destructor for BaseDevice.
     *
     * Ensures derived class destructors are called correctly.
     */
    virtual ~BaseDevice() = default;

    /**
     * @brief Update the accessory state.
     *
     * This pure virtual method must be implemented by derived classes to update
     * the state of the accessory.
     *
     * @return esp_err_t Error code indicating success or failure.
     */
    virtual esp_err_t updateAccessory() = 0;

    /**
     * @brief Report the endpoint state.
     *
     * This pure virtual method must be implemented by derived classes to report
     * the current state of the endpoint.
     *
     * @return esp_err_t Error code indicating success or failure.
     */
    virtual esp_err_t reportEndpoint() = 0;

    /**
     * @brief Identify the device.
     *
     * This pure virtual method must be implemented by derived classes to trigger
     * the identification process for the device.
     *
     * @return esp_err_t Error code indicating success or failure.
     */
    virtual esp_err_t identify() = 0;
};

#endif // BASE_DEVICE_HPP
