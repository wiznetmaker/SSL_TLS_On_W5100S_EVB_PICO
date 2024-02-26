/*W5100S_EVB_PICO ChatGPT API SSL Communication Test */ 
#include <SPI.h>
#include <Ethernet.h>
#include <SSLClient.h>
#include "trust_anchors.h"
#include "html_page.h"

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0x00, 0x08, 0xdc, 0xEF, 0xFE, 0xED };

// Enter a Server address for your controller below.
char chatgpt_server[] = "api.openai.com"; 
char chatgpt_token[] = "sk-vhq3ZsD6bc8w3sA********************";
 
// Choose the analog pin to get random data from a randomish voltage source for SSL
const int rand_pin = 28;
 
// Init "SSLclient" and Input Ethernet_Client, trust_anchors and the random
EthernetClient Ethernet_client;
SSLClient client(Ethernet_client, TAs, (size_t)TAs_NUM, rand_pin);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80); 
EthernetClient http_client;

typedef enum 
{
  do_webserver_index= 0,
  do_webserver_js,
  send_chatgpt_request,
  get_chatgpt_answer,
  Show_session_in_webpage
}STATE_;
STATE_ currentState;

uint8_t data_now = 0; 
String json_String; 
uint16_t dataStart = 0;
uint16_t dataEnd = 0;
String dataStr;
uint16_t dataLen = 0;
String History_String;
String chatgpt_Q;
String chatgpt_A;
uint16_t chatgpt_num = 0;

//// Variables to measure the speed
//unsigned long beginMicros, endMicros;
//unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement
 
void setup() {
    Serial.begin(115200);
//    while (!Serial) {
//      ; // wait for serial port to connect. Needed for native USB port only
//    }    
//    delay(500);
    Serial.println("W5100S_EVB_PICO ChatGPT API SSL Communication Test");
 
    // You can use Ethernet.init(pin) to configure the CS pin
    Ethernet.init(17);  // WIZnet W5100S-EVB-Pico
 
    if (Ethernet.begin(mac)) {
      Serial.print("W5100S_EVB_PICO IP : ");
      Serial.println(Ethernet.localIP());
    }
    delay(5000); 
    
    // start the web server on port 80
    server.begin();
}
 
void loop() {
  switch(currentState){
    case do_webserver_index:
       {
          http_client = server.available();
          if (http_client) 
          {            
             // an http request ends with a blank line
             boolean currentLineIsBlank = true;    
             while (http_client.connected()) 
             {
                if (http_client.available()) {
                char c = http_client.read();
                json_String += c;
                if (c == '\n' && currentLineIsBlank) {                                 
                  dataStr = json_String.substring(0, 4);
                  if(dataStr == "GET ")
                  {
//                    Serial.println(html_page_start + History_String + html_page_end);
                    http_client.print(html_page_start + History_String + html_page_end);
                  }
                  else if(dataStr == "POST")
                  {
                    json_String = "";
                    while(http_client.available())
                    {
                      json_String += (char)http_client.read();
                    }
                    dataStart = json_String.indexOf("chatgpttext=") + strlen("chatgpttext=");
                    chatgpt_Q = json_String.substring(dataStart, json_String.length());
                    http_client.print(html_page_start + History_String + html_page_end);    
                    // close the connection:
                    delay(10);
                    http_client.stop();       
                    currentState = send_chatgpt_request;
                  }
                  json_String = "";
                  break;
                }
                if (c == '\n') {
                  // you're starting a new line
                  currentLineIsBlank = true;
                }
                else if (c != '\r') {
                  // you've gotten a character on the current line
                  currentLineIsBlank = false;
                }
              }
            }
          }
       }
      break;
      
    case send_chatgpt_request:
       {
        // if you get a connection, report back via serial
        if (client.connect(chatgpt_server,443)){
          // Make a HTTPS request
          client.println(String("POST /v1/chat/completions HTTP/1.1"));
          client.println(String("Host: ")+ chatgpt_server);          
          client.println(String("Content-Type: application/json"));
          client.println(String("Content-Length: ")+(95+chatgpt_Q.length()));
          client.println(String("Authorization: Bearer ")+ chatgpt_token);
          client.println("Connection: close");
          client.println();
          client.println(String("{\"model\": \"gpt-3.5-turbo-0125\",\"messages\": [{\"role\": \"user\", \"content\": \"")+ chatgpt_Q + String("\"}],\"temperature\": 0}")); 
          chatgpt_Q.replace("+", " ");
          History_String += String("<span style=\"color: green;\">You:");
          History_String += chatgpt_Q;
          History_String += String("</span><br>");
          json_String= "";
          delay(300);
          currentState = get_chatgpt_answer;          
        }
        else
        {
          client.stop();
          delay(1000);
        }
       }
      break;
      
    case get_chatgpt_answer:
       {
          while (client.available()) {
            json_String += (char)client.read();
            data_now =1; 
          }
          if(data_now)
          {
            dataStart = json_String.indexOf("\"content\": \"") + strlen("\"content\": \"");
            dataEnd = json_String.indexOf("\"", dataStart); 
            chatgpt_A = json_String.substring(dataStart, dataEnd);
            chatgpt_A.replace("\\n\\n", "<br>");
            History_String += String("<span style=\"color: blue;\">OpenAI:");
            History_String += chatgpt_A;
            History_String += String("</span><br>");
            Serial.println(json_String);
            Serial.println(History_String);
            json_String = "";
            data_now =0;
            client.stop();
            currentState = do_webserver_index;
          }
       }
      break;
  }
}
