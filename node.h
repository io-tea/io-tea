#ifndef IOTEA_NODE_H_
#define IOTEA_NODE_H_

#include "mbed.h"
#include "nRF24L01P.h"
#include "cantcoap/cantcoap.h"
#include <string>

extern Serial pc;
extern nRF24L01P radio;
extern Ticker readTicker;
extern volatile bool tick;

void setupSerial(const std::string& label);
void setupNodeRadio(uint64_t address);
void setupGatewayRadio();
void setupTicker();

// Only for nodes
void sendCoapMessage(const std::string& uri, const std::string& data);
void printStatus();

#endif // ifndef IOTEA_NODE_H_
