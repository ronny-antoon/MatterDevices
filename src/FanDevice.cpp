#include "FanDevice.hpp"

#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_endpoint.h>

#include <cstdint>

FanDevice::FanDevice(char *device_name, gpio_num_t fan_pin, gpio_num_t button_pin,
                     esp_matter::endpoint_t *aggregator)
    : BaseDevice() {
  if (fan_pin == GPIO_NUM_NC) {
    ESP_LOGW(__FILENAME__, "fan_pin is not set");
  } else {
    if (button_pin == GPIO_NUM_NC) {
      ESP_LOGW(__FILENAME__, "button_pin is not set");
    }

    ESP_LOGI(__FILENAME__, "Creating FanAccessory with fan_pin: %d, button_pin: %d", fan_pin, button_pin);

    // Create the FanAccessory instance
    fanAccessory = new FanAccessory(button_pin, fan_pin);

    // Set up the callback for reporting attributes
    fanAccessory->setReportAttributesCallback(
        [](void *self) { static_cast<FanDevice *>(self)->reportEndpoint(); }, this);
  }

  // Check if an aggregator is provided
  if (aggregator != nullptr) {
    esp_matter::endpoint::bridged_node::config_t bridged_node_config;
    uint8_t flags = esp_matter::endpoint_flags::ENDPOINT_FLAG_BRIDGE |
                    esp_matter::endpoint_flags::ENDPOINT_FLAG_DESTROYABLE;
    endpoint = esp_matter::endpoint::bridged_node::create(esp_matter::node::get(), &bridged_node_config,
                                                          flags, this);
    if (device_name != nullptr && strlen(device_name) > 0 &&
        strlen(device_name) < 64)  // TODO: change to a define
    {
      strncpy(this->name, device_name, sizeof(this->name));
      ESP_LOGI(__FILENAME__, "Creating Bridged Node FanDevice with name: %s", name);
      esp_matter::cluster_t *bridge_device_basic_information_cluster =
          esp_matter::cluster::get(endpoint, chip::app::Clusters::BridgedDeviceBasicInformation::Id);
      esp_matter::cluster::bridged_device_basic_information::attribute::create_node_label(
          bridge_device_basic_information_cluster, name, strlen(name));
    } else {
      ESP_LOGW(__FILENAME__, "device_name is not set");
      ESP_LOGI(__FILENAME__, "Creating Bridged Node FanDevice with default name");
    }
    esp_matter::endpoint::set_parent_endpoint(endpoint, aggregator);
  } else {
    ESP_LOGI(__FILENAME__, "Creating FanDevice standalone endpoint");
    uint8_t flags = esp_matter::endpoint_flags::ENDPOINT_FLAG_NONE;
    endpoint = esp_matter::endpoint::create(esp_matter::node::get(), flags, this);
  }

  esp_matter::endpoint::fan::config_t fan_config;
  esp_matter::endpoint::fan::add(endpoint, &fan_config);

  setAccessoryPowerState(getEndpointPowerState());
}

esp_err_t FanDevice::updateAccessory() {
  bool powerState = getEndpointPowerState();
  ESP_LOGI(__FILENAME__, "Updating FanDevice accessory with power state: %d", powerState);
  setAccessoryPowerState(powerState);
  return ESP_OK;
}

esp_err_t FanDevice::reportEndpoint() {
  bool powerState = getAccessoryPowerState();
  ESP_LOGI(__FILENAME__, "Reporting FanDevice endpoint with power state: %d", powerState);

  setEndpointPowerState(powerState);
  return ESP_OK;
}

bool FanDevice::getAccessoryPowerState() { return fanAccessory->getPower(); }

void FanDevice::setAccessoryPowerState(bool powerState) { fanAccessory->setPower(powerState); }

bool FanDevice::getEndpointPowerState() {
  esp_matter::cluster_t *fan_cluster =
      esp_matter::cluster::get(endpoint, chip::app::Clusters::FanControl::Id);
  esp_matter::attribute_t *fan_percent_setting_attribute = esp_matter::attribute::get(
      fan_cluster, chip::app::Clusters::FanControl::Attributes::PercentSetting::Id);
  esp_matter_attr_val_t attr_val;
  esp_matter::attribute::get_val(fan_percent_setting_attribute, &attr_val);

  if (attr_val.val.u8 == 0) {
    return false;
  } else {
    return true;
  }
}

void FanDevice::setEndpointPowerState(bool powerState) {
  esp_matter_attr_val_t percentCurrent_val = esp_matter_uint8(powerState ? 100 : 0);
  esp_matter::attribute::report(esp_matter::endpoint::get_id(endpoint), chip::app::Clusters::FanControl::Id,
                                chip::app::Clusters::FanControl::Attributes::PercentCurrent::Id,
                                &percentCurrent_val);

  esp_matter_attr_val_t fanMode_val = esp_matter_enum8(powerState ? 3 : 0);
  esp_matter::attribute::report(esp_matter::endpoint::get_id(endpoint), chip::app::Clusters::FanControl::Id,
                                chip::app::Clusters::FanControl::Attributes::FanMode::Id, &fanMode_val);

  esp_matter_attr_val_t percentSetting_val = esp_matter_nullable_uint8(powerState ? 100 : 0);
  esp_matter::attribute::report(esp_matter::endpoint::get_id(endpoint), chip::app::Clusters::FanControl::Id,
                                chip::app::Clusters::FanControl::Attributes::PercentSetting::Id,
                                &percentSetting_val);
}

esp_err_t FanDevice::identify() {
  ESP_LOGI(__FILENAME__, "Identifying FanDevice");
  fanAccessory->identifyYourSelf();
  return ESP_OK;
}
