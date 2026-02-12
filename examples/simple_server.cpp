/*++

Simple UDP Echo Server Example

This example demonstrates how to use the SubSpace libraries to create
a basic UDP echo server that receives packets and sends them back.

Build:
    g++ -std=c++11 -I../lib/include -L../lib simple_server.cpp \
        -lnetlib -ltimelib -lloglib -o simple_server

Usage:
    ./simple_server [port]

Test:
    echo "Hello" | nc -u localhost 6000

--*/

#include "base.h"
#include "NetLib.h"
#include "TimeLib.h"
#include "LogLib.h"

#include <iostream>
#include <csignal>

static bool gRunning = true;

void signalHandler(int signal)
{
    gRunning = false;
}

int main(int argc, char * argv[])
{
    // Set up signal handler
    signal(SIGINT, signalHandler);
    
    // Parse port from command line
    uint16 port = 6000;
    if(argc > 1)
        sscanf(argv[1], "%hu", &port);
    
    // Set up logging
    StdoutLogSink sink;
    Logger::Instance().AttachSink(sink);
    
    Logger::Instance().Log(KLogInfo, "Starting UDP echo server on port %d", port);
    Logger::Instance().Log(KLogInfo, "Press Ctrl+C to stop");
    
    try
    {
        // Create selector and UDP socket
        Selector selector;
        UdpClient server;
        
        // Bind to port and register with selector
        server.Bind(port);
        server.Register(selector, Selector::KRead);
        
        Logger::Instance().Log(KLogInfo, "Server started successfully");
        
        // Main server loop
        while(gRunning)
        {
            // Wait for incoming data (100ms timeout)
            selector.Select(100);
            
            // Check if we have data to read
            if(!selector.ReadSet().empty())
            {
                // Read the packet
                InetAddress clientAddr;
                bstring packet = server.Read(clientAddr, 65536);
                
                if(packet.length() > 0)
                {
                    Logger::Instance().Log(KLogInfo, 
                        "Received %zu bytes from %s:%d",
                        packet.length(),
                        clientAddr.GetHost().c_str(),
                        clientAddr.GetPort());
                    
                    // Echo the packet back
                    server.Write(clientAddr, packet);
                    
                    Logger::Instance().Log(KLogInfo, "Echoed packet back to client");
                }
            }
        }
        
        Logger::Instance().Log(KLogInfo, "Server shutting down");
    }
    catch(const std::exception & e)
    {
        Logger::Instance().Log(KLogError, "Exception: %s", e.what());
        return 1;
    }
    
    return 0;
}
