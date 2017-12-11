# pragma once

#include "protocol/message.h"

#include <list>
#include <memory>

namespace iotea {
    namespace sensor {
        class Sensor {
        public:
            virtual ~Sensor();

            /**
             * Setup sensor's data, pins, etc.
             */
            virtual void configure();

            /**
             * React on message from gateway.
             */
            virtual void handleMessage(const protocol::Message& message);

            /**
             * Messages sensor has for server.
             */
            virtual std::list<std::unique_ptr<protocol::Message>> getMessages();
        };
    }
}
