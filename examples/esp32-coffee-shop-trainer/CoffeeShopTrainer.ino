/*
  CoffeeShopTrainer.ino
  ESP32 Wi-Fi training node for lab-only packet analysis education.

  What it does:
  - Starts a controlled SoftAP named FuLLC-CoffeeLab
  - Serves a small training page at http://192.168.4.1/
  - Provides /scan JSON for nearby 2.4 GHz Wi-Fi networks
  - Provides /status JSON for AP status and connected station count
  - Provides /events JSON for simple lab events
  - Uses DNS catch-all so many clients open the training page automatically

  What it intentionally does NOT do:
  - Does not impersonate a real business SSID
  - Does not collect usernames, passwords, cookies, or tokens
  - Does not deauthenticate clients
  - Does not bypass encryption or inspect private traffic

  Board target:
  - Arduino IDE board: ESP32 Dev Module
  - Partition scheme: Default 4MB with spiffs is fine
  - Upload speed: start with 115200 if flashing is unreliable
*/

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

#define LAB_SSID       "FuLLC-CoffeeLab"
#define LAB_PASSWORD   "training123"   // 8+ chars required. Change for your lab.
#define AP_CHANNEL     6
#define MAX_CLIENTS    4

IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

WebServer server(80);
DNSServer dnsServer;

static const byte DNS_PORT = 53;

struct EventRecord {
  unsigned long t;
  String event;
};

const int EVENT_LOG_SIZE = 20;
EventRecord events[EVENT_LOG_SIZE];
int eventIndex = 0;

unsigned long lastClientPoll = 0;
int lastStationCount = -1;

void addEvent(const String &event) {
  events[eventIndex].t = millis();
  events[eventIndex].event = event;
  eventIndex = (eventIndex + 1) % EVENT_LOG_SIZE;
  Serial.println("[EVENT] " + event);
}

String htmlPage() {
  String page;
  page += "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>";
  page += "<title>Fu-LLC CoffeeLab WiFi Trainer</title>";
  page += "<style>body{font-family:system-ui;background:#0b1020;color:#e8eefc;margin:0;padding:24px;}";
  page += ".card{max-width:860px;margin:0 auto 16px auto;background:#121a33;border:1px solid #26345f;border-radius:16px;padding:18px;}";
  page += "h1{margin-top:0}.muted{color:#9fb0d0}.ok{color:#9df29d}.warn{color:#ffd37a}";
  page += "code,pre{background:#07101f;border-radius:8px;padding:8px;display:block;overflow:auto}";
  page += "button{background:#467cff;color:white;border:0;border-radius:10px;padding:10px 14px;font-weight:700}";
  page += "table{width:100%;border-collapse:collapse}td,th{border-bottom:1px solid #26345f;padding:8px;text-align:left}</style>";
  page += "</head><body><div class='card'><h1>Fu-LLC CoffeeLab WiFi Trainer</h1>";
  page += "<p class='muted'>Lab-only ESP32 wireless training node. Use this for controlled packet-analysis lessons, RF scanning, client-join observation, and dashboard demos.</p>";
  page += "<p><span class='ok'>Safe mode:</span> no credential capture, no deauth, no private traffic interception.</p>";
  page += "<button onclick='refreshAll()'>Refresh telemetry</button></div>";
  page += "<div class='card'><h2>Status</h2><pre id='status'>loading...</pre></div>";
  page += "<div class='card'><h2>Nearby 2.4 GHz Networks</h2><div id='scan'>loading...</div></div>";
  page += "<div class='card'><h2>Lab Events</h2><pre id='events'>loading...</pre></div>";
  page += "<script>async function j(u){return await (await fetch(u)).json()}";
  page += "async function refreshAll(){let s=await j('/status');document.getElementById('status').textContent=JSON.stringify(s,null,2);";
  page += "let ev=await j('/events');document.getElementById('events').textContent=JSON.stringify(ev,null,2);";
  page += "let sc=await j('/scan');let rows='<table><tr><th>SSID</th><th>BSSID</th><th>CH</th><th>RSSI</th><th>Auth</th></tr>';";
  page += "for(const n of sc.networks){rows+=`<tr><td>${n.ssid||'<hidden>'}</td><td>${n.bssid}</td><td>${n.channel}</td><td>${n.rssi}</td><td>${n.auth}</td></tr>`}";
  page += "document.getElementById('scan').innerHTML=rows+'</table>'; } refreshAll(); setInterval(refreshAll,10000);</script>";
  page += "</body></html>";
  return page;
}

String authModeName(wifi_auth_mode_t auth) {
  switch (auth) {
    case WIFI_AUTH_OPEN: return "OPEN";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA_PSK";
    case WIFI_AUTH_WPA2_PSK: return "WPA2_PSK";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA_WPA2_PSK";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2_ENTERPRISE";
    case WIFI_AUTH_WPA3_PSK: return "WPA3_PSK";
    case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2_WPA3_PSK";
    default: return "UNKNOWN";
  }
}

String jsonEscape(const String &s) {
  String out;
  out.reserve(s.length() + 8);
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '"') out += "\\\"";
    else if (c == '\\') out += "\\\\";
    else if (c == '\n') out += "\\n";
    else if (c == '\r') out += "\\r";
    else out += c;
  }
  return out;
}

void handleRoot() {
  server.send(200, "text/html", htmlPage());
}

void handleStatus() {
  String json = "{";
  json += "\"ssid\":\"" + String(LAB_SSID) + "\",";
  json += "\"ip\":\"" + WiFi.softAPIP().toString() + "\",";
  json += "\"channel\":" + String(AP_CHANNEL) + ",";
  json += "\"stations\":" + String(WiFi.softAPgetStationNum()) + ",";
  json += "\"uptime_ms\":" + String(millis()) + ",";
  json += "\"heap_free\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"chip_model\":\"" + String(ESP.getChipModel()) + "\",";
  json += "\"chip_revision\":" + String(ESP.getChipRevision()) + ",";
  json += "\"flash_mb\":" + String(ESP.getFlashChipSize() / (1024 * 1024));
  json += "}";
  server.send(200, "application/json", json);
}

void handleEvents() {
  String json = "{\"events\":[";
  bool first = true;
  for (int i = 0; i < EVENT_LOG_SIZE; i++) {
    int idx = (eventIndex + i) % EVENT_LOG_SIZE;
    if (events[idx].event.length() == 0) continue;
    if (!first) json += ",";
    json += "{\"t_ms\":" + String(events[idx].t) + ",\"event\":\"" + jsonEscape(events[idx].event) + "\"}";
    first = false;
  }
  json += "]}";
  server.send(200, "application/json", json);
}

void handleScan() {
  addEvent("Wi-Fi scan requested from " + server.client().remoteIP().toString());

  // Temporarily scan nearby networks. This can briefly affect AP responsiveness.
  int n = WiFi.scanNetworks(false, true);
  String json = "{\"count\":" + String(n) + ",\"networks\":[";
  for (int i = 0; i < n; i++) {
    if (i > 0) json += ",";
    json += "{";
    json += "\"ssid\":\"" + jsonEscape(WiFi.SSID(i)) + "\",";
    json += "\"bssid\":\"" + WiFi.BSSIDstr(i) + "\",";
    json += "\"channel\":" + String(WiFi.channel(i)) + ",";
    json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
    json += "\"auth\":\"" + authModeName(WiFi.encryptionType(i)) + "\"";
    json += "}";
  }
  json += "]}";
  WiFi.scanDelete();
  server.send(200, "application/json", json);
}

void handleNotFound() {
  // Captive-portal style redirect for lab convenience.
  server.sendHeader("Location", String("http://") + apIP.toString() + "/", true);
  server.send(302, "text/plain", "Redirecting to CoffeeLab trainer");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("Booting Fu-LLC CoffeeLab WiFi Trainer");

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, netMsk);

  bool apStarted = WiFi.softAP(LAB_SSID, LAB_PASSWORD, AP_CHANNEL, false, MAX_CLIENTS);
  if (!apStarted) {
    Serial.println("SoftAP failed to start");
    while (true) delay(1000);
  }

  addEvent("SoftAP started: " + String(LAB_SSID) + " at " + WiFi.softAPIP().toString());

  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/events", handleEvents);
  server.on("/scan", handleScan);

  // Common captive portal checks.
  server.on("/generate_204", handleRoot);
  server.on("/gen_204", handleRoot);
  server.on("/hotspot-detect.html", handleRoot);
  server.on("/library/test/success.html", handleRoot);
  server.on("/ncsi.txt", []() { server.send(200, "text/plain", "Microsoft NCSI"); });

  server.onNotFound(handleNotFound);
  server.begin();
  addEvent("HTTP server started");

  Serial.println("Connect to SSID: " + String(LAB_SSID));
  Serial.println("Password: " + String(LAB_PASSWORD));
  Serial.println("Open: http://192.168.4.1/");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();

  if (millis() - lastClientPoll > 3000) {
    lastClientPoll = millis();
    int stations = WiFi.softAPgetStationNum();
    if (stations != lastStationCount) {
      lastStationCount = stations;
      addEvent("Station count changed: " + String(stations));
    }
  }
}
