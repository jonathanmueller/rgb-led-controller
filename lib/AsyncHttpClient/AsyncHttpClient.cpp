/*
  AsyncHttpClient.cpp - Library for sending simple data via HTTP GET/POST.
  Created by Jonathan Strauss, July 27, 2020.
*/

#include "Arduino.h"
#include "ESPAsyncTCP.h"
#include "AsyncHttpClient.h"

/*
    String method = POST or GET (most used for GET)
    String fullUrl = full URL e.g. http://server.com:8080/push.php?api-key=1asnd12i3kas&firstname=Jonathan&lastname=Strauss  or http://server.com or http://server.com:8081
*/

void AsyncHttpClient::init(const String& method, const String& url)
{
    setMethod(method);
    setURL(url);
    _headers.clear();
}

void AsyncHttpClient::setMethod(const String& method) {
    _method = method;
}

void AsyncHttpClient::setURL(const String& url) {
    _url = url;
    updateHostAndPort();
}

void AsyncHttpClient::addHeader(const String& header, const String& value) {
    _headers += header + ": " + value + "\r\n";
}

/* Extract Hostname and Port from URL */
void AsyncHttpClient::updateHostAndPort() {
    String url = _url;
    int index = url.indexOf(':');
    if (index < 0) {
        _hostname = "";
        return;
    }
    String protocol = url.substring(0, index);
    _port = 80;
    if (index == 5) {
        _port = 443;
    }

    url.remove(0, (index + 3));
    index = url.indexOf('/');
    String hostPart;
    if (index > 0) {
        hostPart = url.substring(0, index);
    } else {
        hostPart = url;
    }

    url.remove(0, index);

    index = hostPart.indexOf(':');
    if (index >= 0) {
        _hostname = hostPart.substring(0, index);
        hostPart.remove(0, (index + 1)); // remove hostname + :
        _port = hostPart.toInt();        // get port
    } else {
        _hostname = hostPart;
    }
}

bool AsyncHttpClient::isReady() {
    return !aClient;
}


void AsyncHttpClient::send(const String& body) {
    if (_hostname.length() <= 0) {
        Serial.println("Hostname is not defined");
        return;
    }

    if (_port <= 0) {
        Serial.println("Port is not defined");
        return;
    }

    if (_method.length() <= 0) {
        Serial.println("method is not defined (Only GET or POST)");
        return;
    }

    if (_url.length() <= 0) {
        Serial.println("URL is not defined");
        return;
    }

    if (aClient) //client already exists
        return;

    aClient = new AsyncClient();
    aClient->setRxTimeout(5000);
    aClient->setAckTimeout(5000);

    aClient->onError([&](void* arg, AsyncClient* client, int error) {
        aClient = NULL;
        delete client;
        }, NULL);

    aClient->onConnect([&](void* arg, AsyncClient* client) {
        aClient->onError(NULL, NULL);
        aClient->onData(NULL, NULL);

        client->onDisconnect([&](void* arg, AsyncClient* c) {
            aClient = NULL;
            delete c;
            }, NULL);

        //send the request
        client->write(String(_method + " " + _url + " HTTP/1.1\r\nHost: " + _hostname + "\r\nConnection: close\r\n" + _headers).c_str());
        if (!body.isEmpty()) {
            client->write(String("Content-Length: " + String(body.length()) + "\r\n\r\n" + body).c_str());
        }
        client->write("\r\n");

        }, NULL);


    if (!aClient->connect(_hostname.c_str(), _port)) {
        Serial.println("HTTP connection failed");
        AsyncClient* client = aClient;
        aClient = NULL;
        delete client;
    }
}