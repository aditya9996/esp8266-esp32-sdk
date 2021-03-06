#ifndef _RANGECONTROLLER_H_
#define _RANGECONTROLLER_H_

#include "SinricProRequest.h"

/**
 * @brief RangeController
 * @ingroup Capabilities
 **/
template <typename T>
class RangeController {
  public:
    RangeController() { static_cast<T &>(*this).requestHandlers.push_back(std::bind(&RangeController<T>::handleRangeController, this, std::placeholders::_1)); }
    /**
     * @brief Callback definition for onRangeValue function
     * 
     * Gets called when device receive a `setRangeValue` reuqest \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   rangeValue  Integer 0..3 for range value device has to be set
     * @param[out]  rangeValue  Integer 0..3 returning the current range value
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section SetRangeValueCallback Example-Code
     * @snippet callbacks.cpp onRangeValue
     **/
    using SetRangeValueCallback = std::function<bool(const String &, int &)>;

    /**
     * @brief Callback definition for onRangeValue function on a specific instance
     * 
     * Gets called when device receive a `setRangeValue` reuqest \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   instance    String instance name
     * @param[in]   rangeValue  Integer 0..3 for range value device has to be set
     * @param[out]  rangeValue  Integer 0..3 returning the current range value
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section GenericSetRangeValueCallback Example-Code
     * @snippet callbacks.cpp onRangeValueGeneric
     **/
    using GenericSetRangeValueCallback = std::function<bool(const String &, const String &, int &)>;

    /**
     * @brief Callback definition for onAdjustRangeValue function
     * 
     * Gets called when device receive a `adjustRangeValue` reuqest \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   rangeValue  Integer -3..3 delta value for range value have to change
     * @param[out]  rangeValue  Integer 3..3 returning the absolute range value 
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section AdjustRangeValueCallback Example-Code
     * @snippet callbacks.cpp onAdjustRangeValue
     **/
    using AdjustRangeValueCallback = std::function<bool(const String &, int &)>;

    /**
     * @brief Callback definition for onAdjustRangeValue function on a specific instance for custom devices
     * 
     * Gets called when device receive a `adjustRangeValue` reuqest \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   instance    String instance name
     * @param[in]   rangeValue  Integer -3..3 delta value for range value have to change
     * @param[out]  rangeValue  Integer 3..3 returning the absolute range value 
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section GenericAdjustRangeValueCallback Example-Code
     * @snippet callbacks.cpp onAdjustRangeValueGeneric
     **/
    using GenericAdjustRangeValueCallback = std::function<bool(const String&, const String&, int&)>;

    void onRangeValue(SetRangeValueCallback cb);
    void onRangeValue(const String& instance, GenericSetRangeValueCallback cb);

    void onAdjustRangeValue(AdjustRangeValueCallback cb);
    void onAdjustRangeValue(const String& instance, GenericAdjustRangeValueCallback cb);

    bool sendRangeValueEvent(int rangeValue, String cause = "PHYSICAL_INTERACTION");
    bool sendRangeValueEvent(const String& instance, int rangeValue, String cause = "PHYSICAL_INTERACTION");

  protected:
    bool handleRangeController(SinricProRequest &request);

  private:
    SetRangeValueCallback setRangeValueCallback;
    std::map<String, GenericSetRangeValueCallback> genericSetRangeValueCallback;
    AdjustRangeValueCallback adjustRangeValueCallback;
    std::map<String, GenericAdjustRangeValueCallback> genericAdjustRangeValueCallback;
};

/**
 * @brief Set callback function for `setRangeValue` request
 * 
 * @param cb Function pointer to a `SetRangeValueCallback` function
 * @see SetRangeValueCallback
 */
template <typename T>
void RangeController<T>::onRangeValue(SetRangeValueCallback cb) {
  setRangeValueCallback = cb;
}

/**
 * @brief Set callback function for `setRangeValue` request on a specific instance (custom device)
 * 
 * @param instance String instance name (custom device)
 * @param cb Function pointer to a `GenericSetRangeValueCallback` function
 * @see GenericSetRangeValueCallback
 */
template <typename T>
void RangeController<T>::onRangeValue(const String& instance, GenericSetRangeValueCallback cb) {
  genericSetRangeValueCallback[instance] = cb;
}

/**
 * @brief Set callback function for `adjustRangeValue` request
 * 
 * @param cb Function pointer to a `AdjustRangeValueCallback` function
 * @see AdjustRangeValueCallback
 */
template <typename T>
void RangeController<T>::onAdjustRangeValue(AdjustRangeValueCallback cb) {
  adjustRangeValueCallback = cb;
}

template <typename T>
void RangeController<T>::onAdjustRangeValue(const String &instance, GenericAdjustRangeValueCallback cb) {
  genericAdjustRangeValueCallback[instance] = cb;
}


/**
 * @brief Send `rangeValue` event to report curent rangeValue to SinricPro server
 * 
 * @param   rangeValue  Value between 0..3
 * @param   cause       (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the even
 * @retval  true        event has been sent successfully
 * @retval  false       event has not been sent, maybe you sent to much events in a short distance of time
 */
template <typename T>
bool RangeController<T>::sendRangeValueEvent(int rangeValue, String cause) {
  T& device = static_cast<T&>(*this);
  
  DynamicJsonDocument eventMessage = device.prepareEvent("setRangeValue", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["rangeValue"] = rangeValue;
  return device.sendEvent(eventMessage);
}

/**
 * @brief Send `rangeValue` event to report curent rangeValue to SinricPro server for a specific instance (custom device)
 * 
 * @param   instance    String instance name
 * @param   rangeValue  Value between 0..3
 * @param   cause       (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the even
 * @retval  true        event has been sent successfully
 * @retval  false       event has not been sent, maybe you sent to much events in a short distance of time
 */
template <typename T>
bool RangeController<T>::sendRangeValueEvent(const String& instance, int rangeValue, String cause){
  T &device = static_cast<T &>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setRangeValue", cause.c_str());
  eventMessage["payload"]["instanceId"] = instance;

  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["rangeValue"] = rangeValue;
  return device.sendEvent(eventMessage);
}

template <typename T>
bool RangeController<T>::handleRangeController(SinricProRequest &request) {
  T &device = static_cast<T &>(*this);

  bool success = false;

  if (request.action == "setRangeValue") {
    int rangeValue = request.request_value["rangeValue"] | 0;
    if (request.instance != "") {
      if (genericSetRangeValueCallback.find(request.instance) != genericSetRangeValueCallback.end()) 
        success = genericSetRangeValueCallback[request.instance](device.deviceId, request.instance, rangeValue);
    } else {
      if (setRangeValueCallback) success = setRangeValueCallback(device.deviceId, rangeValue);
    }
    request.response_value["rangeValue"] = rangeValue;
    return success;
  }

  if (request.action == "adjustRangeValue") {
    int rangeValueDelta = request.request_value["rangeValueDelta"] | 0;
    if (request.instance != "") {
      if (genericAdjustRangeValueCallback.find(request.instance) != genericAdjustRangeValueCallback.end()) 
        success = genericAdjustRangeValueCallback[request.instance](device.deviceId, request.instance, rangeValueDelta);
    } else {
      if (adjustRangeValueCallback)
        success = adjustRangeValueCallback(device.deviceId, rangeValueDelta);
    }
    
    request.response_value["rangeValue"] = rangeValueDelta;
    return success;
  }

  return success;
}

#endif