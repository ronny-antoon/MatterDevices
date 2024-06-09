#include "ButtonDevice.hpp"

#include <esp_log.h>
#include <esp_err.h>
#include <cstdint>
#include <esp_matter.h>
#include <esp_matter_endpoint.h>

ButtonDevice::ButtonDevice(char *device_name, gpio_num_t button_pin, esp_matter::endpoint_t *aggregator)
    : BaseDevice()
{
    if (button_pin == GPIO_NUM_NC)
    {
        ESP_LOGW(__FILENAME__, "button_pin is not set");
    }
    else
    {
        ESP_LOGI(__FILENAME__, "Creating SwitchButtonAccessory with button_pin: %d", button_pin);

        // Create the SwitchButtonAccessory instance
        switchButtonAccessory = new SwitchButtonAccessory(button_pin);

        // Set up the callback for reporting attributes
        switchButtonAccessory->setReportAttributesCallback([](void *self)
                                                           { static_cast<ButtonDevice *>(self)->reportEndpoint(); }, this);
    }

    // Check if an aggregator is provided
    if (aggregator != nullptr)
    {
        esp_matter::endpoint::bridged_node::config_t bridged_node_config;
        uint8_t flags = esp_matter::endpoint_flags::ENDPOINT_FLAG_BRIDGE | esp_matter::endpoint_flags::ENDPOINT_FLAG_DESTROYABLE;
        endpoint = esp_matter::endpoint::bridged_node::create(
            esp_matter::node::get(),
            &bridged_node_config,
            flags,
            this);
        if (device_name != nullptr && strlen(device_name) > 0 && strlen(device_name) < 64) // TODO: change to a define
        {
            strncpy(this->name, device_name, sizeof(this->name));
            ESP_LOGI(__FILENAME__, "Creating Bridged Node ButtonDevice with name: %s", name);
            esp_matter::cluster_t *bridge_device_basic_information_cluster = esp_matter::cluster::get(
                endpoint, chip::app::Clusters::BridgedDeviceBasicInformation::Id);
            esp_matter::cluster::bridged_device_basic_information::attribute::create_node_label(
                bridge_device_basic_information_cluster, name, strlen(name));
        }
        else
        {
            ESP_LOGW(__FILENAME__, "device_name is not set");
            ESP_LOGI(__FILENAME__, "Creating Bridged Node ButtonDevice with default name");
        }
        esp_matter::endpoint::set_parent_endpoint(endpoint, aggregator);
    }
    else
    {
        ESP_LOGI(__FILENAME__, "Creating ButtonDevice standalone endpoint");
        uint8_t flags = esp_matter::endpoint_flags::ENDPOINT_FLAG_NONE;
        endpoint = esp_matter::endpoint::create(esp_matter::node::get(), flags, this);
    }

    esp_matter::endpoint::generic_switch::config_t generic_switch_config;
    esp_matter::endpoint::generic_switch::add(endpoint, &generic_switch_config);

    // Add single press feature to the cluster
    esp_matter::cluster_t *switch_cluster = esp_matter::cluster::get(endpoint, chip::app::Clusters::Switch::Id);
    esp_matter::cluster::switch_cluster::feature::momentary_switch::add(switch_cluster);
    esp_matter::cluster::switch_cluster::feature::momentary_switch_release::add(switch_cluster);

    // Add long press feature to the cluster
    esp_matter::cluster::switch_cluster::feature::momentary_switch_long_press::add(switch_cluster);

    // Add double press feature to the cluster
    esp_matter::cluster::switch_cluster::feature::momentary_switch_multi_press::config_t double_press_config;
    esp_matter::cluster::switch_cluster::feature::momentary_switch_multi_press::add(switch_cluster, &double_press_config);
}

esp_err_t ButtonDevice::updateAccessory()
{
    return ESP_OK;
}

esp_err_t ButtonDevice::reportEndpoint()
{
    ESP_LOGI(__FILENAME__, "Reporting ButtonDevice endpoint");

    // Get the last press type
    SwitchButtonAccessory::PressType pressType = switchButtonAccessory->getLastPressType();

    // Report the endpoint state
    setEndpointSwitchPressEvent(pressType);
    return ESP_OK;
}

esp_err_t ButtonDevice::identify()
{
    return ESP_OK;
}

void ButtonDevice::setEndpointSwitchPressEvent(SwitchButtonAccessory::PressType pressType)
{
    uint16_t endpoint_id = esp_matter::endpoint::get_id(endpoint);
    esp_matter::cluster_t *switch_cluster = esp_matter::cluster::get(endpoint, chip::app::Clusters::Switch::Id);
    esp_matter_attr_val_t attr_val = esp_matter_uint8(0);
    esp_matter::attribute::report(
        endpoint_id,
        chip::app::Clusters::Switch::Id,
        chip::app::Clusters::Switch::Attributes::CurrentPosition::Id,
        &attr_val);

    switch (pressType)
    {
    case SwitchButtonAccessory::PressType::SinglePress:
        ESP_LOGW(__FILENAME__, "SinglePress");
        esp_matter::lock::chip_stack_lock(portMAX_DELAY);
        esp_matter::cluster::switch_cluster::event::send_multi_press_complete(
            endpoint_id, 0, 1);
        esp_matter::lock::chip_stack_unlock();
        break;
    case SwitchButtonAccessory::PressType::LongPress:
        ESP_LOGW(__FILENAME__, "LongPress");
        esp_matter::lock::chip_stack_lock(portMAX_DELAY);
        esp_matter::cluster::switch_cluster::event::send_long_press(
            endpoint_id, 0);
        esp_matter::lock::chip_stack_unlock();
        break;
    case SwitchButtonAccessory::PressType::DoublePress:
        ESP_LOGW(__FILENAME__, "DoublePress");
        esp_matter::lock::chip_stack_lock(portMAX_DELAY);
        esp_matter::cluster::switch_cluster::event::send_multi_press_complete(
            endpoint_id, 0, 2);
        esp_matter::lock::chip_stack_unlock();
        break;
    default:
        break;
    }
}