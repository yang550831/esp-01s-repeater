#include <ESP8266WiFi.h>

const char* ssid = "D_CCM";
const char* password = "0925760693";

WiFiServer server(80);


//////////////////74HC595(移位暫存器)///////////////////////////////////
const byte edataPin = D2;    //黃 74HC595 序列腳接「數位 2」
const byte elatchPin = D3;   //綠 74HC595 暫存器時脈腳接「數位 3」
const byte eclockPin = D4;   //藍 74HC595 序列時脈腳接「數位 4」
/////////////////////////////////////////////////////////////////////////////
//////////////////74HC165(移位暫存器)///////////////////////////////////
//定義觸發移位寄存器讀取和鎖存的脈衝寬度。
#define PULSE_WIDTH_USEC   5
//底下引腳設定可誰意設定,但最號必開有用的引腳
int ploadPin        = D5;  // 連接到165N的並行負載引腳
int clockEnablePin  = D6;  // 連接到165N的時鐘致能引腳
int dataPin         = D7; //  連接到165N的Q7引腳
int clockPin        = D8; // 連接到165N的時鐘引腳
//8=黃.9=橘,11=藍,12=綠
/////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  /*
    IPAddress ip(192,168,0,121);
    IPAddress dn(8,8,8,8);
    IPAddress gw(192,168,1,1);
    IPAddress sb(255,255,255,0);
    WiFi.config(ip, dn, gw, sb);*/
  // Start the server
  server.begin();


  printWifiStatus();

  ////初始化 74HC595////////////////////////////////////
  pinMode(elatchPin, OUTPUT);
  pinMode(eclockPin, OUTPUT);
  pinMode(edataPin, OUTPUT);
  //////////////////////////////////
  ////初始化 74HC165////////////////////////////////////
  pinMode(ploadPin, OUTPUT);
  pinMode(clockEnablePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(clockPin, LOW);
  digitalWrite(ploadPin, HIGH);
  //////////////////////////////////

}

int cout = 0;
void loop() {
  //ww01(); //595測試
  ww02(); //165測試
  
}
///////////////////////////////////////////////////////////////
//////////////165~/////////////////////////////////////////////////////
void ww02(){
    if (cout == 0) {
    if (Serial.available() <= 0) return;
    //char a = Serial.read();
    String aa = Serial.readString();
    Serial.println("STA");
  }
///////////////////////////////////////
  cout++;

  Serial.print(tt1()); Serial.print(" <= "); Serial.println(cout);
  
  } 

String tt1()
{
  delay(3000);
  //觸發並行Load以鎖存數據線的狀態
  digitalWrite(clockEnablePin, HIGH);
  digitalWrite(ploadPin, LOW);
  delayMicroseconds(PULSE_WIDTH_USEC);
  digitalWrite(ploadPin, HIGH);
  digitalWrite(clockEnablePin, LOW);

  String L = "", u;

  for (int i = 0; i < 16; i++) //有兩個74HC165n晶片,每個IC有8個狀態輸入,所以共有16個狀態輸入.
  {
    byte j = digitalRead(dataPin);//


    L += String(j); if (i % 4 == 3) L += ",";

    digitalWrite(clockPin, HIGH);//
    delayMicroseconds(PULSE_WIDTH_USEC);
    digitalWrite(clockPin, LOW);
  }

  return L;
}








//////////////////////////////////////////////////////////////////
//////////////////595~////////////////////////////////////////////////
void ww01() {
  if (Serial.available() <= 0) return;
  //char a = Serial.read();
  String aa = Serial.readString();
  //---------------------------------------------------
  t99(aa);
}


void t99(String s) {
  if (s == "tt") {
    tin();
    return;
  }
  String sp = " ";
  String C[2];
  myhs_IStrSplit(C, s, sp, 2);
  Serial.print("Val = "); Serial.println(s);
  Serial.print("C1 C2 = "); Serial.print(C[0]); Serial.print(" : "); Serial.println(C[1]);

  byte A = (byte)(C[1].toInt());
  byte B = (byte)(C[0].toInt());
  digitalWrite(elatchPin, LOW);   // 關上閘門
  shiftOut(edataPin, eclockPin, LSBFIRST, A); //A
  shiftOut(edataPin, eclockPin, LSBFIRST, B); //B
  digitalWrite(elatchPin, HIGH);  // 開啟閘門
  //Serial.print("END "); Serial.println(s);
}

void tin() {
  byte a, b;

  t99("0 0");

  for (int i = 0; i < 16; i++) {
    delay(100);
    if (i == 0) {
      a = 1;
      b = 1;
    }
    else {
      if (i < 8) {
        a <<= 1;
        b <<= 1;
      }
      else {
        a >>= 1;
        b >>= 1;
      }
    }
    t99(String(a) + " " + String(b));
  }

  delay(200);
  t99("255 255");
  delay(200);
  t99("0 0");
  delay(200);
  t99("255 255");
  delay(200);
  t99("0 0");
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());


  IPAddress p = WiFi.localIP();
  Serial.print("IP Address: "); Serial.println(p);

  p = WiFi.subnetMask();
  Serial.print("IP subnetMask: "); Serial.println(p);

  p = WiFi.gatewayIP();
  Serial.print("IP gatewayIP: "); Serial.println(p);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

/////////////////////////////////////////////////////
//字元,字串,分割等處理
/////////////////////////////////////////////////////
char* hm_multi_tok(char *i, char *d) {
  static char *s;
  if (i != NULL)
    s = i;

  if (s == NULL)
    return s;

  char *end = strstr(s, d);
  if (end == NULL) {
    char *t = s;
    s = NULL;
    return t;
  }

  char *t = s;

  *end = '\0';
  s = end + strlen(d);
  return t;
}

//搜尋字串計數
int myhs_strstrCount(char* m, char* s) {
  //m:被搜尋的字串. s:要搜尋的字
  int c = 0; const char *t = m;
  while (t = strstr(t, s)) {
    c++;
    t++;
  }
  return c;
}

//char 字串替換
void myhs_replace(char* str, char* a, char* b) {
  //str:字串. a:要被替換的字. b:替換後的字
  int len  = strlen(str);
  int lena = strlen(a), lenb = strlen(b);
  for (char* p = str; p = strstr(p, a); ++p) {
    if (lena != lenb) // shift end as needed
      memmove(p + lenb, p + lena,
              len - (p - str) + lenb);
    memcpy(p, b, lenb);
  }
}

/*/單的字元分割(這有問題,暫時勿用)
  void myhs_split(char **a, char *r, const char *d) {
  //a:字串陣列. r:被搜尋者字串. d:分割的'字元'
  char *s = strtok(r, d);
  while (s != NULL) {
     a++ = s;
    s = strtok(NULL, d);
  }
  }*/



/*/將String Type to char* Type
  char* myhs_StringToChar(String s) {
  int j = s.length() + 1; char t[j];
  s.toCharArray(t, j);
  return (char*)t;
  }*/

///多個的字元分割 //ok
void myhs_msplit(char **a, char *s, char *d) {
  //a:字串陣列. s:被搜尋者字串. d:分割的"字串"
  char *r = hm_multi_tok(s, d);
  while (r != NULL) {
    *a++ = r;
    r = hm_multi_tok(NULL, d);
  }
}

/*/多個的字元分割(須配合hm_multi_tok函數) //回傳的陣列指標必須用 "delete[] 變數名;" 來清除;
  char** myhs_msplit(char *s, const char *d) {
  //s:被搜尋者字串. d:分割的"字串"
  int c = myhs_strstrCount(s, d);
  char **p = new char*[++c];
  myhs_msplit(p, s, d);
  return p;
  }*/

/*/多個的字元分割(須配合hm_multi_tok函數) //回傳的陣列指標必須用 "delete[] 變數名;" 來清除;
  char** myhs_msplit(char *s, const char *d, int *i) {
  //s:被搜尋者字串. d:分割的"字串". i:分割後的陣列長度
  int c = myhs_strstrCount(s, d); c++; *i = c;
  char **p = new char*[c];
  myhs_msplit(p, s, d);
  return p;
  }*/

///字串分割 //回傳的陣列指標必須用 "delete[] 變數名;" 來清除;
void myhs_RStrSplit(String* a, char *s, char *d, int *i) {
  //a:準備分割後好要放進來的陣列. s:被搜尋者字串. d:分割的"字串". i:分割後的陣列長度
  int c = myhs_strstrCount(s, d); c++; *i = c;
  char* p[c];
  myhs_msplit(p, s, d);
  for (int j = 0; j < c; j++) a[j] = p[j];
}

///字串分割
void myhs_IStrSplit(String* a, char *s, char *d, int i) {
  ///a:準備分割後好要放進來的陣列. s:被搜尋者字串. d:分割的"字串". i:分割後的"指定"要放進來的元素數,該值不得超過a的長度
  int c = myhs_strstrCount(s, d); c++;
  char* p[c];
  myhs_msplit(p, s, d);
  for (int j = 0; j < i; j++) a[j] = p[j];
}

///字串分割
void  myhs_RStrSplit(String* a, String r, String d, int *i) {
  //a:準備分割後好要放進來的陣列. s:被搜尋者字串. d:分割的"字串". i:分割後的陣列長度
  int l = d.length() + 1; char t[l];
  d.toCharArray(t, l);

  int c , j  =  r.length() + 1;
  char s[j]; r.toCharArray(s, j);
  c = myhs_strstrCount(s, t); c++; *i = c;
  char* p[c];
  myhs_msplit(p, s, t);
  for (j = 0; j < c; j++) a[j] = p[j];
}

///字串分割
void  myhs_IStrSplit(String* a, String r, String d, int i) {
  ///a:準備分割後好要放進來的陣列. s:被搜尋者字串. d:分割的"字串". i:分割後的"指定"要放進來的元素數,該值不得超過a的長度
  int l = d.length() + 1; char t[l];
  d.toCharArray(t, l);

  int c , j  =  r.length() + 1;
  char s[j]; r.toCharArray(s, j);
  c = myhs_strstrCount(s, t); c++;
  char* p[c];
  myhs_msplit(p, s, t);
  for (j = 0; j < i; j++) a[j] = p[j];
}
/////////////////////////////////////////////////////
//字元,字串,分割等處理
/////////////////////////////////////////////////////
