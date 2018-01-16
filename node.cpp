#include "node.h"

Serial pc(USBTX, USBRX);
nRF24L01P radio(PB_15, PB_14, PB_13, PB_12, PB_1, PB_2);
Ticker readTicker;
volatile bool tick = false;

void setupSerial(const std::string& label) {
    pc.baud(115200);
    pc.printf("\r\n\r\n\r\n");
    pc.printf((label + "\r\n").c_str());
}

void setupTicker() {
    readTicker.attach([&]() { tick = true; }, 0.1);
}

void setupNodeRadio(uint64_t address) {
    radio.powerDown();
    radio.powerUp();

    radio.setAirDataRate(NRF24L01P_DATARATE_250_KBPS);
    radio.setRfOutputPower(NRF24L01P_TX_PWR_ZERO_DB);
    radio.setRfFrequency(NRF24L01P_MIN_RF_FREQUENCY + 4 * 2);
    radio.setCrcWidth(NRF24L01P_CRC_8_BIT);

    radio.setTxAddress(address, 5);
    radio.setRxAddress(address, 5, NRF24L01P_PIPE_P0);

    radio.setTransferSize(32, NRF24L01P_PIPE_P0);
    
    pc.printf("nRF24L01+ Frequency    : %d MHz\r\n",  radio.getRfFrequency());
    pc.printf("nRF24L01+ Output power : %d dBm\r\n",  radio.getRfOutputPower());
    pc.printf("nRF24L01+ Data Rate    : %d kbps\r\n", radio.getAirDataRate());
    pc.printf("nRF24L01+ TX Address   : 0x%010llX\r\n", radio.getTxAddress());
    pc.printf("nRF24L01+ RX0 Address  : 0x%010llX\r\n", radio.getRxAddress(NRF24L01P_PIPE_P0));

    radio.setReceiveMode();
    radio.enable();
}

void setupGatewayRadio() {
    radio.powerDown();
    radio.powerUp();

    radio.setAirDataRate(NRF24L01P_DATARATE_250_KBPS);
    radio.setRfOutputPower(NRF24L01P_TX_PWR_ZERO_DB);
    radio.setRfFrequency(NRF24L01P_MIN_RF_FREQUENCY + 4 * 2);
    radio.setCrcWidth(NRF24L01P_CRC_8_BIT);

    radio.setRxAddress(0xABCDEF00, 5, NRF24L01P_PIPE_P0);
    radio.setRxAddress(0xABCDEF01, 5, NRF24L01P_PIPE_P1);
    radio.setRxAddress(0xABCDEF02, 5, NRF24L01P_PIPE_P2);

    radio.setTransferSize(32, NRF24L01P_PIPE_P0);
    radio.setTransferSize(32, NRF24L01P_PIPE_P1);
    radio.setTransferSize(32, NRF24L01P_PIPE_P2);

    pc.printf("nRF24L01+ Frequency    : %d MHz\r\n",  radio.getRfFrequency());
    pc.printf("nRF24L01+ Output power : %d dBm\r\n",  radio.getRfOutputPower());
    pc.printf("nRF24L01+ Data Rate    : %d kbps\r\n", radio.getAirDataRate());
    pc.printf("nRF24L01+ TX Address   : 0x%010llX\r\n", radio.getTxAddress());
    pc.printf("nRF24L01+ RX0 Address  : 0x%010llX\r\n", radio.getRxAddress(NRF24L01P_PIPE_P0));
    pc.printf("nRF24L01+ RX1 Address  : 0x%010llX\r\n", radio.getRxAddress(NRF24L01P_PIPE_P1));
    pc.printf("nRF24L01+ RX2 Address  : 0x%010llX\r\n", radio.getRxAddress(NRF24L01P_PIPE_P2));

    radio.setReceiveMode();
    radio.enable();
}

void sendCoapMessage(const std::string& uri, const std::string& data) {
    static uint64_t nextMessageID = 0;

    CoapPDU coapPDU;

    coapPDU.setType(CoapPDU::COAP_NON_CONFIRMABLE);
    coapPDU.setCode(CoapPDU::COAP_PUT);
    coapPDU.setMessageID(nextMessageID++);

    // Sam protokół zjada nam ~5 bajtów, zostaje więc ~25 na uri i dane
    // (bo ramki radiowe mogą mieć maksymalnie ~32 bajty).
    coapPDU.setURI(const_cast<char*>(uri.c_str()), uri.size());
    coapPDU.setPayload((uint8_t*)&data[0], data.size());

    std::string msg(coapPDU.getPDUPointer(), coapPDU.getPDUPointer() + coapPDU.getPDULength());
    msg.resize(32, '\0');

    for (int tries = 0; tries < 4; ++tries) {
        radio.write(/* unused */ 0, &msg[0], 32);
        wait((rand() % 10 + 1) * 1e-3);
    }
}
