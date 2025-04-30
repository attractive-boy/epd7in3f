/**
    @filename   :   EPD_7in3f.ino
    @brief      :   EPD_7in3 e-paper F display demo
    @author     :   Waveshare

    Copyright (C) Waveshare     10 21 2022

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documnetation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to  whom the Software is
   furished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/


#include "imagedata.h"
#include "epd7in3f.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// WiFi设置
const char* ssid = "EPD_Display";  // WiFi名称
const char* password = "12345678";  // WiFi密码

// 创建Web服务器实例
WebServer server(80);

// 创建EPD实例
Epd epd;

// 处理图片上传请求
void handleUpload() {
  if (server.method() == HTTP_POST) {
    String imageData = server.arg("imageData");
    
    // 解析JSON数据
    DynamicJsonDocument doc(1024 * 1024);  // 分配1MB内存
    DeserializationError error = deserializeJson(doc, imageData);
    
    if (error) {
      server.send(400, "text/plain", "Invalid JSON data");
      return;
    }
    
    // 获取图片数据数组
    JsonArray array = doc.as<JsonArray>();
    
    // 创建图片缓冲区
    uint8_t* imageBuffer = new uint8_t[array.size()];
    
    // 将JSON数组转换为字节数组
    for (size_t i = 0; i < array.size(); i++) {
      imageBuffer[i] = array[i];
    }
    
    // 显示图片
    epd.EPD_7IN3F_Display_part(imageBuffer, 0, 0, 800, 480);
    
    // 释放内存
    delete[] imageBuffer;
    
    // 发送成功响应
    server.send(200, "text/plain", "Image uploaded successfully");
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void setup() {
  Serial.begin(115200);
  
  // 初始化EPD
  if (epd.Init() != 0) {
    Serial.print("e-Paper init failed");
    return;
  }
  
  // 清屏
  epd.Clear(EPD_7IN3F_WHITE);
  
  // 设置WiFi
  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP started");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  
  // 设置Web服务器路由
  server.on("/upload", HTTP_POST, handleUpload);
  
  // 启动服务器
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
