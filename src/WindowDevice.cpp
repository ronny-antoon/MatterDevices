#include "WindowDevice.hpp"

#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_endpoint.h>

#include <cstdint>

WindowDevice::WindowDevice(const char *device_name, BlindAccessoryInterface *blindAccessory,
                           esp_matter::endpoint_t *aggregator)
    : BaseDevice() {
  BlindAccessory = blindAccessory;

  // Set up the callback for reporting attributes
  BlindAccessory->setReportAppCallback(
      [](void *self) { static_cast<WindowDevice *>(self)->reportEndpoint(); }, this);

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
      ESP_LOGI(__FILENAME__, "Creating Bridged Node WindowDevice with name: %s", name);
      esp_matter::cluster_t *bridge_device_basic_information_cluster =
          esp_matter::cluster::get(endpoint, chip::app::Clusters::BridgedDeviceBasicInformation::Id);
      esp_matter::cluster::bridged_device_basic_information::attribute::create_node_label(
          bridge_device_basic_information_cluster, name, strlen(name));
    } else {
      ESP_LOGW(__FILENAME__, "device_name is not set");
      ESP_LOGI(__FILENAME__, "Creating Bridged Node WindowDevice with default name");
    }
    esp_matter::endpoint::set_parent_endpoint(endpoint, aggregator);
  } else {
    ESP_LOGI(__FILENAME__, "Creating WindowDevice standalone endpoint");
    uint8_t flags = esp_matter::endpoint_flags::ENDPOINT_FLAG_NONE;
    endpoint = esp_matter::endpoint::create(esp_matter::node::get(), flags, this);
  }

  esp_matter::endpoint::window_covering_device::config_t window_config;
  esp_matter::endpoint::window_covering_device::add(endpoint, &window_config);

  esp_matter::cluster_t *window_covering_cluster =
      esp_matter::cluster::get(endpoint, chip::app::Clusters::WindowCovering::Id);

  esp_matter::cluster::window_covering::feature::lift::config_t lift_config;
  esp_matter::cluster::window_covering::feature::position_aware_lift::config_t position_aware_lift_config;
  esp_matter::cluster::window_covering::feature::absolute_position::config_t absolute_position_config;

  position_aware_lift_config.current_position_lift_percentage =
      nullable<uint8_t>(0);  // TODO: change to last known position
  position_aware_lift_config.current_position_lift_percent_100ths =
      nullable<uint16_t>(0);  // TODO: change to last known position
  position_aware_lift_config.target_position_lift_percent_100ths =
      nullable<uint16_t>(0);  // TODO: change to last known position

  esp_matter::cluster::window_covering::feature::lift::add(window_covering_cluster, &lift_config);
  esp_matter::cluster::window_covering::feature::position_aware_lift::add(window_covering_cluster,
                                                                          &position_aware_lift_config);
  esp_matter::cluster::window_covering::feature::absolute_position::add(window_covering_cluster,
                                                                        &absolute_position_config);

  // syncAccessoryState();
}

esp_err_t WindowDevice::updateAccessory() {
  ESP_LOGI(__FILENAME__, "Updating WindowDevice Accessory with target position: %d",
           getEndpointTargetPosition());
  setAccessoryTargetPosition(getEndpointTargetPosition());
  return ESP_OK;
}

esp_err_t WindowDevice::reportEndpoint() {
  ESP_LOGI(__FILENAME__, "Reporting WindowDevice Endpoint with target position: %d",
           getAccessoryTargetPosition());
  setEndpointCurrentPosition(getAccessoryCurrentPosition());
  setEndpointTargetPosition(getAccessoryTargetPosition());
  return ESP_OK;
}

esp_err_t WindowDevice::identify() {
  ESP_LOGI(__FILENAME__, "Identifying WindowDevice");
  BlindAccessory->identifyYourSelf();
  return ESP_OK;
}

uint16_t WindowDevice::getAccessoryCurrentPosition() { return BlindAccessory->getCurrentPosition(); }

uint16_t WindowDevice::getAccessoryTargetPosition() { return BlindAccessory->getTargetPosition(); }

void WindowDevice::setAccessoryTargetPosition(uint16_t position) { BlindAccessory->moveBlindTo(position); }

uint16_t WindowDevice::getEndpointTargetPosition() {
  esp_matter::cluster_t *window_covering_cluster =
      esp_matter::cluster::get(endpoint, chip::app::Clusters::WindowCovering::Id);
  esp_matter::attribute_t *target_position_attribute = esp_matter::attribute::get(
      window_covering_cluster,
      chip::app::Clusters::WindowCovering::Attributes::TargetPositionLiftPercent100ths::Id);
  esp_matter_attr_val_t attr_val;
  esp_matter::attribute::get_val(target_position_attribute, &attr_val);
  return (attr_val.val.u16) / 100;
}

void WindowDevice::setEndpointTargetPosition(uint16_t position) {
  esp_matter_attr_val_t attr_val = esp_matter_nullable_uint16(position * 100);
  esp_matter::attribute::report(
      esp_matter::endpoint::get_id(endpoint), chip::app::Clusters::WindowCovering::Id,
      chip::app::Clusters::WindowCovering::Attributes::TargetPositionLiftPercent100ths::Id, &attr_val);
}

void WindowDevice::setEndpointCurrentPosition(uint16_t position) {
  esp_matter_attr_val_t attr_val = esp_matter_nullable_uint16(position * 100);
  esp_matter::attribute::report(
      esp_matter::endpoint::get_id(endpoint), chip::app::Clusters::WindowCovering::Id,
      chip::app::Clusters::WindowCovering::Attributes::CurrentPositionLiftPercent100ths::Id, &attr_val);
}
