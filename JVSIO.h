// Copyright 2018 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>

#if !defined(__JVSIO_H__)
#define __JVSIO_H__

// JVSIO provides JVS (Jamma Video Standard 3.0) I/O transport,
// differential serial at 115.2Kbps, 8-bits, 1-start/stop bits, non parity.
// This library can not use with the Serial library because RXI is used
// internally.
// Incomoing data is analyzed by the hardware serial peripheral, RXI.
// The library just reads data from Data+ pin, but ignores Data-.
// Outgoint data is generated by hand-crafted code because the hardware
// does not support differential serial output.
//
// Pins:
//  Data+: See DataClient implementation
//  Data-: See DataClient implementation
//  Sense: See SenseClient implementation
//  LED Ready: See LedClient implementation
class JVSIO {
 public:
 class DataClient {
  public:
   virtual void setMode(int mode) {}
   virtual void write(uint8_t data) {}
 };
  class SenseClient {
   public:
    virtual void begin() {}
    virtual void set(bool ready) {}
  };
  class LedClient {
   public:
    virtual void begin() {}
    virtual void set(bool ready) {}
  };
  enum {
    kCmdReset = 0xF0,           // mandatory
    kCmdAddressSet = 0xF1,      // mandatory

    kCmdIoId = 0x10,            // mandatory
    kCmdCommandRev = 0x11,      // mandatory
    kCmdJvRev = 0x12,           // mandatory
    kCmdProtocolVer = 0x13,     // mandatory
    kCmdFunctionCheck = 0x14,   // mandatory
    kCmdMainId = 0x15,

    kCmdSwInput = 0x20,
    kCmdCoinInput = 0x21,
    kCmdAnalogInput = 0x22,

    kCmdRetry = 0x2F,           // mandatory

    kCmdCoinSub = 0x30,
    kCmdDriverOutput = 0x32,

    kReportOk = 0x01,
    kReportParamErrorNoResponse = 0x02,
    kReportParamErrorIgnored = 0x03,
    kReportBusy = 0x04,
  };
  JVSIO(DataClient data, SenseClient sense, LedClient led);
  ~JVSIO();

  void begin();
  void end();

  uint8_t* getNextCommand(uint8_t* len);

  void pushReport(uint8_t report);

 private:
  void receive();

  void senseNotReady();
  void senseReady();

  void sendStatus();
  void sendOkStatus();
  void sendUnknownCommandStatus();
  void sendSumErrorStatus();
  void sendOverflowStatus();

  DataClient data_;
  SenseClient sense_;
  LedClient led_;
  uint8_t rx_data_[256];
  uint8_t rx_size_;
  uint8_t rx_read_ptr_;
  bool rx_receiving_;
  bool rx_escaping_;
  bool rx_available_;
  uint8_t address_;
  uint8_t tx_data_[256];
  uint8_t tx_report_size_;
};

#endif  // !defined(__JVSIO_H__)
