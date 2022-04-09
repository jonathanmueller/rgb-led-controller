#include "HomeKitNotificationRequest.h"

extern String notificationServer;

void HomeKitNotificationRequest::sendUpdate() {
    if (!this->httpClient.isReady()) {
        return;
    }

    char chipId[9]; sprintf(chipId, "%08X", ESP.getChipId());
    this->httpClient.init("POST", notificationServer + "/" + chipId + _notificationSuffx);
    this->httpClient.addHeader("Content-Type", "application/json");
    this->httpClient.send("{\"characteristic\":\"" + _characteristic + "\",\"value\":" + _getValue() + "}");
}