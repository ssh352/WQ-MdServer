//
//
// Author: lijiaheng
// this is a base virtual class used in main.cpp

#ifndef MDSERVER_MDENGINE_H_
#define MDSERVER_MDENGINE_H_

#include "ctpwzadapter.h"
#include "wzconstant.h"
#include "wzdatastruct.h"

class WZPiper;

class MdEngine {
 public:
  MdEngine(WZPiper* _output = NULL)
      : output(_output) {}
  virtual void Init() = 0;
  virtual void Join() = 0;
  virtual void Release() = 0;
  virtual void ReqSubscribeMarketData(char *contracts[], int contractsnum) = 0;
  void SetOutput(WZPiper* _output);
  void RtnDepthMarketData(WZMarketDataField* pDepthMarketData);

 private:
  // DISALLOW_COPY_AND_ASSIGN(MdEngine);
  WZPiper* output;
};

#endif  // MDSERVER_MDENGINE_H_
