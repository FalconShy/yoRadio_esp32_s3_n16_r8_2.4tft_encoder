#include "netserver.h"
#include <SPIFFS.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "config.h"
#include "player.h"
#include "telnet.h"
#include "display.h"
#include "options.h"
#include "network.h"
#include "mqtt.h"
#include "controls.h"
#include <Update.h>
#include <ESPmDNS.h>
#ifdef USE_SD
#include "sdmanager.h"
#endif
#ifndef MIN_MALLOC
#define MIN_MALLOC 24112
#endif
#ifndef NSQ_SEND_DELAY
  #define NSQ_SEND_DELAY       (TickType_t)100  //portMAX_DELAY?
#endif

//#define CORS_DEBUG //Enable CORS policy: 'Access-Control-Allow-Origin' (for testing)

NetServer netserver;

AsyncWebServer webserver(80);
AsyncWebSocket websocket("/ws");
AsyncUDP udp;

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
void handleUploadWeb(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
void handleUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
void handleHTTPArgs(AsyncWebServerRequest * request);
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

bool  shouldReboot  = false;
#ifdef MQTT_ROOT_TOPIC
Ticker mqttplaylistticker;
bool  mqttplaylistblock = false;
void mqttplaylistSend() {
  mqttplaylistblock = true;
  mqttplaylistticker.detach();
  mqttPublishPlaylist();
  mqttplaylistblock = false;
}
#endif

char* updateError() {
  static char ret[140] = {0};
  sprintf(ret, "Update failed with error (%d)<br /> %s", (int)Update.getError(), Update.errorString());
  return ret;
}

bool NetServer::begin(bool quiet) {
  if(network.status==SDREADY) return true;
  if(!quiet) Serial.print("##[BOOT]#\tnetserver.begin\t");
  importRequest = IMDONE;
  irRecordEnable = false;
  nsQueue = xQueueCreate( 20, sizeof( nsRequestParams_t ) );
  while(nsQueue==NULL){;}

  if(config.emptyFS){
    webserver.on("/", HTTP_GET, [](AsyncWebServerRequest * request) { request->send_P(200, "text/html", emptyfs_html); });
    webserver.on("/webboard", HTTP_POST, [](AsyncWebServerRequest *request) { request->redirect("/"); ESP.restart(); }, handleUploadWeb);
    webserver.on("/", HTTP_POST, [](AsyncWebServerRequest *request) { 
      if(request->arg("ssid")!="" && request->arg("pass")!=""){
        char buf[BUFLEN];
        memset(buf, 0, BUFLEN);
        snprintf(buf, BUFLEN, "%s\t%s", request->arg("ssid").c_str(), request->arg("pass").c_str());
        request->redirect("/");
        config.saveWifiFromNextion(buf);
        return;
      }
      request->redirect("/"); 
      ESP.restart();
    }, handleUploadWeb);
  }else{
    webserver.on("/", HTTP_ANY, handleHTTPArgs);
    webserver.on("/settings.html", HTTP_GET, handleHTTPArgs);
    webserver.on("/update.html", HTTP_GET, handleHTTPArgs);
    webserver.on("/ir.html", HTTP_GET, handleHTTPArgs);
    webserver.on("/webboard", HTTP_GET, [](AsyncWebServerRequest * request) { request->send_P(200, "text/html", emptyfs_html); });
    webserver.on("/webboard", HTTP_POST, [](AsyncWebServerRequest *request) { request->redirect("/"); }, handleUploadWeb);
  }
  
  webserver.on(PLAYLIST_PATH, HTTP_GET, handleHTTPArgs);
  webserver.on(INDEX_PATH, HTTP_GET, handleHTTPArgs);
  webserver.on(PLAYLIST_SD_PATH, HTTP_GET, handleHTTPArgs);
  webserver.on(INDEX_SD_PATH, HTTP_GET, handleHTTPArgs);
  webserver.on(SSIDS_PATH, HTTP_GET, handleHTTPArgs);
  
  webserver.on("/upload", HTTP_POST, beginUpload, handleUpload);
  webserver.on("/update", HTTP_GET, handleHTTPArgs);
  webserver.on("/update", HTTP_POST, beginUpdate, handleUpdate);
  
  webserver.on("/variables.js", HTTP_GET, [](AsyncWebServerRequest * request) { 
    char varjsbuf[BUFLEN];
    sprintf (varjsbuf, "var yoVersion='%s';\nvar formAction='%s';\nvar playMode='%s';\n", YOVERSION, (network.status == CONNECTED && !config.emptyFS)?"webboard":"", (network.status == CONNECTED)?"player":"ap");
    request->send(200, "text/html", varjsbuf); 
  });

  webserver.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    File script = SPIFFS.open("/www/script.js.gz", "r");
    if (script) {
      auto *response = request->beginResponse(script, "/script.js", "application/javascript");
      response->addHeader("Cache-Control", "no-cache");
      request->send(response);
    } else {
      request->send(SPIFFS, "/www/script.js", "application/javascript");
    }
  });
  webserver.serveStatic("/", SPIFFS, "/www/").setCacheControl("max-age=31536000");
#ifdef CORS_DEBUG
  DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Origin"), F("*"));
  DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Headers"), F("content-type"));
#endif
  webserver.begin();
  if(strlen(config.store.mdnsname)>0)
    MDNS.begin(config.store.mdnsname);
  websocket.onEvent(onWsEvent);
  webserver.addHandler(&websocket);

  //echo -n "helle?" | socat - udp-datagram:255.255.255.255:44490,broadcast
  if (udp.listen(44490)) {
    udp.onPacket([](AsyncUDPPacket packet) {
      if (strcmp((char*)packet.data(), "helle?") == 0)
        packet.println(WiFi.localIP());
    });
  }
  if(!quiet) Serial.println("done");
  return true;
}

void NetServer::beginUpdate(AsyncWebServerRequest *request) {
  shouldReboot = !Update.hasError();
  AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot ? "OK" : updateError());
  response->addHeader("Connection", "close");
  request->send(response);
}

void handleUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index) {
    int target = (request->getParam("updatetarget", true)->value() == "spiffs") ? U_SPIFFS : U_FLASH;
    Serial.printf("Update Start: %s\n", filename.c_str());
    player.sendCommand({PR_STOP, 0});
    display.putRequest(NEWMODE, UPDATING);
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, target)) {
      Update.printError(Serial);
      request->send(200, "text/html", updateError());
    }
  }
  if (!Update.hasError()) {
    if (Update.write(data, len) != len) {
      Update.printError(Serial);
      request->send(200, "text/html", updateError());
    }
  }
  if (final) {
    if (Update.end(true)) {
      Serial.printf("Update Success: %uB\n", index + len);
    } else {
      Update.printError(Serial);
      request->send(200, "text/html", updateError());
    }
  }
}

static constexpr char playlistUploadPath[] = "/data/plupload.tmp";
static constexpr char playlistConvertedPath[] = "/data/plconvert.tmp";
static AsyncWebServerRequest *playlistUpload = nullptr;
static int playlistUploadFd = -1;
static bool playlistUploadComplete = false;
static size_t playlistUploadBytes = 0;
static const char *playlistUploadError = nullptr;
static int playlistUploadErrno = 0;
static uint8_t *playlistRollback = nullptr;
static size_t playlistRollbackSize = 0;

static bool releasePlaylistUpload(bool saved = false) {
  if (playlistUploadFd >= 0) {
    close(playlistUploadFd);
    playlistUploadFd = -1;
  }
  SPIFFS.remove(playlistUploadPath);
  bool restored = true;
  if (!saved && playlistRollback) {
    const String path = String(SPIFFS.mountpoint()) + PLAYLIST_PATH;
    const int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    restored = fd >= 0;
    if (restored) {
      restored = write(fd, playlistRollback, playlistRollbackSize) == static_cast<ssize_t>(playlistRollbackSize) && fsync(fd) == 0;
      if (close(fd) < 0) restored = false;
    }
    if (restored) restored = config.indexPlaylist();
    if (!restored) Serial.println("[PLAYLIST] Could not restore the previous playlist.");
  }
  free(playlistRollback);
  playlistRollback = nullptr;
  playlistUpload = nullptr;
  return restored;
}

void NetServer::beginUpload(AsyncWebServerRequest *request) {
  if (const auto *part = request->getParam("plfile", true, true)) {
    if (playlistUpload != request) {
      request->send(409);
      return;
    }
    String error;
    int status = 507;
    if (playlistUploadError) {
      error = playlistUploadError;
      if (playlistUploadErrno) error += " (" + String(strerror(playlistUploadErrno)) + ")";
      if (config.getMode() == PM_SDCARD) status = 422;
    } else if (playlistUploadComplete && playlistUploadBytes == part->size()) {
      status = netserver.importPlaylist(playlistUploadBytes, error);
    } else {
      error = "Incomplete playlist upload: received " + String(playlistUploadBytes) +
              " of " + String(part->size()) + " bytes.";
    }
    if (status == 507) {
      error += " SPIFFS used " + String(SPIFFS.usedBytes()) + " of " + String(SPIFFS.totalBytes()) + " bytes.";
    }
    if (!releasePlaylistUpload(status == 200)) error += " Could not restore the previous playlist.";
    if (status != 200) Serial.printf("[PLAYLIST] %d: %s\n", status, error.c_str());
    request->send(status, "text/plain", error);
  } else if (request->hasParam("wifile", true, true)) {
    netserver.importRequest = IMWIFI;
    request->send(200);
  } else {
    request->send(404);
  }
}

#ifndef DSP_NOT_FLIPPED
  #define DSP_CAN_FLIPPED true
#else
  #define DSP_CAN_FLIPPED false
#endif
#if !defined(HIDE_WEATHER) && (!defined(DUMMYDISPLAY) && !defined(USE_NEXTION))
  #define SHOW_WEATHER  true
#else
  #define SHOW_WEATHER  false
#endif

#ifndef NS_QUEUE_TICKS
  #define NS_QUEUE_TICKS 0
#endif

const char *getFormat(BitrateFormat _format) {
  switch (_format) {
    case BF_MP3:  return "MP3";
    case BF_AAC:  return "AAC";
    case BF_FLAC: return "FLC";
    case BF_OGG:  return "OGG";
    case BF_WAV:  return "WAV";
    default:      return "bitrate";
  }
}

char wsbuf[BUFLEN * 2];
void NetServer::processQueue(){
  if(nsQueue==NULL) return;
  nsRequestParams_t request;
  if(xQueueReceive(nsQueue, &request, NS_QUEUE_TICKS)){
    memset(wsbuf, 0, BUFLEN * 2);
    uint8_t clientId = request.clientId;
    switch (request.type) {
      case PLAYLIST:        getPlaylist(clientId); break;
      case PLAYLISTSAVED:   {
        #ifdef USE_SD
        if(config.getMode()==PM_SDCARD) {
        //  config.indexSDPlaylist();
          config.initSDPlaylist();
        }
        #endif
        if(config.getMode()==PM_WEB){
          config.indexPlaylist(); 
          config.initPlaylist(); 
        }
        getPlaylist(clientId); break;
      }
      case GETACTIVE: {
          bool dbgact = false, nxtn=false;
          String act = F("\"group_wifi\",");
          if (network.status == CONNECTED) {
                                                                act += F("\"group_system\",");
            if (BRIGHTNESS_PIN != 255 || DSP_CAN_FLIPPED || DSP_MODEL == DSP_NOKIA5110 || dbgact)    act += F("\"group_display\",");
          #ifdef USE_NEXTION
                                                                act += F("\"group_nextion\",");
            if (!SHOW_WEATHER || dbgact)                        act += F("\"group_weather\",");
            nxtn=true;
          #endif
                                                              #if defined(LCD_I2C) || defined(DSP_OLED)
                                                                act += F("\"group_oled\",");
                                                              #endif
                                                              #ifndef HIDE_VU
                                                                act += F("\"group_vu\",");
                                                              #endif
            if (BRIGHTNESS_PIN != 255 || nxtn || dbgact)                act += F("\"group_brightness\",");
            if (DSP_CAN_FLIPPED || dbgact)                      act += F("\"group_tft\",");
            if (TS_MODEL != TS_MODEL_UNDEFINED || dbgact)       act += F("\"group_touch\",");
            if (DSP_MODEL == DSP_NOKIA5110)                     act += F("\"group_nokia\",");
                                                                act += F("\"group_timezone\",");
            if (SHOW_WEATHER || dbgact)                         act += F("\"group_weather\",");
                                                                act += F("\"group_controls\",");
            if (ENC_BTNL != 255 || ENC2_BTNL != 255 || dbgact)  act += F("\"group_encoder\",");
            if (IR_PIN != 255 || dbgact)                        act += F("\"group_ir\",");
          }
                                                                act = act.substring(0, act.length() - 1);
          sprintf (wsbuf, "{\"act\":[%s]}", act.c_str());
          break;
        }
      //case STARTUP:       sprintf (wsbuf, "{\"command\":\"startup\", \"payload\": {\"mode\":\"%s\", \"version\":\"%s\"}}", network.status == CONNECTED ? "player" : "ap", YOVERSION); break;
      case GETINDEX:      {
          requestOnChange(STATION, clientId); 
          requestOnChange(TITLE, clientId); 
          requestOnChange(VOLUME, clientId); 
          requestOnChange(EQUALIZER, clientId); 
          requestOnChange(BALANCE, clientId); 
          requestOnChange(BITRATE, clientId); 
          requestOnChange(MODE, clientId); 
          requestOnChange(SDINIT, clientId);
          requestOnChange(GETPLAYERMODE, clientId); 
          if (config.getMode()==PM_SDCARD) { requestOnChange(SDPOS, clientId); requestOnChange(SDLEN, clientId); requestOnChange(SDSNUFFLE, clientId); } 
          return; 
          break;
        }
      case GETSYSTEM:     sprintf (wsbuf, "{\"sst\":%d,\"aif\":%d,\"vu\":%d,\"softr\":%d,\"vut\":%d,\"mdns\":\"%s\"}", 
                                  config.store.smartstart != 2, 
                                  config.store.audioinfo, 
                                  config.store.vumeter, 
                                  config.store.softapdelay,
                                  config.vuThreshold,
                                  config.store.mdnsname); 
                                  break;
      case GETSCREEN:     sprintf (wsbuf, "{\"flip\":%d,\"inv\":%d,\"nump\":%d,\"tsf\":%d,\"tsd\":%d,\"dspon\":%d,\"br\":%d,\"con\":%d,\"scre\":%d,\"scrt\":%d,\"scrb\":%d,\"scrpe\":%d,\"scrpt\":%d,\"scrpb\":%d}", 
                                  config.store.flipscreen, 
                                  config.store.invertdisplay, 
                                  config.store.numplaylist, 
                                  config.store.fliptouch, 
                                  config.store.dbgtouch, 
                                  config.store.dspon, 
                                  config.store.brightness, 
                                  config.store.contrast,
                                  config.store.screensaverEnabled,
                                  config.store.screensaverTimeout,
                                  config.store.screensaverBlank,
                                  config.store.screensaverPlayingEnabled,
                                  config.store.screensaverPlayingTimeout,
                                  config.store.screensaverPlayingBlank);
                                  break;
      case GETTIMEZONE:   sprintf (wsbuf, "{\"tzstr\":\"%s\",\"sntp1\":\"%s\",\"sntp2\":\"%s\"}", 
                                  config.store.tzstr, 
                                  config.store.sntp1, 
                                  config.store.sntp2); 
                                  break;
      case GETWEATHER:    sprintf (wsbuf, "{\"wen\":%d,\"wlat\":\"%s\",\"wlon\":\"%s\",\"wkey\":\"%s\"}", 
                                  config.store.showweather, 
                                  config.store.weatherlat, 
                                  config.store.weatherlon, 
                                  config.store.weatherkey); 
                                  break;
      case GETCONTROLS:   sprintf (wsbuf, "{\"vols\":%d,\"enca\":%d,\"irtl\":%d,\"skipup\":%d}", 
                                  config.store.volsteps, 
                                  config.store.encacc, 
                                  config.store.irtlp,
                                  config.store.skipPlaylistUpDown); 
                                  break;
      case DSPON:         sprintf (wsbuf, "{\"dspontrue\":%d}", 1); break;
      case STATION:       requestOnChange(STATIONNAME, clientId); requestOnChange(ITEM, clientId); break;
      case STATIONNAME:   sprintf (wsbuf, "{\"payload\":[{\"id\":\"nameset\", \"value\": \"%s\"}]}", config.station.name); break;
      case ITEM:          sprintf (wsbuf, "{\"current\": %d}", config.lastStation()); break;
      case TITLE:         sprintf (wsbuf, "{\"payload\":[{\"id\":\"meta\", \"value\": \"%s\"}]}", config.station.title); telnet.printf("##CLI.META#: %s\n> ", config.station.title); break;
      case VOLUME:        sprintf (wsbuf, "{\"payload\":[{\"id\":\"volume\", \"value\": %d}]}", config.store.volume); telnet.printf("##CLI.VOL#: %d\n", config.store.volume); break;
      case NRSSI:         sprintf (wsbuf, "{\"payload\":[{\"id\":\"rssi\", \"value\": %d}]}", rssi); /*rssi = 255;*/ break;
      case SDPOS:         sprintf (wsbuf, "{\"sdpos\": %d,\"sdend\": %d,\"sdtpos\": %d,\"sdtend\": %d}", 
                                  player.getFilePos(), 
                                  player.getFileSize(), 
                                  player.getAudioCurrentTime(), 
                                  player.getAudioFileDuration()); 
                                  break;
      case SDLEN:         sprintf (wsbuf, "{\"sdmin\": %d,\"sdmax\": %d}", player.sd_min, player.sd_max); break;
      case SDSNUFFLE:     sprintf (wsbuf, "{\"snuffle\": %d}", config.store.sdsnuffle); break;
      case BITRATE:       sprintf (wsbuf, "{\"payload\":[{\"id\":\"bitrate\", \"value\": %d}, {\"id\":\"fmt\", \"value\": \"%s\"}]}", config.station.bitrate, getFormat(config.configFmt)); break;
      case MODE:          sprintf (wsbuf, "{\"payload\":[{\"id\":\"playerwrap\", \"value\": \"%s\"}]}", player.status() == PLAYING ? "playing" : "stopped"); telnet.info(); break;
      case EQUALIZER:     sprintf (wsbuf, "{\"payload\":[{\"id\":\"bass\", \"value\": %d}, {\"id\": \"middle\", \"value\": %d}, {\"id\": \"trebble\", \"value\": %d}]}", config.store.bass, config.store.middle, config.store.trebble); break;
      case BALANCE:       sprintf (wsbuf, "{\"payload\":[{\"id\": \"balance\", \"value\": %d}]}", config.store.balance); break;
      case SDINIT:        sprintf (wsbuf, "{\"sdinit\": %d}", SDC_CS!=255); break;
      case GETPLAYERMODE: sprintf (wsbuf, "{\"playermode\": \"%s\"}", config.getMode()==PM_SDCARD?"modesd":"modeweb"); break;
      #ifdef USE_SD
        case CHANGEMODE:    config.changeMode(newConfigMode); return; break;
      #endif
      default:          break;
    }
    if (strlen(wsbuf) > 0) {
      if (clientId == 0) { websocket.textAll(wsbuf); }else{ websocket.text(clientId, wsbuf); }
  #ifdef MQTT_ROOT_TOPIC
      if (clientId == 0 && (request.type == STATION || request.type == ITEM || request.type == TITLE || request.type == MODE)) mqttPublishStatus();
      if (clientId == 0 && request.type == VOLUME) mqttPublishVolume();
  #endif
    }
  }
}

void NetServer::loop() {
  if(network.status==SDREADY) return;
  if (shouldReboot) {
    Serial.println("Rebooting...");
    delay(100);
    ESP.restart();
  }
  websocket.cleanupClients();
  switch (importRequest) {
    case IMWIFI:  config.saveWifi(); importRequest = IMDONE; break;
    default:      break;
  }
  //if (rssi < 255) requestOnChange(NRSSI, 0);
  processQueue();
}

#if IR_PIN!=255
void NetServer::irToWs(const char* protocol, uint64_t irvalue) {
  char buf[BUFLEN] = { 0 };
  sprintf (buf, "{\"ircode\": %llu, \"protocol\": \"%s\"}", irvalue, protocol);
  websocket.textAll(buf);
}
void NetServer::irValsToWs() {
  if (!irRecordEnable) return;
  char buf[BUFLEN] = { 0 };
  sprintf (buf, "{\"irvals\": [%llu, %llu, %llu]}", config.ircodes.irVals[config.irindex][0], config.ircodes.irVals[config.irindex][1], config.ircodes.irVals[config.irindex][2]);
  websocket.textAll(buf);
}
#endif

void NetServer::onWsMessage(void *arg, uint8_t *data, size_t len, uint8_t clientId) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    char cmd[65], val[65];
    if (config.parseWsCommand((const char*)data, cmd, val, 65)) {
      //if (strcmp(cmd, "getmode") == 0     ) { requestOnChange(GETMODE, clientId);     return; }
      if (strcmp(cmd, "getindex") == 0    ) { requestOnChange(GETINDEX, clientId);    return; }
      if (strcmp(cmd, "getsystem") == 0   ) { requestOnChange(GETSYSTEM, clientId);   return; }
      if (strcmp(cmd, "getscreen") == 0   ) { requestOnChange(GETSCREEN, clientId);   return; }
      if (strcmp(cmd, "gettimezone") == 0 ) { requestOnChange(GETTIMEZONE, clientId); return; }
      if (strcmp(cmd, "getcontrols") == 0 ) { requestOnChange(GETCONTROLS, clientId); return; }
      if (strcmp(cmd, "getweather") == 0  ) { requestOnChange(GETWEATHER, clientId);  return; }
      if (strcmp(cmd, "getactive") == 0   ) { requestOnChange(GETACTIVE, clientId);   return; }
      if (strcmp(cmd, "newmode") == 0     ) { newConfigMode = atoi(val); requestOnChange(CHANGEMODE, 0); return; }
      if (strcmp(cmd, "smartstart") == 0) {
        uint8_t valb = atoi(val);
        uint8_t ss = valb == 1 ? 1 : 2;
        if (!player.isRunning() && ss == 1) ss = 0;
        config.setSmartStart(ss);
        return;
      }
      if (strcmp(cmd, "audioinfo") == 0) {
        bool valb = static_cast<bool>(atoi(val));
        config.saveValue(&config.store.audioinfo, valb);
        display.putRequest(AUDIOINFO);
        return;
      }
      if (strcmp(cmd, "vumeter") == 0) {
        bool valb = static_cast<bool>(atoi(val));
        config.saveValue(&config.store.vumeter, valb);
        display.putRequest(SHOWVUMETER);
        return;
      }
      if (strcmp(cmd, "prev") == 0) { player.prev(); return; }
      if (strcmp(cmd, "toggle") == 0) { player.toggle(); return; }
      if (strcmp(cmd, "next") == 0) { player.next(); return; }
      if (strcmp(cmd, "volm") == 0) { player.stepVol(false); return; }
      if (strcmp(cmd, "volp") == 0) { player.stepVol(true); return; }
      if (strcmp(cmd, "play") == 0) { uint16_t valb = atoi(val); player.sendCommand({PR_PLAY, valb}); return; }
      if (strcmp(cmd, "softap") == 0) {
        uint8_t valb = atoi(val);
        config.saveValue(&config.store.softapdelay, valb);
        return;
      }
      if (strcmp(cmd, "mdnsname") == 0) {
        config.saveValue(config.store.mdnsname, val, MDNS_LENGTH);
        return;
      }
      if (strcmp(cmd, "rebootmdns") == 0) {
        char buf[MDNS_LENGTH*2];
        if(strlen(config.store.mdnsname)>0)
          snprintf(buf, MDNS_LENGTH*2, "{\"redirect\": \"http://%s.local\"}", config.store.mdnsname);
        else
          snprintf(buf, MDNS_LENGTH*2, "{\"redirect\": \"http://%s/\"}", WiFi.localIP().toString().c_str());
        websocket.text(clientId, buf);
        delay(500);
        ESP.restart();
        return;
      }
      if (strcmp(cmd, "invertdisplay") == 0) {
        bool valb = static_cast<bool>(atoi(val));
        config.saveValue(&config.store.invertdisplay, valb);
        display.invert();
        return;
      }
      if (strcmp(cmd, "numplaylist") == 0) {
        bool valb = static_cast<bool>(atoi(val));
        config.saveValue(&config.store.numplaylist, valb);
        display.putRequest(NEWMODE, CLEAR); display.putRequest(NEWMODE, PLAYER);
        return;
      }
      if (strcmp(cmd, "fliptouch") == 0) {
        bool valb = static_cast<bool>(atoi(val));
        config.saveValue(&config.store.fliptouch, valb);
        flipTS();
        return;
      }
      if (strcmp(cmd, "dbgtouch") == 0) {
        bool valb = static_cast<bool>(atoi(val));
        config.saveValue(&config.store.dbgtouch, valb);
        return;
      }
      if (strcmp(cmd, "flipscreen") == 0) {
        bool valb = static_cast<bool>(atoi(val));
        config.saveValue(&config.store.flipscreen, valb);
        display.flip();
        display.putRequest(NEWMODE, CLEAR); display.putRequest(NEWMODE, PLAYER);
        return;
      }
      if (strcmp(cmd, "brightness") == 0) {
        uint8_t valb = atoi(val);
        if (!config.store.dspon) requestOnChange(DSPON, 0);
        config.store.brightness = valb;
        config.setBrightness(true);
        return;
      }
      if (strcmp(cmd, "screenon") == 0) {
        bool valb = static_cast<bool>(atoi(val));
        config.setDspOn(valb);
        return;
      }
      if (strcmp(cmd, "contrast") == 0) {
        uint8_t valb = atoi(val);
        config.saveValue(&config.store.contrast, valb);
        display.setContrast();
        return;
      }
      if (strcmp(cmd, "screensaverenabled") == 0) {
        bool valb = static_cast<bool>(atoi(val));
        config.saveValue(&config.store.screensaverEnabled, valb);
        #ifndef DSP_LCD
        display.putRequest(NEWMODE, PLAYER);
        #endif
        return;
      }
      if (strcmp(cmd, "screensavertimeout") == 0) {
        uint16_t valb = atoi(val);
        valb = constrain(valb,5,65520);
        config.saveValue(&config.store.screensaverTimeout, valb);
        #ifndef DSP_LCD
        display.putRequest(NEWMODE, PLAYER);
        #endif
        return;
      }
      if (strcmp(cmd, "screensaverblank") == 0) {
        bool valb = static_cast<bool>(atoi(val));
        config.saveValue(&config.store.screensaverBlank, valb);
        #ifndef DSP_LCD
        display.putRequest(NEWMODE, PLAYER);
        #endif
        return;
      }
      if (strcmp(cmd, "screensaverplayingenabled") == 0) {
        bool valb = static_cast<bool>(atoi(val));
        config.saveValue(&config.store.screensaverPlayingEnabled, valb);
        #ifndef DSP_LCD
        display.putRequest(NEWMODE, PLAYER);
        #endif
        return;
      }
      if (strcmp(cmd, "screensaverplayingtimeout") == 0) {
        uint16_t valb = atoi(val);
        valb = constrain(valb,1,1080);
        config.saveValue(&config.store.screensaverPlayingTimeout, valb);
        #ifndef DSP_LCD
        display.putRequest(NEWMODE, PLAYER);
        #endif
        return;
      }
      if (strcmp(cmd, "screensaverplayingblank") == 0) {
        bool valb = static_cast<bool>(atoi(val));
        config.saveValue(&config.store.screensaverPlayingBlank, valb);
        #ifndef DSP_LCD
        display.putRequest(NEWMODE, PLAYER);
        #endif
        return;
      }
      if (strcmp(cmd, "tzstr") == 0) {
        config.saveValue(config.store.tzstr, val, 55, false);
        return;
      }
      if (strcmp(cmd, "sntp2") == 0) {
        config.saveValue(config.store.sntp2, val, 35, false);
        return;
      }
      if (strcmp(cmd, "sntp1") == 0) {
        bool tzdone = false;
        if (strlen(val) > 0 && strlen(config.store.sntp2) > 0) {
          configTzTime(config.store.tzstr, val, config.store.sntp2);
          tzdone = true;
        } else if (strlen(val) > 0) {
          configTzTime(config.store.tzstr, val);
          tzdone = true;
        }
        if (tzdone) {
          network.forceTimeSync = true;
          config.saveValue(config.store.sntp1, val, 35);
        }
        return;
      }
      if (strcmp(cmd, "volsteps") == 0) {
        uint8_t valb = atoi(val);
        config.saveValue(&config.store.volsteps, valb);
        return;
      }
      if (strcmp(cmd, "encacceleration") == 0) {
        uint16_t valb = atoi(val);
        setEncAcceleration(valb);
        config.saveValue(&config.store.encacc, valb);
        return;
      }
      if (strcmp(cmd, "irtlp") == 0) {
        uint8_t valb = atoi(val);
        setIRTolerance(valb);
        return;
      }
      if (strcmp(cmd, "oneclickswitching") == 0) {
        bool valb = static_cast<bool>(atoi(val));
        config.saveValue(&config.store.skipPlaylistUpDown, valb);
        return;
      }
      if (strcmp(cmd, "showweather") == 0) {
        bool valb = static_cast<bool>(atoi(val));
        config.saveValue(&config.store.showweather, valb);
        network.trueWeather=false;
        network.forceWeather = true;
        display.putRequest(SHOWWEATHER);
        return;
      }
      if (strcmp(cmd, "lat") == 0) {
        config.saveValue(config.store.weatherlat, val, 10, false);
        return;
      }
      if (strcmp(cmd, "lon") == 0) {
        config.saveValue(config.store.weatherlon, val, 10, false);
        return;
      }
      if (strcmp(cmd, "key") == 0) {
        config.saveValue(config.store.weatherkey, val, WEATHERKEY_LENGTH);
        network.trueWeather=false;
        display.putRequest(NEWMODE, CLEAR); display.putRequest(NEWMODE, PLAYER);
        return;
      }
      /*  RESETS  */
      if (strcmp(cmd, "reset") == 0) {
        if (strcmp(val, "system") == 0) {
          config.saveValue(&config.store.smartstart, (uint8_t)2, false);
          config.saveValue(&config.store.audioinfo, false, false);
          config.saveValue(&config.store.vumeter, false, false);
          config.saveValue(&config.store.softapdelay, (uint8_t)0, false);
          snprintf(config.store.mdnsname, MDNS_LENGTH, "yoradio-%x", config.getChipId());
          config.saveValue(config.store.mdnsname, config.store.mdnsname, MDNS_LENGTH, true, true);
          display.putRequest(NEWMODE, CLEAR); display.putRequest(NEWMODE, PLAYER);
          requestOnChange(GETSYSTEM, clientId);
          return;
        }
        if (strcmp(val, "screen") == 0) {
          config.saveValue(&config.store.flipscreen, false, false);
          display.flip();
          config.saveValue(&config.store.invertdisplay, false, false);
          display.invert();
          config.saveValue(&config.store.dspon, true, false);
          config.store.brightness = 100;
          config.setBrightness(false);
          config.saveValue(&config.store.contrast, (uint8_t)55, false);
          display.setContrast();
          config.saveValue(&config.store.numplaylist, false);
          config.saveValue(&config.store.screensaverEnabled, false);
          config.saveValue(&config.store.screensaverTimeout, (uint16_t)20);
          config.saveValue(&config.store.screensaverBlank, false);
          config.saveValue(&config.store.screensaverPlayingEnabled, false);
          config.saveValue(&config.store.screensaverPlayingTimeout, (uint16_t)5);
          config.saveValue(&config.store.screensaverPlayingBlank, false);
          display.putRequest(NEWMODE, CLEAR); display.putRequest(NEWMODE, PLAYER);
          requestOnChange(GETSCREEN, clientId);
          return;
        }
        if (strcmp(val, "timezone") == 0) {
          config.saveValue(config.store.tzstr, "CET-1CEST,M3.5.0,M10.5.0/3", 55, false);
          config.saveValue(config.store.sntp1, "tempus1.gum.gov.pl", 35, false);
          config.saveValue(config.store.sntp2, "tempus2.gum.gov.pl", 35);
          configTzTime(config.store.tzstr, config.store.sntp1, config.store.sntp2);
          network.forceTimeSync = true;
          requestOnChange(GETTIMEZONE, clientId);
          return;
        }
        if (strcmp(val, "weather") == 0) {
          config.saveValue(&config.store.showweather, false, false);
          config.saveValue(config.store.weatherlat, "55.7512", 10, false);
          config.saveValue(config.store.weatherlon, "37.6184", 10, false);
          config.saveValue(config.store.weatherkey, "", WEATHERKEY_LENGTH);
          network.trueWeather=false;
          display.putRequest(NEWMODE, CLEAR); display.putRequest(NEWMODE, PLAYER);
          requestOnChange(GETWEATHER, clientId);
          return;
        }
        if (strcmp(val, "controls") == 0) {
          config.saveValue(&config.store.volsteps, (uint8_t)1, false);
          config.saveValue(&config.store.fliptouch, false, false);
          config.saveValue(&config.store.dbgtouch, false, false);
          config.saveValue(&config.store.skipPlaylistUpDown, false);
          setEncAcceleration(200);
          setIRTolerance(40);
          requestOnChange(GETCONTROLS, clientId);
          return;
        }
        if (strcmp(val, "1") == 0) {
          config.reset();
          return;
        }
      } /*  EOF RESETS  */
      if (strcmp(cmd, "volume") == 0) {
        uint8_t v = atoi(val);
        player.setVol(v);
      }
      if (strcmp(cmd, "sdpos") == 0) {
        //return;
        if (config.getMode()==PM_SDCARD){
          config.sdResumePos = 0;
          if(!player.isRunning()){
            player.setResumeFilePos(atoi(val)-player.sd_min);
            player.sendCommand({PR_PLAY, config.store.lastSdStation});
          }else{
            player.setFilePos(atoi(val)-player.sd_min);
          }
        }
        return;
      }
      if (strcmp(cmd, "snuffle") == 0) {
        config.setSnuffle(strcmp(val, "true") == 0);
        return;
      }
      if (strcmp(cmd, "balance") == 0) {
        int8_t valb = atoi(val);
        player.setBalance(valb);
        config.setBalance(valb);
        netserver.requestOnChange(BALANCE, 0);
        return;
      }
      if (strcmp(cmd, "trebble") == 0) {
        int8_t valb = atoi(val);
        player.setTone(config.store.bass, config.store.middle, valb);
        config.setTone(config.store.bass, config.store.middle, valb);
        netserver.requestOnChange(EQUALIZER, 0);
        return;
      }
      if (strcmp(cmd, "middle") == 0) {
        int8_t valb = atoi(val);
        player.setTone(config.store.bass, valb, config.store.trebble);
        config.setTone(config.store.bass, valb, config.store.trebble);
        netserver.requestOnChange(EQUALIZER, 0);
        return;
      }
      if (strcmp(cmd, "bass") == 0) {
        int8_t valb = atoi(val);
        player.setTone(valb, config.store.middle, config.store.trebble);
        config.setTone(valb, config.store.middle, config.store.trebble);
        netserver.requestOnChange(EQUALIZER, 0);
        return;
      }
      if (strcmp(cmd, "reboot") == 0) {
        ESP.restart();
        return;
      }
      if (strcmp(cmd, "format") == 0) {
        SPIFFS.format();
        ESP.restart();
        return;
      }
      if (strcmp(cmd, "submitplaylist") == 0) {
        return;
      }
      if (strcmp(cmd, "submitplaylistdone") == 0) {
#ifdef MQTT_ROOT_TOPIC
        //mqttPublishPlaylist();
        mqttplaylistticker.attach(5, mqttplaylistSend);
#endif
        if (player.isRunning()) {
          player.sendCommand({PR_PLAY, -config.lastStation()});
        }
        return;
      }
#if IR_PIN!=255
      if (strcmp(cmd, "irbtn") == 0) {
        config.irindex = atoi(val);
        irRecordEnable = (config.irindex >= 0);
        config.irchck = 0;
        irValsToWs();
        if (config.irindex < 0) config.saveIR();
      }
      if (strcmp(cmd, "chkid") == 0) {
        config.irchck = atoi(val);
      }
      if (strcmp(cmd, "irclr") == 0) {
        uint8_t cl = atoi(val);
        config.ircodes.irVals[config.irindex][cl] = 0;
      }
#endif
    }
  }
}

void NetServer::getPlaylist(uint8_t clientId) {
  char buf[160] = {0};
  sprintf(buf, "{\"file\": \"http://%s%s\"}", WiFi.localIP().toString().c_str(), PLAYLIST_PATH);
  if (clientId == 0) { websocket.textAll(buf); } else { websocket.text(clientId, buf); }
}

int NetServer::_readPlaylistLine(File &file, char * line, size_t size){
  int bytesRead = file.readBytesUntil('\n', line, size);
  line[bytesRead] = 0;
  if(bytesRead>0){
    if(line[bytesRead-1]=='\r') line[bytesRead-1]=0;
  }
  return bytesRead;
}

int NetServer::importPlaylist(size_t expectedSize, String &error) {
  if (config.getMode() == PM_SDCARD) {
    error = "Switch from SD card to Web Radio mode before saving stations.";
    return 422;
  }
  File tempfile = SPIFFS.open(playlistUploadPath, "r");
  if (!tempfile) {
    error = "Could not reopen the uploaded playlist in SPIFFS.";
    return 507;
  }
  if (tempfile.size() != expectedSize) {
    error = "SPIFFS stored " + String(tempfile.size()) + " of " + String(expectedSize) + " playlist bytes.";
    return 507;
  }
  char sName[BUFLEN], sUrl[BUFLEN], linePl[BUFLEN*3];
  int sOvol;
  const char *replacement = playlistUploadPath;
  const int firstLength = _readPlaylistLine(tempfile, linePl, sizeof(linePl)-1);
  if (firstLength >= sizeof(linePl)-1) {
    error = "Playlist line 1 is too long.";
    return 422;
  }
  if (config.parseCSV(linePl, sName, sUrl, sOvol)) {
    unsigned int row = 1;
    while (tempfile.available()) {
      ++row;
      int length = _readPlaylistLine(tempfile, linePl, sizeof(linePl)-1);
      if (length >= sizeof(linePl)-1 || (length && *linePl && !config.parseCSV(linePl, sName, sUrl, sOvol))) {
        error = "Invalid playlist line " + String(row) + ". Expected name, URL and volume separated by tabs.";
        return 422;
      }
    }
  } else if (config.parseJSON(linePl, sName, sUrl, sOvol)) {
    File converted = SPIFFS.open(playlistConvertedPath, "w");
    if (!converted) {
      error = "Could not create a converted playlist in SPIFFS.";
      return 507;
    }
    bool complete = true;
    bool nextStation;
    size_t expectedSize = 0;
    do {
      int length = snprintf(linePl, sizeof(linePl), "%s\t%s\t0\n", sName, sUrl);
      if (length < 0 || length >= sizeof(linePl) || converted.write((const uint8_t *)linePl, length) != length) {
        complete = false;
        break;
      }
      expectedSize += length;
      nextStation = false;
      while (tempfile.available()) {
        int read = _readPlaylistLine(tempfile, linePl, sizeof(linePl)-1);
        if (read >= sizeof(linePl)-1) complete = false;
        if (!complete) break;
        if (read && config.parseJSON(linePl, sName, sUrl, sOvol)) {
          nextStation = true;
          break;
        }
      }
    } while (nextStation);
    converted.flush();
    complete = complete && converted.size() == expectedSize;
    converted.close();
    if (!complete) {
      SPIFFS.remove(playlistConvertedPath);
      error = "Could not finish converting the playlist. Check the file and SPIFFS space.";
      return 507;
    }
    replacement = playlistConvertedPath;
  } else {
    error = "Invalid playlist line 1. Expected name, URL and volume separated by tabs, or a legacy JSON record.";
    return 422;
  }
  tempfile.close();
  const bool hadPlaylist = SPIFFS.exists(PLAYLIST_PATH);
  // Keep the previous playlist until the replacement and its index are usable.
  const bool backedUp = !hadPlaylist ||
    ((!SPIFFS.exists(PLAYLIST_BACKUP) || SPIFFS.remove(PLAYLIST_BACKUP)) &&
     SPIFFS.rename(PLAYLIST_PATH, PLAYLIST_BACKUP));
  if (!backedUp || !SPIFFS.rename(replacement, PLAYLIST_PATH)) {
    if (backedUp && hadPlaylist) SPIFFS.rename(PLAYLIST_BACKUP, PLAYLIST_PATH);
    if (replacement == playlistConvertedPath) SPIFFS.remove(playlistConvertedPath);
    error = backedUp ? "SPIFFS could not move the uploaded file into place." : "SPIFFS could not back up the current playlist.";
    return 507;
  }
  if (!config.indexPlaylist()) {
    SPIFFS.remove(PLAYLIST_PATH);
    if (hadPlaylist && SPIFFS.rename(PLAYLIST_BACKUP, PLAYLIST_PATH)) config.indexPlaylist();
    error = "SPIFFS could not write the station index. Check free space.";
    return 507;
  }
  if (SPIFFS.exists(PLAYLIST_BACKUP)) SPIFFS.remove(PLAYLIST_BACKUP);
  requestOnChange(PLAYLIST, 0);
  return 200;
}

void NetServer::requestOnChange(requestType_e request, uint8_t clientId) {
  if(nsQueue==NULL) return;
  nsRequestParams_t nsrequest;
  nsrequest.type = request;
  nsrequest.clientId = clientId;
  xQueueSend(nsQueue, &nsrequest, NSQ_SEND_DELAY);
}

void NetServer::resetQueue(){
  if(nsQueue!=NULL) xQueueReset(nsQueue);
}

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (filename == "tempwifi.csv") {
    if (!index) request->_tempFile = SPIFFS.open(TMP_PATH, "w");
    if (len && request->_tempFile) request->_tempFile.write(data, len);
    if (final) request->_tempFile.close();
    return;
  }
  if (!index && !playlistUpload) {
    playlistUpload = request;
    playlistUploadComplete = false;
    playlistUploadBytes = 0;
    playlistUploadError = nullptr;
    playlistUploadErrno = 0;
    request->onDisconnect([request]() {
      if (playlistUpload == request) releasePlaylistUpload();
    });
    if (config.getMode() == PM_SDCARD) {
      playlistUploadError = "Switch from SD card to Web Radio mode before saving stations.";
      return;
    }
    if (!len || memchr(data, '\0', len)) {
      playlistUploadError = "Playlist must contain UTF-8 text.";
      return;
    }
    // Bypass File's stdio buffer so that flash write failures are visible here.
    const String path = String(SPIFFS.mountpoint()) + playlistUploadPath;
    playlistUploadFd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (playlistUploadFd < 0) {
      playlistUploadErrno = errno;
      playlistUploadError = "SPIFFS could not create the temporary playlist file.";
      return;
    }
    // Reclaim flash as in the pre-30_03 version, with a RAM copy for rollback.
    if (SPIFFS.exists(PLAYLIST_PATH)) {
      File previous = SPIFFS.open(PLAYLIST_PATH, "r");
      const size_t size = previous ? previous.size() : 0;
      uint8_t *backup = size ? static_cast<uint8_t *>(psramFound() ? ps_malloc(size) : malloc(size)) : nullptr;
      const bool copied = previous && (!size || (backup && previous.read(backup, size) == size));
      previous.close();
      if (!copied || !SPIFFS.remove(PLAYLIST_PATH)) {
        free(backup);
        playlistUploadError = "Could not safely release the old playlist's SPIFFS space.";
        close(playlistUploadFd);
        playlistUploadFd = -1;
        return;
      }
      playlistRollback = backup;
      playlistRollbackSize = size;
    }
    if (SPIFFS.exists(INDEX_PATH)) SPIFFS.remove(INDEX_PATH);
  }
  if (playlistUpload != request || playlistUploadFd < 0) return;
  if (index != playlistUploadBytes) {
    playlistUploadError = "Playlist upload chunks arrived out of order.";
  } else if (len && memchr(data, '\0', len)) {
    playlistUploadError = "Playlist contains a NUL byte. Save it as UTF-8 text.";
  } else if (len) {
    const ssize_t written = write(playlistUploadFd, data, len);
    if (written > 0) playlistUploadBytes += written;
    if (written < 0 || static_cast<size_t>(written) != len) {
      if (written < 0) playlistUploadErrno = errno;
      playlistUploadError = "SPIFFS could not write the temporary playlist.";
    }
  }
  if (playlistUploadError || final) {
    if (!playlistUploadError && fsync(playlistUploadFd) < 0) {
      playlistUploadErrno = errno;
      playlistUploadError = "SPIFFS could not flush the temporary playlist to flash.";
    }
    if (close(playlistUploadFd) < 0 && !playlistUploadError) {
      playlistUploadErrno = errno;
      playlistUploadError = "SPIFFS could not close the temporary playlist.";
    }
    playlistUploadFd = -1;
    playlistUploadComplete = final && !playlistUploadError;
  }
}

void handleUploadWeb(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  DBGVB("File: %s, size:%u bytes, index: %u, final: %s\n", filename.c_str(), len, index, final?"true":"false");
  if (!index) {
    String spath = "/www/";
    if(filename=="playlist.csv" || filename=="wifi.csv") spath = "/data/";
    request->_tempFile = SPIFFS.open(spath + filename , "w");
  }
  if (len) {
    request->_tempFile.write(data, len);
  }
  if (final) {
    request->_tempFile.close();
    if(filename=="playlist.csv") config.indexPlaylist();
  }
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT: /*netserver.requestOnChange(STARTUP, client->id()); */if (config.store.audioinfo) Serial.printf("[WEBSOCKET] client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str()); break;
    case WS_EVT_DISCONNECT: if (config.store.audioinfo) Serial.printf("[WEBSOCKET] client #%u disconnected\n", client->id()); break;
    case WS_EVT_DATA: netserver.onWsMessage(arg, data, len, client->id()); break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void handleHTTPArgs(AsyncWebServerRequest * request) {
  if (request->method() == HTTP_GET) {
    DBGVB("[%s] client ip=%s request of %s", __func__, request->client()->remoteIP().toString().c_str(), request->url().c_str());
    if (strcmp(request->url().c_str(), PLAYLIST_PATH) == 0 || 
        strcmp(request->url().c_str(), SSIDS_PATH) == 0 || 
        strcmp(request->url().c_str(), INDEX_PATH) == 0 || 
        strcmp(request->url().c_str(), TMP_PATH) == 0 || 
        strcmp(request->url().c_str(), PLAYLIST_SD_PATH) == 0 || 
        strcmp(request->url().c_str(), INDEX_SD_PATH) == 0) {
#ifdef MQTT_ROOT_TOPIC
      if (strcmp(request->url().c_str(), PLAYLIST_PATH) == 0) while (mqttplaylistblock) vTaskDelay(5);
#endif
      const bool playlist = request->url() == PLAYLIST_PATH;
      const bool fromSD = request->url() == PLAYLIST_SD_PATH || (playlist && config.getMode() == PM_SDCARD);
      FS &storage = fromSD ? *config.SDPLFS() : SPIFFS;
      const String path = fromSD ? String(PLAYLIST_SD_PATH) : request->url();
      auto *response = request->beginResponse(storage, path, "application/octet-stream");
      if (response) {
        response->addHeader("Cache-Control", "no-store");
        request->send(response);
      } else {
        request->send(404);
      }
      return;
    }
    Serial.println(request->url());
    if (strcmp(request->url().c_str(), "/") == 0 && request->params() == 0) {
      if(network.status == CONNECTED){
        request->send_P(200, "text/html", index_html);
      }else{
        request->redirect("/settings.html");
      }
      return;
    }
  }
  if (strcmp(request->url().c_str(), "/settings.html") == 0 || strcmp(request->url().c_str(), "/update.html") == 0 || strcmp(request->url().c_str(), "/ir.html") == 0){
    request->send_P(200, "text/html", index_html);
    return;
  }
  if (network.status == CONNECTED) {
    bool commandFound=false;
    if (request->hasArg("start")) { player.sendCommand({PR_PLAY, config.lastStation()}); commandFound=true; }
    if (request->hasArg("stop")) { player.sendCommand({PR_STOP, 0}); commandFound=true; }
    if (request->hasArg("toggle")) { player.toggle(); commandFound=true; }
    if (request->hasArg("prev")) { player.prev(); commandFound=true; }
    if (request->hasArg("next")) { player.next(); commandFound=true; }
    if (request->hasArg("volm")) { player.stepVol(false); commandFound=true; }
    if (request->hasArg("volp")) { player.stepVol(true); commandFound=true; }
    #ifdef USE_SD
    if (request->hasArg("mode")) {
      AsyncWebParameter* p = request->getParam("mode");
      int mm = atoi(p->value().c_str());
      if(mm>2) mm=0;
      if(mm==2)
        config.changeMode();
      else
        config.changeMode(mm);
      commandFound=true;
    }
    #endif
    if (request->hasArg("reset")) { request->redirect("/"); request->send(200); config.reset(); return; }
    if (request->hasArg("trebble") && request->hasArg("middle") && request->hasArg("bass")) {
      AsyncWebParameter* pt = request->getParam("trebble", request->method() == HTTP_POST);
      AsyncWebParameter* pm = request->getParam("middle", request->method() == HTTP_POST);
      AsyncWebParameter* pb = request->getParam("bass", request->method() == HTTP_POST);
      int t = atoi(pt->value().c_str());
      int m = atoi(pm->value().c_str());
      int b = atoi(pb->value().c_str());
      player.setTone(b, m, t);
      config.setTone(b, m, t);
      netserver.requestOnChange(EQUALIZER, 0);
      commandFound=true;
    }
    if (request->hasArg("ballance")) {
      AsyncWebParameter* p = request->getParam("ballance", request->method() == HTTP_POST);
      int b = atoi(p->value().c_str());
      player.setBalance(b);
      config.setBalance(b);
      netserver.requestOnChange(BALANCE, 0);
      commandFound=true;
    }
    if (request->hasArg("playstation") || request->hasArg("play")) {
      AsyncWebParameter* p = request->getParam(request->hasArg("playstation") ? "playstation" : "play", request->method() == HTTP_POST);
      int id = atoi(p->value().c_str());
      if (id < 1) id = 1;
      uint16_t cs = config.playlistLength();
      if (id > cs) id = cs;
      //config.sdResumePos = 0;
      player.sendCommand({PR_PLAY, id});
      commandFound=true;
      DBGVB("[%s] play=%d", __func__, id);
    }
    if (request->hasArg("vol")) {
      AsyncWebParameter* p = request->getParam("vol", request->method() == HTTP_POST);
      int v = atoi(p->value().c_str());
      if (v < 0) v = 0;
      if (v > 254) v = 254;
      config.store.volume = v;
      player.setVol(v);
      commandFound=true;
      DBGVB("[%s] vol=%d", __func__, v);
    }
    if (request->hasArg("dspon")) {
      AsyncWebParameter* p = request->getParam("dspon", request->method() == HTTP_POST);
      int d = atoi(p->value().c_str());
      config.setDspOn(d!=0);
      commandFound=true;
    }
    if (request->hasArg("dim")) {
      AsyncWebParameter* p = request->getParam("dim", request->method() == HTTP_POST);
      int d = atoi(p->value().c_str());
      if (d < 0) d = 0;
      if (d > 100) d = 100;
      config.store.brightness = (uint8_t)d;
      config.setBrightness(true);
      commandFound=true;
    }
    if (request->hasArg("sleep")) {
      AsyncWebParameter* sfor = request->getParam("sleep", request->method() == HTTP_POST);
      int sford = atoi(sfor->value().c_str());
      int safterd = 0;
      if(request->hasArg("after")){
        AsyncWebParameter* safter = request->getParam("after", request->method() == HTTP_POST);
        safterd = atoi(safter->value().c_str());
      }
      if(sford > 0 && safterd >= 0){
        request->send(200);
        config.sleepForAfter(sford, safterd);
        commandFound=true;
      }
    }
    if (request->hasArg("clearspiffs")) {
      if(config.spiffsCleanup()){
        config.saveValue(&config.store.play_mode, static_cast<uint8_t>(PM_WEB));
        request->redirect("/");
        ESP.restart();
      }else{
        request->send(200);
      }
      return;
    }
    if (request->params() > 0) {
      request->send(commandFound?200:404);
      return;
    }
  } else {
    if (request->params() > 0) {
      request->send(404);
      return;
    }
  }
}
