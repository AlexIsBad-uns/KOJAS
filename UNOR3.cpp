#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <string.h>
#include <stdlib.h>

MCUFRIEND_kbv tft;

#define C_BG       0x0000
#define C_HDR      0x18C3
#define C_ROW_A    0x1082
#define C_ROW_B    0x0841
#define C_DIV      0x4208
#define C_WHITE    0xFFFF
#define C_DIM      0x632C
#define C_BUS      0xFB60
#define C_TROL     0x033F
#define C_TRAM     0xF800
#define C_SUNNY    0xFFE0
#define C_ORANGE   0xFD20
#define C_GOLD     0xFEA0  
#define C_NDAY_CLR 0x867F  

const int N_STOPS  = 2;
const int HDR_H    = 14;   
const int ROW_H    = 13;   
const int DIV      = 2;
const int MAX_DEPS = 8;    

int modW, modH;
int col1W;
int maxRows;

char stopNames[N_STOPS][16] = {{0},{0}};

char dispSsid[32] = "KOJAS-BOOTING";
char dispPass[16] = "..........";

struct Stop { int x, y, w, h; };
Stop panels[N_STOPS];

struct Dep { char type[5]; char route[6]; int16_t secs; }; 
Dep  deps[N_STOPS][MAX_DEPS];
int  depCount[N_STOPS] = {};
bool newData[N_STOPS]  = {};
int  rxStop   = -1;
bool receiving = false;

char rxBuf[80]; 
int rxIdx = 0;

char w_temp[6]="--", w_max[6]="--", w_min[6]="--", w_pop[5]="--", w_precip[5]="--", w_wind[5]="--";
char w_rise[6]="--", w_set[6]="--", w_hum[5]="--", w_prech[5]="--", w_desc[12]="--";
int  w_is_day = 1;

char w_weekday[12]="---", w_date_d[4]="--", w_date_m[4]="---";
char w_sday[42]="----";

bool drawWeatherFlag = true;
bool drawDaysFlag    = true;

#define MAX_BDAYS 5
struct BdayEvt {
  char name[16]; 
  char date[12];
  bool today;
};
BdayEvt bdays[MAX_BDAYS];
int     bdayCount = 0;
bool    bdayRxing = false;
bool    drawBdayFlag = true;

#define MAX_NDAYS 3
char ndays[MAX_NDAYS][16]; 
int  ndayCount = 0;
bool ndayRxing = false;
bool drawNdayFlag = true;

#define MAX_CAL_EVTS 5 
struct CalEvt {
  char timeLabel[24]; 
  char title[32];     
  uint16_t color;
};
CalEvt calEvts[MAX_CAL_EVTS];
int    calCount = 0;
bool   calRxing = false;
bool   drawCalFlag = true;

uint16_t parseColor(const char* s) {
  if (s[0]=='0' && (s[1]=='x'||s[1]=='X')) {
    return (uint16_t)strtol(s+2, nullptr, 16);
  }
  return C_WHITE;
}

int  currentMode     = -1;
bool modeChangedFlag = true;

int  cur_h=0, cur_m=0, cur_s=0;
unsigned long lastSecTick = 0;
bool clockValid = false;

uint16_t typeColor(const char* t) {
  if (t[0]=='t' && t[1]=='r' && t[2]=='o') return C_TROL;
  if (t[0]=='t' && t[1]=='r' && t[2]=='a') return C_TRAM;
  return C_BUS;
}

void ctext(const char* s, int x, int y, int w, uint16_t c) {
  tft.setTextSize(1); tft.setTextColor(c);
  tft.setCursor(x + max(0, (w-(int)strlen(s)*6)/2), y);
  tft.print(s);
}

void drawGrid() {
  tft.fillRect(modW-1, 0, DIV, tft.height(), C_DIV);
  tft.fillRect(0, modH-1, tft.width(), DIV, C_DIV);
  tft.fillRect((modW/2) - 1, modH, DIV, modH, C_DIV);
}

void drawTransportHeader(int i) {
  Stop& p = panels[i];
  tft.fillRect(p.x, p.y, p.w, p.h, C_BG);
  tft.fillRect(p.x, p.y, p.w, HDR_H, C_HDR);
  ctext(stopNames[i][0]?stopNames[i]:"--", p.x, p.y+3, p.w, C_WHITE);
}

void drawTransportRow(int si, int ri, const Dep& d) {
  Stop& p = panels[si];
  int y = p.y + HDR_H + ri*ROW_H; 
  if (y+ROW_H > p.y+p.h) return;
  tft.fillRect(p.x,        y, col1W,       ROW_H, typeColor(d.type));
  tft.fillRect(p.x+col1W,  y, p.w-col1W, ROW_H, ri%2?C_ROW_B:C_ROW_A);
  ctext(d.route, p.x, y+3, col1W, C_BG);
  
  bool isNow = (d.secs < 60);
  char buf[8];
  if (isNow) {
    strcpy(buf, "now");
  } else {
    itoa(d.secs/60, buf, 10);
    strcat(buf, " min");
  }
  ctext(buf, p.x+col1W, y+3, p.w-col1W, isNow?C_DIM:C_WHITE);
}

void renderTable(int i) {
  drawTransportHeader(i);
  for (int r=0; r<maxRows; r++) {
    Stop& p = panels[i];
    int y = p.y + HDR_H + r*ROW_H;
    if (y+ROW_H > p.y+p.h) break;
    if (r < depCount[i]) drawTransportRow(i, r, deps[i][r]);
    else tft.fillRect(p.x, y, p.w, ROW_H, r%2?C_ROW_B:C_ROW_A);
  }
}

uint16_t getTempColor(int t) {
  int r, g, b;
  if      (t <= -10){ r=0; g=0; b=255; }
  else if (t <= 0)  { g = r = ((t + 10) * 255) / 10; b = 255; }
  else if (t <= 20) { r = 255; g = 255; b = 255 - ((t * 255) / 20); }
  else if (t <= 35) { r = 255; g = 255 - (((t - 20) * 255) / 15); b = 0; }
  else              { r = 255; g = 0; b = 0; }
  return tft.color565(constrain(r,0,255), constrain(g,0,255), constrain(b,0,255));
}

void drawTimeOnly() {
  if (!clockValid || currentMode==1) return;
  int wx=modW;
  tft.fillRect(wx+1, 1, modW-2, 21, C_BG); 
  tft.setTextSize(2); tft.setTextColor(C_WHITE);
  
  int tw=8*12;
  tft.setCursor(wx+(modW-tw)/2, 5); 
  if(cur_h < 10) tft.print('0'); tft.print(cur_h); tft.print(':');
  if(cur_m < 10) tft.print('0'); tft.print(cur_m); tft.print(':');
  if(cur_s < 10) tft.print('0'); tft.print(cur_s);
}

void drawWeatherIcon(int cx, int cy, const char* desc, int isDay) {
  uint16_t c_cloud=C_WHITE, c_sun=0xFE40, c_rain=0x051F, c_storm=0x8410;
  if (desc[0]=='S' && desc[1]=='u') { 
    if (isDay) {
      tft.fillCircle(cx,cy,7,c_sun);
      tft.fillRect(cx-1,cy-14,3,4,c_sun); tft.fillRect(cx-1,cy+11,3,4,c_sun);
      tft.fillRect(cx-14,cy-1,4,3,c_sun); tft.fillRect(cx+11,cy-1,4,3,c_sun);
      tft.drawLine(cx-9,cy-9,cx-11,cy-11,c_sun); tft.drawLine(cx-8,cy-9,cx-10,cy-11,c_sun);
      tft.drawLine(cx+9,cy+9,cx+11,cy+11,c_sun); tft.drawLine(cx+8,cy+9,cx+10,cy+11,c_sun);
      tft.drawLine(cx-9,cy+9,cx-11,cy+11,c_sun); tft.drawLine(cx-8,cy+9,cx-10,cy+11,c_sun);
      tft.drawLine(cx+9,cy-9,cx+11,cy-11,c_sun); tft.drawLine(cx+8,cy-9,cx+10,cy-11,c_sun);
    } else { tft.fillCircle(cx,cy,8,c_sun); tft.fillCircle(cx-3,cy-3,8,C_BG); }
  } else if (desc[0]=='C') { 
    if(!isDay){ tft.fillCircle(cx-5,cy-5,6,c_sun); tft.fillCircle(cx-8,cy-8,6,C_BG); }
    tft.fillCircle(cx-7,cy+1,5,c_cloud); tft.fillCircle(cx+7,cy+2,4,c_cloud);
    tft.fillCircle(cx,cy-3,7,c_cloud); tft.fillRect(cx-7,cy+1,15,6,c_cloud);
  } else if (desc[0]=='R' || desc[0]=='D') { 
    int cy_c=cy-3;
    tft.fillCircle(cx-7,cy_c+1,5,c_cloud); tft.fillCircle(cx+7,cy_c+2,4,c_cloud);
    tft.fillCircle(cx,cy_c-3,7,c_cloud); tft.fillRect(cx-7,cy_c+1,15,6,c_cloud);
    tft.drawLine(cx-5,cy+6,cx-7,cy+12,c_rain); tft.drawLine(cx-4,cy+6,cx-6,cy+12,c_rain);
    tft.drawLine(cx+3,cy+6,cx+1,cy+12,c_rain); tft.drawLine(cx+4,cy+6,cx+2,cy+12,c_rain);
  } else if (desc[1]=='n') { 
    int cy_c=cy-3;
    tft.fillCircle(cx-7,cy_c+1,5,c_cloud); tft.fillCircle(cx+7,cy_c+2,4,c_cloud);
    tft.fillCircle(cx,cy_c-3,7,c_cloud); tft.fillRect(cx-7,cy_c+1,15,6,c_cloud);
    tft.fillCircle(cx-4,cy+8,1,C_WHITE); tft.fillCircle(cx+4,cy+9,1,C_WHITE); tft.fillCircle(cx,cy+13,1,C_WHITE);
  } else if (desc[1]=='t') { 
    int cy_c=cy-4;
    tft.fillCircle(cx-7,cy_c+1,5,c_storm); tft.fillCircle(cx+7,cy_c+2,4,c_storm);
    tft.fillCircle(cx,cy_c-3,7,c_storm); tft.fillRect(cx-7,cy_c+1,15,6,c_storm);
    tft.fillTriangle(cx+2,cy+3,cx-4,cy+9,cx+2,cy+9,c_sun);
    tft.fillTriangle(cx-2,cy+8,cx+3,cy+8,cx-2,cy+15,c_sun);
  } else if (desc[0]=='F') { 
    tft.fillRect(cx-10,cy-4,20,3,c_cloud); tft.fillRect(cx-6,cy+1,16,3,c_cloud); tft.fillRect(cx-12,cy+6,24,3,c_cloud);
  } else {
    tft.drawCircle(cx,cy,10,C_WHITE); tft.setCursor(cx-2,cy-3); tft.print('?');
  }
}

#define BDAY_HDR_H   12
#define BDAY_ROW_H   12
#define NDAY_HDR_H   10
#define NDAY_ROW_H   12

void renderTopLeft() {
  int wx=0, wy=0, ww=modW, wh=modH;
  tft.fillRect(wx, wy, ww, wh, C_BG);

  int dateColW = ww * 35 / 100;
  int rightX   = wx + dateColW;
  int rightW   = ww - dateColW;

  tft.fillRect(rightX, wy, DIV, wh, C_DIV);
  rightX += DIV; rightW -= DIV;

  tft.setTextSize(4); tft.setTextColor(C_WHITE);
  int dLen = strlen(w_date_d)*24;
  tft.setCursor(wx + max(0, (dateColW - dLen)/2), wy+6);
  tft.print(w_date_d);

  tft.setTextSize(1);
  char mwBuf[16];
  snprintf(mwBuf, sizeof(mwBuf), "%s %s", w_date_m, w_weekday);
  int mwLen = strlen(mwBuf) * 6;
  int startX = wx + max(0, (dateColW - mwLen)/2);
  
  tft.setTextColor(C_DIM);
  tft.setCursor(startX, wy + 40);
  tft.print(w_date_m);
  
  tft.setTextColor(C_ORANGE);
  tft.setCursor(startX + strlen(w_date_m)*6 + 6, wy + 40);
  tft.print(w_weekday);

  int sLen = strlen(w_sday);
  if(sLen > 0) {
    int lineCount = 0;
    int start = 0;
    while(start < sLen && lineCount < 6) { 
      int l = min(9, sLen - start);
      int breakAt = l;
      if(start + l < sLen) {
        for(int i=l; i>0; i--) {
          if(w_sday[start+i] == ' ' || w_sday[start+i] == '-') {
            breakAt = i + (w_sday[start+i] == '-' ? 1 : 0);
            break;
          }
        }
      }
      lineCount++;
      start += breakAt;
      if(w_sday[start] == ' ') start++;
    }

    int topY = wy + 52;
    int bottomY = wy + wh - 4;
    int availH = bottomY - topY;
    int blockH = lineCount * 10;
    int sy = topY + max(0, (availH - blockH) / 2);

    start = 0;
    char line[12];
    for (int k=0; k<lineCount; k++) {
      int l = min(9, sLen - start);
      int breakAt = l;
      if(start + l < sLen) {
        for(int i=l; i>0; i--) {
          if(w_sday[start+i] == ' ' || w_sday[start+i] == '-') {
            breakAt = i + (w_sday[start+i] == '-' ? 1 : 0);
            break;
          }
        }
      }
      strncpy(line, w_sday + start, breakAt);
      line[breakAt] = '\0';
      if(breakAt > 0 && line[breakAt-1] == ' ') line[breakAt-1] = '\0';
      
      ctext(line, wx, sy, dateColW, C_WHITE);
      sy += 10; 
      start += breakAt;
      if(w_sday[start] == ' ') start++;
    }
  }

  int cy = wy;

  tft.fillRect(rightX, cy, rightW, BDAY_HDR_H, C_HDR);
  tft.setTextSize(1); tft.setTextColor(C_DIM);
  tft.setCursor(rightX+2, cy+3);
  tft.print(F("BIRTHDAYS"));
  cy += BDAY_HDR_H;

  if (bdayCount == 0) {
    tft.fillRect(rightX, cy, rightW, BDAY_ROW_H * MAX_BDAYS, C_BG);
    tft.setTextColor(C_DIM); tft.setCursor(rightX+4, cy+3); tft.print(F("--"));
    cy += BDAY_ROW_H * MAX_BDAYS;
  } else {
    for (int i=0; i<MAX_BDAYS; i++) {
      uint16_t rowBg = (i%2) ? C_ROW_B : C_ROW_A;
      tft.fillRect(rightX, cy, rightW, BDAY_ROW_H, rowBg);

      if (i < bdayCount) {
        tft.setTextSize(1); tft.setTextColor(C_DIM);
        tft.setCursor(rightX+2, cy+3);
        tft.print(bdays[i].date);

        int nameX = rightX + 44;
        tft.setTextColor(bdays[i].today ? C_GOLD : C_WHITE);
        tft.setCursor(nameX, cy+3);
        tft.print(bdays[i].name);
        
        if(bdays[i].today) {
           tft.setCursor(nameX+1, cy+3); 
           tft.print(bdays[i].name);
        }
      }
      cy += BDAY_ROW_H;
    }
  }

  tft.fillRect(rightX, cy, rightW, 1, C_DIV);
  cy += 1;

  tft.fillRect(rightX, cy, rightW, NDAY_HDR_H, C_HDR);
  tft.setTextSize(1); tft.setTextColor(C_DIM);
  tft.setCursor(rightX+2, cy+2);
  tft.print(F("NAMEDAYS"));
  cy += NDAY_HDR_H;

  if (ndayCount == 0) {
    tft.fillRect(rightX, cy, rightW, NDAY_ROW_H * MAX_NDAYS, C_BG);
    tft.setTextColor(C_DIM); tft.setCursor(rightX+4, cy+2); tft.print(F("--"));
  } else {
    for (int i=0; i<MAX_NDAYS; i++) {
      tft.fillRect(rightX, cy, rightW, NDAY_ROW_H, C_BG);
      if (i < ndayCount) {
        tft.fillRect(rightX, cy+1, 2, NDAY_ROW_H-2, C_NDAY_CLR);
        tft.setTextSize(1); tft.setTextColor(C_NDAY_CLR);
        tft.setCursor(rightX+5, cy+2);
        tft.print(ndays[i]);
      }
      cy += NDAY_ROW_H;
    }
  }
}

void renderWeather() {
  int wx=modW, wy=0, ww=modW, wh=modH;
  tft.fillRect(wx, wy, ww, wh, C_BG);

  drawTimeOnly();
  drawWeatherIcon(wx+28, wy+41, w_desc, w_is_day); 

  tft.setTextSize(1); tft.setTextColor(C_WHITE);
  int dw=strlen(w_desc)*6;
  tft.setCursor(wx+28-dw/2, wy+57); tft.print(w_desc); 

  tft.setTextSize(3); tft.setTextColor(getTempColor(atoi(w_temp)));
  tft.setCursor(wx+60, wy+28); tft.print(w_temp); tft.print(F("C"));

  tft.setTextSize(1); tft.setCursor(wx+60, wy+58);
  tft.setTextColor(getTempColor(atoi(w_max))); tft.print(w_min); tft.print(F("C"));
  tft.setTextColor(C_DIM); tft.print(F(" to "));
  tft.setTextColor(getTempColor(atoi(w_min))); tft.print(w_max); tft.print(F("C"));

  int yo=77;
  int wDataX = wx + 8;
  
  tft.setCursor(wDataX, wy+yo);
  tft.setTextColor(C_DIM);   tft.print(F("Wind:"));
  tft.setTextColor(C_WHITE); tft.print(w_wind); tft.print(F("km/h  "));
  tft.setTextColor(C_DIM);   tft.print(F("Humid:"));
  tft.setTextColor(C_WHITE); tft.print(w_hum); tft.print(F("%"));

  tft.setCursor(wDataX, wy+yo+11);
  tft.setTextColor(C_DIM);   tft.print(F("Percip:"));
  tft.setTextColor(C_WHITE); tft.print(w_pop); tft.print(F("%  "));
  tft.print(w_precip); tft.print(F("mm"));

  tft.setCursor(wDataX, wy+yo+22);
  tft.setTextColor(C_DIM);   tft.print(F("Sun:"));
  tft.setTextColor(C_WHITE); tft.print(w_rise); tft.print(F(" - ")); tft.print(w_set);
}

#define CAL_HDR_H   14
#define CAL_ROW_H   13
#define CAL_BAR_W    4   

void renderCalendar() {
  int wx=modW, wy=modH, ww=modW, wh=modH;
  tft.fillRect(wx, wy, ww, wh, C_BG);

  tft.fillRect(wx, wy, ww, CAL_HDR_H, C_HDR);
  for (int gx=0; gx<3; gx++)
    for (int gy=0; gy<2; gy++)
      tft.fillRect(wx+3+gx*4, wy+3+gy*4, 2, 2, (gy==0 && gx==0) ? C_DIM : C_WHITE);
      
  tft.setTextSize(1); tft.setTextColor(C_DIM);
  tft.setCursor(wx+18, wy+4);
  tft.print(F("EVENTS"));
  int cy = wy + CAL_HDR_H;

  if (calCount == 0) {
    tft.setTextSize(1); tft.setTextColor(C_DIM);
    tft.setCursor(wx+4, cy+10);
    tft.print(F("No events"));
    return;
  }

  for (int i=0; i<calCount; i++) {
    bool hasTitle = (calEvts[i].title[0] != '\0');
    int eventH = hasTitle ? (CAL_ROW_H * 2) : CAL_ROW_H; 

    if (cy + eventH > wy + wh) break; 

    uint16_t rowBg = (i%2) ? 0x0821 : 0x1042;
    tft.fillRect(wx, cy, ww, eventH, rowBg);
    tft.fillRect(wx, cy, CAL_BAR_W, eventH, calEvts[i].color);

    tft.setTextSize(1); 
    if (hasTitle) {
      tft.setTextColor(C_DIM);
      tft.setCursor(wx + CAL_BAR_W + 4, cy + 3);
      tft.print(calEvts[i].timeLabel);
      
      tft.setTextColor(C_WHITE);
      tft.setCursor(wx + CAL_BAR_W + 4, cy + CAL_ROW_H + 3);
      tft.print(calEvts[i].title);
    } else {
      tft.setTextColor(C_DIM); 
      tft.setCursor(wx + CAL_BAR_W + 4, cy + 3);
      tft.print(calEvts[i].timeLabel);
    }
    cy += eventH;
  }
  if (cy < wy + wh) tft.fillRect(wx, cy, ww, wy + wh - cy, C_BG); 
}

void drawConfigScreen() {
  tft.fillScreen(C_BG);
  tft.setTextSize(6); tft.setTextColor(C_WHITE);
  tft.setCursor((tft.width()-180)/2, tft.height()/2-40); tft.print(F("KOJAS"));
  
  tft.setTextSize(2); tft.setTextColor(C_DIM);
  
  char buf[64];
  sprintf(buf, "WiFi: %s", dispSsid);
  tft.setCursor((tft.width()-strlen(buf)*12)/2, tft.height()/2+10); 
  tft.print(buf);
  
  sprintf(buf, "Pass: %s", dispPass);
  tft.setCursor((tft.width()-strlen(buf)*12)/2, tft.height()/2+30); 
  tft.print(buf);
  
  tft.setTextSize(1); tft.setTextColor(C_ORANGE);
  tft.setCursor((tft.width()-180)/2, tft.height()/2+60); tft.print(F("Connect, then open 192.168.4.1"));
}

void drawWorkScreen() {
  tft.fillScreen(C_BG);
  renderTopLeft();
  renderWeather();
  for (int i=0;i<N_STOPS;i++) renderTable(i);
  renderCalendar();
  drawGrid();
}

void copyToComma(char* dest, const char** src, int maxLen) {
  int i = 0;
  while (**src && **src != ',' && i < maxLen - 1) {
    dest[i++] = *(*src)++;
  }
  dest[i] = '\0';
  if (**src == ',') (*src)++;
}

void processLine(char* l) {
  if (strncmp(l, "CMD:MODE,", 9) == 0) {
    currentMode = (strcmp(l+9, "CONFIG") == 0) ? 1 : 0;
    modeChangedFlag = true; return;
  }
  
  if (strncmp(l, "CMD:ROT,", 8) == 0) {
    int rot = atoi(l+8);
    tft.setRotation(rot ? 3 : 1);
    tft.fillScreen(C_BG);
    if(currentMode==1) drawConfigScreen();
    else if(currentMode==0) drawWorkScreen();
    return;
  }

  if (strncmp(l, "CMD:AP_INFO,", 12) == 0) {
    const char* p = l + 12;
    copyToComma(dispSsid, &p, sizeof(dispSsid));
    copyToComma(dispPass, &p, sizeof(dispPass));
    if (currentMode == 1) drawConfigScreen();
    return;
  }

  if (strncmp(l, "W1:", 3) == 0) {
    const char* p = l + 3;
    copyToComma(w_temp, &p, sizeof(w_temp));
    copyToComma(w_max, &p, sizeof(w_max));
    copyToComma(w_min, &p, sizeof(w_min));
    copyToComma(w_pop, &p, sizeof(w_pop));
    copyToComma(w_precip, &p, sizeof(w_precip));
    strncpy(w_wind, p, sizeof(w_wind)-1); w_wind[sizeof(w_wind)-1] = '\0';
    return;
  }

  if (strncmp(l, "W2:", 3) == 0) {
    const char* p = l + 3;
    copyToComma(w_rise, &p, sizeof(w_rise));
    copyToComma(w_set, &p, sizeof(w_set));
    copyToComma(w_hum, &p, sizeof(w_hum));
    copyToComma(w_prech, &p, sizeof(w_prech));
    char isDayStr[4];
    copyToComma(isDayStr, &p, sizeof(isDayStr));
    w_is_day = atoi(isDayStr);
    strncpy(w_desc, p, sizeof(w_desc)-1); w_desc[sizeof(w_desc)-1] = '\0';
    drawWeatherFlag = true; return;
  }

  if (strncmp(l, "WDAYS:", 6) == 0) {
    const char* p = l + 6;
    copyToComma(w_weekday, &p, sizeof(w_weekday));
    copyToComma(w_date_d, &p, sizeof(w_date_d));
    strncpy(w_date_m, p, sizeof(w_date_m)-1); w_date_m[sizeof(w_date_m)-1] = '\0';
    drawDaysFlag = true; return;
  }

  if (strncmp(l, "WTIME:", 6) == 0) {
    if (strlen(l) < 14) return;
    char tmp[3]; tmp[2] = '\0';
    tmp[0]=l[6]; tmp[1]=l[7]; cur_h = atoi(tmp);
    tmp[0]=l[9]; tmp[1]=l[10]; cur_m = atoi(tmp);
    tmp[0]=l[12]; tmp[1]=l[13]; cur_s = atoi(tmp);
    clockValid = true; lastSecTick = millis(); return;
  }

  if (strncmp(l, "NAME:", 5) == 0) {
    const char* p = l + 5;
    char idxStr[4];
    copyToComma(idxStr, &p, sizeof(idxStr));
    int idx = atoi(idxStr);
    if (idx >= 0 && idx < N_STOPS) {
      strncpy(stopNames[idx], p, 15);
      stopNames[idx][15] = '\0';
    }
    return;
  }

  if (strncmp(l, "SDAY:", 5) == 0) {
    strncpy(w_sday, l + 5, sizeof(w_sday)-1);
    w_sday[sizeof(w_sday)-1] = '\0';
    drawDaysFlag = true;
    return;
  }

  if (strncmp(l, "CMD:BEGIN,", 10) == 0) {
    rxStop = atoi(l+10);
    if(rxStop>=0 && rxStop<N_STOPS){ receiving=true; depCount[rxStop]=0; }
    return;
  }
  
  if (strcmp(l, "CMD:END") == 0) {
    if(rxStop>=0) newData[rxStop]=true;
    receiving=false; rxStop=-1; return;
  }
  
  if (receiving && rxStop>=0 && depCount[rxStop]<maxRows && depCount[rxStop]<MAX_DEPS) {
    const char* p = l;
    int idx = depCount[rxStop];
    copyToComma(deps[rxStop][idx].type, &p, sizeof(deps[rxStop][idx].type));
    copyToComma(deps[rxStop][idx].route, &p, sizeof(deps[rxStop][idx].route));
    deps[rxStop][idx].secs = atoi(p);
    depCount[rxStop]++;
    return;
  }

  if (strcmp(l, "BDAY:BEGIN") == 0) { bdayRxing=true; bdayCount=0; return; }
  if (strcmp(l, "BDAY:END") == 0)   { bdayRxing=false; drawBdayFlag=true; return; }
  if (bdayRxing && strncmp(l, "BDAY:EVT,", 9) == 0 && bdayCount<MAX_BDAYS) {
    const char* p = l + 9;
    int idx = bdayCount;
    copyToComma(bdays[idx].name, &p, sizeof(bdays[idx].name));
    copyToComma(bdays[idx].date, &p, sizeof(bdays[idx].date));
    bdays[idx].today = (atoi(p) == 1);
    bdayCount++;
    return;
  }

  if (strcmp(l, "NDAY:BEGIN") == 0) { ndayRxing=true; ndayCount=0; return; }
  if (strcmp(l, "NDAY:END") == 0)   { ndayRxing=false; drawNdayFlag=true; return; }
  if (ndayRxing && strncmp(l, "NDAY:", 5) == 0) {
    const char* p = l + 5;
    char idxStr[4];
    copyToComma(idxStr, &p, sizeof(idxStr));
    int idx = atoi(idxStr);
    if (idx>=0 && idx<MAX_NDAYS) {
      strncpy(ndays[idx], p, 15); ndays[idx][15] = '\0';
      ndayCount = max(ndayCount, idx+1);
    }
    return;
  }

  if (strcmp(l, "CAL:BEGIN") == 0) { calRxing=true; calCount=0; return; }
  if (strcmp(l, "CAL:END") == 0)   { calRxing=false; drawCalFlag=true; return; }
  if (calRxing && strncmp(l, "CAL:EVT,", 8) == 0 && calCount<MAX_CAL_EVTS) {
    const char* p = l + 8;
    int idx = calCount;
    copyToComma(calEvts[idx].timeLabel, &p, sizeof(calEvts[idx].timeLabel));
    copyToComma(calEvts[idx].title, &p, sizeof(calEvts[idx].title));
    calEvts[idx].color = parseColor(p);
    calCount++;
    return;
  }
}

void setup() {
  Serial.begin(115200);
  tft.begin(0x9341); tft.setRotation(1);

  modW = tft.width()  / 2;
  modH = tft.height() / 2;

  int panelW = modW / 2;
  col1W = panelW / 3;

  int availH = modH - HDR_H; 
  maxRows = min(availH / ROW_H, MAX_DEPS);

  panels[0] = { 0,      modH, panelW, modH };
  panels[1] = { panelW, modH, panelW, modH };
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      rxBuf[rxIdx] = '\0';
      if (rxIdx > 0) processLine(rxBuf);
      rxIdx = 0;
    } else if (c != '\r' && rxIdx < sizeof(rxBuf) - 1) {
      rxBuf[rxIdx++] = c;
    }
  }

  if (modeChangedFlag) {
    modeChangedFlag=false;
    if(currentMode==1) drawConfigScreen();
    else               drawWorkScreen();
  }

  if (currentMode==0) {
    if (clockValid && millis()-lastSecTick>=1000) {
      lastSecTick+=1000; cur_s++;
      if(cur_s>=60){cur_s=0;cur_m++;} if(cur_m>=60){cur_m=0;cur_h++;} if(cur_h>=24)cur_h=0;
      drawTimeOnly();
    }

    for(int i=0;i<N_STOPS;i++){
      if(newData[i]){ newData[i]=false; renderTable(i); drawGrid(); }
    }

    if(drawDaysFlag)   { drawDaysFlag=false;   renderTopLeft(); drawGrid(); }
    if(drawWeatherFlag){ drawWeatherFlag=false; renderWeather(); drawGrid(); }

    if(drawBdayFlag || drawNdayFlag) {
      drawBdayFlag=false; drawNdayFlag=false;
      renderTopLeft(); drawGrid();
    }

    if(drawCalFlag){ drawCalFlag=false; renderCalendar(); drawGrid(); }
  }
}