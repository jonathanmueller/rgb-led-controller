#include <Arduino.h>
#include <AsyncHttpClient.h>

class HomeKitNotificationRequest {
    AsyncHttpClient httpClient;
    const String _notificationSuffx;
    const String _characteristic;
    std::function<String()> _getValue;
public:
    HomeKitNotificationRequest(const String& characteristic, std::function<String()> getValue)
        : _notificationSuffx(""),
        _characteristic(characteristic),
        _getValue(getValue) {}

    HomeKitNotificationRequest(const String &notificationSuffix, const String& characteristic, std::function<String()> getValue)
        : _notificationSuffx(notificationSuffix),
        _characteristic(characteristic),
        _getValue(getValue) {}

    void sendUpdate();
};