/*
  AsyncHttpClient.h - Library for sending simple data via HTTP GET/POST.
  Created by Jonathan Strauss, July 27, 2020.
*/

#ifndef AsyncHttpClient_h
#define AsyncHttpClient_h

#include "Arduino.h"
#include "ESPAsyncTCP.h"
#include <map>

class AsyncHttpClient
{
  String _method;
  String _url;
  String _hostname;
  int _port;
  String _headers;
  AsyncClient* aClient = NULL;
  void updateHostAndPort();

public:
  void init(const String& method, const String& url);
  void setMethod(const String& method);
  void setURL(const String& url);
  void addHeader(const String& header, const String& value);
  void send(const String& data = "");

  bool isReady();
};

#endif