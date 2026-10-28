#ifndef netserver_h
#define netserver_h
#include "Arduino.h"

#include "../AsyncWebServer/ESPAsyncWebServer.h"
#include "AsyncUDP.h"

enum requestType_e : uint8_t  { PLAYLIST=1, STATION=2, STATIONNAME=3, ITEM=4, TITLE=5, VOLUME=6, NRSSI=7, BITRATE=8, MODE=9, EQUALIZER=10, BALANCE=11, PLAYLISTSAVED=12, STARTUP=13, GETINDEX=14, GETACTIVE=15, GETSYSTEM=16, GETSCREEN=17, GETTIMEZONE=18, GETWEATHER=19, GETCONTROLS=20, DSPON=21, SDPOS=22, SDLEN=23, SDSNUFFLE=24, SDINIT=25, GETPLAYERMODE=26, CHANGEMODE=27 };
enum import_e      : uint8_t  { IMDONE=0, IMPL=1, IMWIFI=2 };
const char emptyfs_html[] PROGMEM = R"(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1, minimum-scale=0.25"><meta charset="UTF-8">
<link rel="icon" type="image/png" href="data:image/png;base64,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">
<title>ёRadio - WEB Board Uploader</title><style>html, body { margin: 0; padding: 0; height: 100%; } body{background-color:#000;color:#e3d25f;font-size:20px;display:flex;flex-direction:column;}
hr{margin:20px 0;border:0; border-top:#555 1px solid;} p{text-align:center;margin-bottom:10px;} section{max-width:500px; text-align:center;margin:0 auto 30px auto;padding:20px;flex:1;}
.hidden{display:none;}a { color: var(--accent-color); text-decoration: none; font-weight: bold } a:hover { text-decoration: underline }
#copy { text-align: center; padding: 14px; font-size: 14px; }
input[type=file]{color:#ccc;} input[type=file]::file-selector-button, input[type=submit]{border:2px solid #e3d25f;color:#000;padding:6px 16px;border-radius:25px;background-color:#e3d25f;margin:0 6px;cursor:pointer;}
input[type=submit]{font-size:18px;text-transform:uppercase;padding:8px 26px;margin-top:10px;font-family:Times;} span{color:#ccc} .flex{display:flex;justify-content: space-around;margin-top:10px;}
input[type=text],input[type=password]{width:170px;background:#272727;color:#e3d25f;padding:6px 12px;font-size:20px;border:#2d2d2d 1px solid;margin:4px 0 0 4px;border-radius:4px;outline:none;}
@media screen and (max-width:480px) {section{zoom:0.7;-moz-transform:scale(0.7);}}
</style>
<script type="text/javascript" src="/variables.js"></script>
</head><body>
<section>
<h2>ёRadio - WEB Board Uploader</h2>
<hr />
<span>Select <u>ALL</u> files from <i>yoRadio/data/www/</i><br />and upload them using the form below</span>
<hr />
<form action="/webboard" method="post" enctype="multipart/form-data">
<p><label for="www">www:</label> <input type="file" name="www" id="www" multiple></p>
<hr />
<span>-= OPTIONAL =-<br />You can also upload <i>playlist.csv</i><br />and <i>wifi.csv files</i> from your backup</span>
<p><label for="data">wifi:</label><input type="file" name="data" id="data" multiple></p>
<hr />
<p><input type="submit" name="submit" value="Upload Files"></p>
</form>
<div style="padding:10px 0 0;" id="wupload">
<hr />
<form name="wifiform" method="post" enctype="multipart/form-data">
<span>-= OPTIONAL =-<br />If you can't connect from PC to 192.168.4.1 address<br />setup WiFi connection first</span>
<div class="flex"><div><label for="ssid">ssid:</label><input type="text" id="ssid" name="ssid" value="" maxlength="30" autocomplete="off"></div>
<div><label for="pass">pass:</label><input type="password" id="pass" name="pass" value="" maxlength="40" autocomplete="off"></div>
</div>
<p><input type="submit" name="submit" value="Save Credentials"></p>
</form>
</div>
</section>
<div id="copy">powered by <a target="_blank" href="https://github.com/e2002/yoradio/">ёRadio</a><span id="version"></span></div>
</body>
<script>
document.wifiform.action = `/${formAction}`;
if(playMode=='player') document.getElementById("wupload").classList.add("hidden");
document.getElementById("version").innerHTML=` | v${yoVersion}`;
</script>
</html>
)";
const char index_html[] PROGMEM = R"(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <meta name="theme-color" content="#e3d25f">
  <meta name="apple-mobile-web-app-capable" content="yes">
  <meta name="apple-mobile-web-app-status-bar-style" content="default">
  <link rel="icon" type="image/png" href="data:image/png;base64,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">
  <link rel="stylesheet" href="theme.css" type="text/css" />
  <link rel="stylesheet" href="style.css" type="text/css" />
  <script type="text/javascript" src="variables.js"></script>
  <script type="text/javascript" src="script.js"></script>
  <script type="text/javascript" src="dragpl.js"></script>
  </head>
<body>
<div id="content" class="hidden progmem">
</div><!--content-->
<div id="progress"><span id="loader"></span></div>
</body>
</html>
)";
struct nsRequestParams_t
{
  requestType_e type;
  uint8_t clientId;
};

class NetServer {
  public:
    import_e importRequest;
    bool resumePlay;
    char chunkedPathBuffer[40];
  public:
    NetServer() {};
    bool begin(bool quiet=false);
    void loop();
    void requestOnChange(requestType_e request, uint8_t clientId);
    void setRSSI(int val) { rssi = val; };
    int  getRSSI()        { return rssi; };
    void chunkedHtmlPage(const String& contentType, AsyncWebServerRequest *request, const char * path, bool doproc = true);
    void onWsMessage(void *arg, uint8_t *data, size_t len, uint8_t clientId);
    bool irRecordEnable;
#if IR_PIN!=255
    void irToWs(const char* protocol, uint64_t irvalue);
    void irValsToWs(); 
#endif
		void resetQueue();
  private:
    requestType_e request;
    QueueHandle_t nsQueue;
    int rssi, newConfigMode;
    void getPlaylist(uint8_t clientId);
    bool importPlaylist();
    static size_t chunkedHtmlPageCallback(uint8_t* buffer, size_t maxLen, size_t index);
    static void beginUpload(AsyncWebServerRequest *request);
    static void beginUpdate(AsyncWebServerRequest *request);
    void processQueue();
    int _readPlaylistLine(File &file, char * line, size_t size);
};

extern NetServer netserver;

#endif
