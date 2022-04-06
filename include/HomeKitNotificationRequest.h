#include <Arduino.h>
#include <AsyncHttpClient.h>

class HomeKitNotificationRequest {
    AsyncHttpClient httpClient;
    const String _characteristic;
    std::function<String()> _getValue;
public:
    HomeKitNotificationRequest(const String& characteristic, std::function<String()> getValue) : _characteristic(characteristic), _getValue(getValue) {}
    void sendUpdate();
};