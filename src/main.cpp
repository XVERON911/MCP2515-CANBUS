#include <Arduino.h>
#include <mcp_can.h>
// GEELY EMGRAND --> Delphi MT80 Engine System(KWP2000)
//-----------------------------------------------------//
//  RX \ NO TX
long unsigned int rxId;
unsigned char len;
unsigned char rxBuf[8];

char msgString[128];
#define CAN0_INT 2 // INT pin for arduino r3
MCP_CAN CAN0(10);  // CS pin for arduino r3

String setNormal = "NORMAL";
String setLoopback = "LOOPBACK";
String sendFrame = "SEND";
String sendOBDPID = "SEND2";
String requestRPM = "SEND3";

byte obd2_DEFAULT[] = {0x02, 0x01, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00}; // default OBD2 1
byte OBD2_CUSTOM[] = {0x02, 0x01, 0x00, 0x55, 0x55, 0x55, 0x55, 0x55};  // custom OBD2 ISO-TP / OEM
byte OBD2_REQUEST_RPM[] = {0x02, 0x01, 0x0C, 0, 0, 0, 0, 0};            // request RPM OBD2

#define standard 1 // CAN BUS LISTEN & REPLY
#if standard == 1
#define LISTEN_ID 0x7EA
#define REPLY_ID 0x7E0
#define FUNCTIONAL_ID 0x7DF
#else
#define LISTEN_ID 0x98DAF101
#define REPLY_ID 0x98DA01F1
#define FUNCTIONAL_ID 0x98DB33F1
#endif

void printError()
{
    byte errFlag = CAN0.getError();
    byte errStatus = CAN0.checkError();
    byte errRegister = CAN0.errorCountRX();
    Serial.print("MCP2515 Detailed Error: ");
    Serial.println(errFlag, HEX);

    Serial.print("CAN Status: ");
    Serial.println(errStatus, HEX);

    Serial.print("TX Error Count: ");
    Serial.println(CAN0.errorCountTX());

    Serial.print("RX Error Count: ");
    Serial.println(errRegister);
}

void sendOBDFrame()
{
    Serial.println("Sending OBD2 Frame To the Bus...");
    if (CAN0.sendMsgBuf(0x7DF, 0, 8, obd2_DEFAULT) == CAN_OK)
    {
        Serial.println("Frame SENT Successfully!");
    }
    else
    {
        Serial.println("ERROR Sending Frame...");
        printError();
    }
}
void SendOBD_PID_request()
{
    Serial.println("Sending OBD2 LIBRARY REQUEST To the Bus...");
    if (CAN0.sendMsgBuf(FUNCTIONAL_ID, 8, OBD2_CUSTOM) == CAN_OK)
    {
        Serial.println("Frame SENT Successfully!");
    }
    else
    {
        Serial.println("ERROR Sending Frame...");
        printError();
    }
}

void Send_Request_RPM()
{
    Serial.println("Sending OBD2 REQUEST RPM To the Bus...");
    if (CAN0.sendMsgBuf(FUNCTIONAL_ID, 8, OBD2_REQUEST_RPM) == CAN_OK)
    {
        Serial.println("Frame SENT Successfully!");
    }
    else
    {
        Serial.println("ERROR Sending Frame...");
        printError();
    }
}

void setup()
{
    Serial.begin(250000);
    pinMode(LED_BUILTIN, OUTPUT);
    if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) // make sure of this !
    {
        CAN0.setMode(MCP_LOOPBACK);
        Serial.println("Initialized MCP2515. Mode: LOOPBACK (default unless NORMAL is sent)");
    }
    else
    {
        Serial.println("Error Initializing MCP2515...");
        Serial.println("Error: " + CAN0.getError());
        printError();
    }
    pinMode(CAN0_INT, INPUT);
}

void loop()
{
    if (Serial.available())
    {
        String command = Serial.readStringUntil('\n');
        command.trim();

        if (command == setNormal)
        {
            CAN0.setMode(MODE_CONFIG);
            delay(10);
            CAN0.setMode(MCP_NORMAL);
            delay(10);
            Serial.println("MCP2515 SET TO NORMAL MODE !");
        }
        else if (command == setLoopback)
        {
            CAN0.setMode(MODE_CONFIG);
            delay(10);
            CAN0.setMode(MCP_LOOPBACK);
            delay(10);
            Serial.println("MCP2515 SET TO LOOPBACK MODE !");
        }
        else if (command == sendFrame)
        {
            sendOBDFrame();
        }
        else if (command == sendOBDPID)
        {
            SendOBD_PID_request();
        }
        else if (command == requestRPM)
        {
            Send_Request_RPM();
        }
        else
        {
            Serial.println("Unknown command.");
        }
    }

    if (!digitalRead(CAN0_INT))
    {
        CAN0.readMsgBuf(&rxId, &len, rxBuf);
        if ((rxId & 0x80000000) == 0x80000000)
            sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
        else
            sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
        Serial.print(msgString);
        if ((rxId & 0x40000000) == 0x40000000)
        {
            sprintf(msgString, " REMOTE REQUEST FRAME");
            Serial.print(msgString);
        }
        else
        {
            for (byte i = 0; i < len; i++)
            {
                sprintf(msgString, " 0x%.2X", rxBuf[i]);
                Serial.print(msgString);
            }
        }
        Serial.println();
    }

    byte canStatus = CAN0.checkError();
    if (canStatus == CAN_CTRLERROR)
    {
        Serial.println("Error was returned from MCP2515: " + CAN0.getError());
        printError();
    }
}
