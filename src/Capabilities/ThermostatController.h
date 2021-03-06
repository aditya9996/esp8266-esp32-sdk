#ifndef _THERMOSTATCONTROLLER_H_
#define _THERMOSTATCONTROLLER_H_

#include "SinricProRequest.h"

/**
 * @brief ThermostatController
 * @ingroup Capabilities
 **/
template <typename T>
class ThermostatController {
  public:
    ThermostatController() { static_cast<T &>(*this).requestHandlers.push_back(std::bind(&ThermostatController<T>::handleThermostatController, this, std::placeholders::_1)); }
    /**
     * @brief Callback definition for onThermostatMode function
     * 
     * Gets called when device receive a `setThermostatMode` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   mode        String with mode device should set to
     * @param[out]  mode        String device mode is set to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section ThermostatModeCallback Example-Code
     * @snippet callbacks.cpp onThermostatMode
     **/
    using ThermostatModeCallback = std::function<bool(const String &, String &)>;

    /**
     * @brief Callback definition for onTargetTemperature function
     * 
     * Gets called when device receive a `targetTemperature` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   temperature Float device is requested to set the target temperature to
     * @param[out]  temperature Float target temperature temperature device is set to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section TargetTemperatureCallback Example-Code
     * @snippet callbacks.cpp onTargetTemperature
     **/
    using SetTargetTemperatureCallback = std::function<bool(const String &, float &)>;

    /**
     * @brief Callback definition for onAdjustTargetTemperature function
     * 
     * Gets called when device receive a `adjustTargetTemperature` reuqest \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   temperature Float relative temperature device should change about
     * @param[out]  temperature Float absolute temperature device is set to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section AdjustTargetTemperatureCallback Example-Code
     * @snippet callbacks.cpp onAdjustTargetTemperature
     **/
    using AdjustTargetTemperatureCallback = std::function<bool(const String &, float &)>;

    void onThermostatMode(ThermostatModeCallback cb);
    void onTargetTemperature(SetTargetTemperatureCallback cb);
    void onAdjustTargetTemperature(AdjustTargetTemperatureCallback cb);

    bool sendThermostatModeEvent(String thermostatMode, String cause = "PHYSICAL_INTERACTION");
    bool sendTargetTemperatureEvent(float temperature, String cause = "PHYSICAL_INTERACTION");

  protected:
    bool handleThermostatController(SinricProRequest &request);

  private:
    ThermostatModeCallback thermostatModeCallback;
    SetTargetTemperatureCallback targetTemperatureCallback;
    AdjustTargetTemperatureCallback adjustTargetTemperatureCallback;
};

/**
 * @brief Set callback function for `setThermostatMode` request
 * 
 * @param cb Function pointer to a `ThermostatModeCallback` function
 * @return void
 * @see ThermostatModeCallback
 **/
template <typename T>
void ThermostatController<T>::onThermostatMode(ThermostatModeCallback cb) {
  thermostatModeCallback = cb;
}

/**
 * @brief Set callback function for `targetTemperature` request
 * 
 * @param cb Function pointer to a `SetTargetTemperatureCallback` function
 * @return void
 * @see SetTargetTemperatureCallback
 **/
template <typename T>
void ThermostatController<T>::onTargetTemperature(SetTargetTemperatureCallback cb) {
  targetTemperatureCallback = cb;
}

/**
 * @brief Set callback function for `adjustTargetTemperature` request
 * 
 * @param cb Function pointer to a `AdjustTargetTemperatureCallback` function
 * @return void
 * @see AdjustTargetTemperatureCallback
 **/
template <typename T>
void ThermostatController<T>::onAdjustTargetTemperature(AdjustTargetTemperatureCallback cb) {
  adjustTargetTemperatureCallback = cb;
}

/**
 * @brief Send `thermostatMode` event to report a the new mode the device has been set to
 * 
 * @param   thermostatMode  String with actual mode (`AUTO`, `COOL`, `HEAT`) the device is set to
 * @param   cause           (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the even
 * @retval  true            event has been sent successfully
 * @retval  false           event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool ThermostatController<T>::sendThermostatModeEvent(String thermostatMode, String cause) {
  T &device = static_cast<T &>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setThermostatMode", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["thermostatMode"] = thermostatMode;
  return device.sendEvent(eventMessage);
}

/**
 * @brief Send `targetTemperature` event to report target temperature change
 * 
 * @param   temperature   Float with actual target temperature the device is set to
 * @param   cause         (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the even
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool ThermostatController<T>::sendTargetTemperatureEvent(float temperature, String cause) {
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("targetTemperature", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["temperature"] = roundf(temperature * 10) / 10.0;
  return device.sendEvent(eventMessage);
}

template <typename T>
bool ThermostatController<T>::handleThermostatController(SinricProRequest &request) {
  T &device = static_cast<T &>(*this);

  bool success = false;

  if (request.action == "targetTemperature" && targetTemperatureCallback) {
    float temperature;
    if (request.request_value.containsKey("temperature"))  {
      temperature = request.request_value["temperature"];
    }  else {
      temperature = 1;
    }
    success = targetTemperatureCallback(device.deviceId, temperature);
    request.response_value["temperature"] = temperature;
    return success;
  }

  if (request.action == "adjustTargetTemperature" && adjustTargetTemperatureCallback) {
    float temperatureDelta = request.request_value["temperature"];
    success = adjustTargetTemperatureCallback(device.deviceId, temperatureDelta);
    request.response_value["temperature"] = temperatureDelta;
    return success;
  }

  if (request.action == "setThermostatMode" && thermostatModeCallback) {
    String thermostatMode = request.request_value["thermostatMode"] | "";
    success = thermostatModeCallback(device.deviceId, thermostatMode);
    request.response_value["thermostatMode"] = thermostatMode;
    return success;
  }

  return success;
}

#endif