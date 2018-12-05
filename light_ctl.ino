extern "C" {
#include "user_interface.h"
}
#include <ESP8266WiFi.h> 
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define LED_BUILTIN 2
const char* ssid = "FQQ";
const char* password = "1qaz2wsx";
ESP8266WebServer server(80);
IRsend irsend(4);  // An IR LED is controlled by GPIO pin 4 (D2)
// Panasonic 松下照明：遥控器 适用于HHLAZ2019型号,部分按钮适用于 HHLAZ2003 （开、关、全亮、常夜灯）
// 开
uint16_t kai[131] = {3562, 1614,  468, 378,  466, 1246,  468, 378,  466, 1246,  468, 378,
                     466, 1248,  468, 376,  466, 1246,  468, 378,  466, 378,  466, 378,
                     466, 1246,  468, 378,  466, 1248,  466, 1248,  466, 376,  466, 1248,
                     468, 376,  466, 376,  466, 376,  466, 1248,  466, 1248,  468, 1248,
                     466, 376,  466, 1248,  466, 378,  466, 1248,  466, 376,  466, 1248,
                     468, 1248,  466, 376,  466, 378,  466, 378,  466, 376,  464, 378,
                     466, 376,  466, 378,  466, 378,  464, 380,  462, 378,  466, 1248,
                     466, 376,  466, 1248,  466, 378,  466, 1248,  466, 1246,  466, 1248,
                     466, 376,  466, 1248,  466, 378,  464, 378,  464, 376,  466, 378,
                     466, 1248,  466, 376,  464, 380,  466, 1248,  464, 378,
                     464, 378,  464, 378,  464, 1250,  466, 378,
                     464, 1250,  466, 378,  464
                    };  // UNKNOWN 28D702CA
//关
uint16_t guan[131] = {3486, 1688,  394, 450,  392, 1320,  394, 452,  392, 1322,  392, 452,
                      392, 1322,  392, 452,  392, 1322,  394, 450,  392, 452,  392, 450,
                      394, 1320,  392, 450,  392, 1322,  392, 1320,  394, 450,  392, 1324,
                      392, 450,  392, 452,  390, 452,  392, 1322,  392, 1322,  392, 1322,
                      392, 452,  392, 1322,  392, 450,  392, 1322,  394, 450,  392, 1320,
                      394, 1320,  394, 450,  392, 450,  392, 452,  390, 452,  390, 450,
                      394, 450,  392, 452,  390, 450,  392, 450,  392, 452,  392, 450,
                      392, 1322,  392, 452,  392, 452,  392, 452,  390, 452,  392, 452,
                      392, 452,  390, 1320,  394, 452,  392, 452,  390, 452,  392, 450,
                      394, 1320,  394, 452,  392, 450,  394, 1322,  392, 450,  392, 450,
                      392, 452,  392, 1320,  394, 1320,  394, 450,  392, 1322,  392
                     };  // UNKNOWN 2F31FF69
//明
uint16_t ming[131] = {3566, 1610,  472, 372,  470, 1244,  462, 384,  456, 1256,  458, 388,
                      456, 1256,  440, 404,  458, 1254,  460, 386,  458, 412,  442, 372,
                      460, 1256,  470, 372,  470, 1244,  470, 1248,  468, 374,  468, 1246,
                      470, 374,  468, 374,  468, 376,  468, 1244,  470, 1244,  470, 1246,
                      470, 374,  470, 1244,  462, 384,  468, 1246,  468, 374,  468, 1246,
                      470, 1246,  468, 374,  470, 374,  468, 374,  470, 374,  468, 374,
                      468, 376,  468, 374,  470, 374,  470, 374,  470, 374,  468, 1246,
                      470, 374,  468, 376,  468, 1244,  470, 1244,  470, 374,  468, 374,
                      470, 374,  468, 1246,  468, 374,  468, 376,  468, 374,  470, 374,
                      470, 1244,  470, 374,  468, 374,  468, 1244,  468, 376,  468, 376,
                      468, 376,  468, 1246,  468, 376,  466, 376,  466, 376,  466
                     };  // UNKNOWN AB8530DA
//暗
uint16_t an[131] = {3566, 1608,  474, 372,  470, 1242,  472, 374,  470, 1242,  472, 374,
                    470, 1242,  472, 374,  468, 1244,  472, 372,  470, 374,  470, 372,
                    470, 1244,  470, 374,  470, 1244,  472, 1242,  472, 372,  470, 1242,
                    472, 372,  472, 372,  470, 374,  470, 1242,  472, 1244,  472, 1246,
                    470, 372,  470, 1244,  470, 374,  470, 1244,  472, 372,  470, 1244,
                    470, 1244,  472, 374,  470, 372,  470, 372,  470, 374,  470, 372,
                    470, 374,  468, 374,  470, 372,  470, 374,  470, 372,  470, 1244,
                    472, 372,  470, 374,  470, 374,  468, 374,  470, 1244,  472, 372,
                    470, 374,  470, 1244,  470, 372,  470, 374,  470, 374,  470, 374,
                    470, 1244,  470, 374,  470, 372,  470, 1244,  472, 374,  470, 374,
                    470, 374,  466, 378,  458, 1254,  472, 372,  470, 1244,  470
                   };  // UNKNOWN DABC10BF
//日光色
uint16_t riguangse[131] = {3566, 1608,  472, 374,  470, 1242,  472, 376,  466, 1246,  470, 374,
                           468, 1246,  470, 376,  466, 1246,  470, 374,  470, 374,  468, 374,
                           468, 1244,  472, 374,  468, 1246,  468, 1246,  470, 374,  468, 1244,
                           470, 376,  468, 374,  468, 374,  468, 1246,  470, 1246,  470, 1244,
                           470, 374,  468, 1248,  468, 376,  468, 1244,  470, 374,  470, 1244,
                           470, 1246,  470, 374,  468, 374,  468, 376,  468, 374,  468, 374,
                           468, 376,  468, 374,  470, 374,  468, 374,  468, 376,  468, 1244,
                           470, 374,  468, 1246,  470, 374,  468, 1246,  470, 374,  468, 374,
                           470, 1246,  468, 1246,  470, 376,  468, 376,  468, 374,  468, 374,
                           468, 1246,  468, 376,  468, 376,  466, 1246,  468, 376,  468, 376,
                           468, 376,  468, 1246,  468, 376,  468, 1244,  470, 376,  468
                          };  // UNKNOWN 7DDF653A
//暖白色
uint16_t nuanbaise[131] = {3562, 1614,  468, 376,  468, 1248,  468, 374,  468, 1248,  468, 378,
                           466, 1246,  470, 374,  468, 1248,  468, 376,  466, 378,  466, 376,
                           466, 1248,  468, 376,  466, 1248,  468, 1248,  466, 376,  466, 1248,
                           468, 378,  468, 374,  468, 378,  466, 1248,  466, 1248,  468, 1248,
                           468, 376,  466, 1250,  468, 376,  466, 1248,  468, 378,  466, 1248,
                           466, 1250,  466, 378,  466, 378,  466, 378,  464, 378,  466, 378,
                           466, 378,  466, 378,  466, 376,  466, 378,  466, 376,  466, 1248,
                           466, 378,  466, 1248,  466, 378,  466, 376,  466, 378,  466, 376,
                           466, 1248,  466, 1248,  466, 378,  466, 378,  462, 382,  472, 370,
                           466, 1248,  468, 376,  466, 378,  464, 1250,  466, 378,  466, 378,
                           466, 378,  466, 378,  466, 376,  466, 1250,  466, 378,  466
                          };  // UNKNOWN 861C6892
//全亮
uint16_t quanliang[131] = {3564, 1612,  470, 376,  468, 1246,  466, 378,  466, 1248,  468, 376,
                           468, 1248,  468, 376,  466, 1248,  468, 374,  468, 376,  466, 376,
                           466, 1248,  468, 376,  466, 1246,  468, 1246,  468, 376,  466, 1248,
                           468, 376,  466, 376,  466, 376,  468, 1246,  468, 1246,  468, 1248,
                           466, 376,  466, 1248,  466, 376,  468, 1246,  468, 378,  466, 1248,
                           466, 1248,  466, 378,  466, 376,  466, 376,  466, 378,  466, 376,
                           386, 458,  462, 378,  468, 376,  466, 378,  464, 376,  466, 1248,
                           466, 378,  464, 378,  466, 378,  466, 376,  466, 376,  466, 378,
                           464, 378,  466, 1248,  468, 376,  466, 378,  466, 378,  466, 376,
                           466, 1248,  468, 378,  466, 378,  466, 1248,  466, 378,  466, 378,
                           466, 376,  466, 378,  464, 378,  466, 378,  466, 1248,  466
                          };  // UNKNOWN 82178287
//常夜灯
uint16_t changyedeng[131] = {3566, 1610,  472, 374,  468, 1246,  468, 374,  470, 1246,  468, 374,
                             468, 1246,  468, 374,  468, 1246,  470, 376,  468, 374,  470, 374,
                             470, 1244,  470, 374,  468, 1246,  470, 1244,  470, 374,  470, 1246,
                             470, 376,  468, 376,  468, 374,  468, 1246,  470, 1244,  470, 1246,
                             470, 374,  468, 1246,  468, 376,  466, 1246,  468, 374,  468, 1246,
                             468, 1246,  468, 376,  468, 376,  468, 376,  466, 376,  468, 374,
                             468, 376,  468, 376,  466, 376,  468, 376,  466, 376,  468, 1246,
                             468, 374,  468, 1248,  388, 454,  468, 1246,  388, 460,  436, 404,
                             472, 372,  456, 1256,  388, 458,  466, 376,  458, 388,  434, 406,
                             438, 1276,  436, 436,  430, 388,  464, 1248,  468, 376,  468, 378,
                             466, 376,  466, 1248,  468, 376,  468, 1246,  466, 1248,  468
                            };  // UNKNOWN C5701A31

String stats="off"; // 存储灯的状态， on 代表开， off 代表关
void handleRoot() {
  digitalWrite(LED_BUILTIN, LOW);
  server.send(200, "text/plain", "这是客厅红外控制的主页!");
  digitalWrite(LED_BUILTIN, HIGH);
}

void handleNotFound() {
  digitalWrite(LED_BUILTIN, LOW);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(LED_BUILTIN, HIGH);
}


  //pinMode(LED_BUILTIN,OUTPUT);  
  //digitalWrite(LED_BUILTIN,HIGH); // close ； open if you set "LOW" 
  //Serial.println("内置LED灯状态：关闭。");
  //Serial.println(LED_BUILTIN);
void setup(void) {   
  // 关闭内置的蓝灯
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH); // close ； open if you set "LOW" 
  
  irsend.begin();
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  //Serial.begin(115200);
    
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("等待连接.");
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi连接完成。");
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  //设置进入模式一：light sleep 模式， 省电  ， 约为1毫安
  wifi_set_sleep_type(LIGHT_SLEEP_T); 
  //Serial.println(wifi_get_sleep_type());
  Serial.println("开启LIGHT SLEEP模式，耗电量约为1毫安...");
  //设置进入模式一：modem sleep 模式， 约为15毫安
  //wifi_set_sleep_type(MODEM_SLEEP_T); 
  //Serial.println(wifi_get_sleep_type());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started.");
  }

  server.on("/", handleRoot);
  /*
   * 请求连接 ， 统一改为 http://{ip}/room?roomid={roomid}&state={state}
   * {ip} 值：192.168.88.9,192.168.88.10,192.168.88.13（预留），此IP是在路由器上已经设定好了，放在哪个房间，路由器会分配响应的IP。其代表意义为： 
   *    192.168.88.9:客厅 ， 其MAC地址：5C:CF:7F:C0:03:24
   *    192.168.88.10:主卧 ， 其MAC地址：18:FE:34:CB:84:8E
   *    192.168.88.13:次卧， 其MAC地址：
   * {roomid}值：keting,zhuwo,ciwo,其代表意义为： 
   *    keting:客厅
   *    zhuwo:主卧
   *    ciwo:次卧
   * {state}值：kai,guan,ming,an,riguangse,nuanbaise,quanliang,changyedeng,stats,其代表意义为： 
   *    kai/on(二选一即可): 开相应房间的灯，例如 http://192.168.88.9/room?roomid=keting&state=kai，将会开启客厅的顶灯
   *    guan/off(二选一即可):关相应房间的灯
   *    ming:把相应房间的灯调亮一级
   *    an:把相应房间的灯调暗一级
   *    riguangse:把相应房间的灯调成日光色
   *    nuanbaise:把相应房间的灯调成暖光色
   *    quanliang:把相应房间的灯调成全亮
   *    changyedeng:把相应房间的灯调成常夜灯，跟关闭差不多
   *    stats: 返回相应房间的状态 
  */
  server.on("/room", []() { // request url should be http://ip/room?roomid=keting&state=kai or http://ip/room?roomid=keting&state=kai
    String state = server.arg("state");
    String roomid = server.arg("roomid") ; 
    /*
       使用if else 去判断字符串,C/C++ SWTICH CASE语句只能判断整型,常量或表达式
    */
    digitalWrite(LED_BUILTIN, LOW);
    if (state == "guan" || state == "off" ) {
      Serial.println(roomid+"：关");      
      stats = "off";
      irsend.sendRaw(guan, 131, 38);
      server.send(200, "text/plain", "off");
    } else if (state == "kai" || state == "on") {
      Serial.println(roomid+"：开");
      stats = "on";
      irsend.sendRaw(kai, 131, 38);
      server.send(200, "text/plain", "on");
    } else if  (state == "ming") {
      Serial.println(roomid+"：明亮一级");
      stats = "on";
      irsend.sendRaw(ming, 131, 38);
      server.send(200, "text/plain", "ming");
    } else if  (state == "an") {
      Serial.println(roomid+"：变暗一级");
      stats = "on";
      irsend.sendRaw(an, 131, 38);
      server.send(200, "text/plain", "an");
    } else if  (state == "riguangse") {
      Serial.println(roomid+"：日光色");
      stats = "on";
      irsend.sendRaw(riguangse, 131, 38);
      server.send(200, "text/plain", "riguangse");
    } else if  (state == "nuanbaise") {
      Serial.println(roomid+"：暖白色");
      stats = "on";
      irsend.sendRaw(nuanbaise, 131, 38);
      server.send(200, "text/plain", "nuanbaise");
    } else if (state == "quanliang") {
      Serial.println(roomid+"：全亮");
      stats = "on";
      irsend.sendRaw(quanliang, 131, 38);
      server.send(200, "text/plain", "quanliang");
    } else if  (state == "changyedeng") {
      Serial.println(roomid+"：常夜灯");
      stats = "off";
      irsend.sendRaw(changyedeng, 131, 38);
      server.send(200, "text/plain", "changyedeng");
    }else if  (state == "stats") {
      Serial.println(roomid+"："+stats);
      // stats: on （开） or off （关）  
      server.send(200, "text/plain", stats);
    }
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
