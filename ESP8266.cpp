#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUDP.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <time.h>

String deviceSerial = "IP0115";
String apPassword   = "";

const char DATASET_CSV[] PROGMEM = R"=====(
Date,Special day,name,name2,name3
01.01.,Jaunais gads,Laimnesis,Solvija,
02.01.,Science Fiction Day,Indulis,Iva,Ivis
03.01.,Festival of Sleep Day,Miervaldis,Ringolds,Miervalda
04.04.,Tell a Lie Day,Valda,Herta,Arvalds
05.04.,Read a Road Map Day,Vija,Vidaga,Aivija
06.04.,Table Tennis Day,Zile,Zenta,Vilips
07.04.,Pasaules veselibas diena,Zina,Zinaida,Helmuts
08.04.,Draw a Bird Day,Edgars,Danute,Dana
09.04.,Name Yourself Day,Valerija,Zuhra,
10.04.,Siblings Day,Anita,Anitra,Zile
11.04.,Pet Day,Vilmars,Hermanis,
12.04.,Grilled Cheese Day,Julijs,Ainis,
13.04.,Scrabble Day,Egils,Egils,Nauris
14.04.,Look up at the Sky Day,Strauja,Gudrite,
15.04.,World Art Day,Aelita,Gastons,
16.04.,Wear Pajamas to Work Day,Alfs,Bernadete,
17.04.,Haiku Poetry Day,Rudolfs,Rudis,Viviana
18.04.,Pinata Day,Laura,Jadviga,
19.04.,Bicycle Day,Venta,Fanija,
20.04.,Look Alike Day,Mirta,Ziedite,
21.04.,Tea Day,Anastasija,Margers,
22.04.,Zemes diena,Armands,Armanda,
23.04.,World Book Day,Juris,Jurgis,Georgs
24.04.,Pig in a Blanket Day,Visvaldis,Nameda,Ritums
25.04.,Penguin Day,Liksma,Barbala,
26.04.,Pretzel Day,Alina,Sandris,Rusins
27.04.,Morse Code Day,Tale,Raimonda,Raina
28.04.,Astronomy Day,Gundega,Tereze,
29.04.,International Dance Day,Vilnis,Raimonds,Laine
30.04.,Honesty Day,Lilija,Liana,
01.05.,Darba svetki,Ziedonis,,
02.05.,Password Day,Zigmunds,Zigismunds,Sigmunds
03.05.,Space Day,Gints,Uvis,
04.05.,Neatkaribas atjaunosana,Vizma,Klavs,
05.05.,Cinco de Mayo,Girts,Gedimins,
06.05.,No Diet Day,Gaidis,Didzis,
07.05.,Cosmos Day,Henriete,Henrijs,Jete
08.05.,Nacisma sagraves diena,Stanislavs,Stanislava,Stefanija
09.05.,Eiropas diena,Klavs,Einars,Ervins
10.05.,Clean Up Your Room Day,Maija,Paija,
11.05.,Eat What You Want Day,Milda,Karmena,Manfreds
12.05.,Limerick Day,Inara,Ina,Inars
13.05.,Frog Jumping Day,Irena,Irina,Ira
14.05.,Dance Like a Chicken Day,Krisjanis,Elfa,Aivita
15.05.,Chocolate Chip Day,Sofija,Taiga,Airisa
16.05.,Love a Tree Day,Edvins,Edijs,
17.05.,Pack Rat Day,Herberts,Dailis,Umberts
18.05.,Museum Day,Inese,Inesis,Eriks
19.05.,May Ray Day,Lita,Sibilla,Teodors
20.05.,Be a Millionaire Day,Venta,Salvis,Selva
21.05.,Talk Like Yoda Day,Ernestine,Ingmars,Akvelina
22.05.,Musical Instrument Day,Emilija,,
23.05.,Turtle Day,Leontine,Leokadija,Lonija
24.05.,Scavenger Hunt Day,Ilmars,Marlena,
25.05.,Towel Day,Anslavs,Junijs,
26.05.,Paper Airplane Day,Eduards,Edvards,Varijs
27.05.,Sun Screen Day,Dzidra,Dzidris,Gunita
28.05.,Hamburger Day,Vilis,Vilhelms,
29.05.,Pillow on Fridge Day,Maksis,Maksims,Rauls
30.05.,Water a Flower Day,Vitolds,Lolita,
31.05.,Macaroon Day,Alida,Jusma,
01.06.,Bernu aizsardzibas diena,Biruta,Mairita,Bernedine
02.06.,Leave Office Early Day,Emma,Liba,
03.06.,Repeat Day,Inta,Ineta,Intra
04.06.,Hug Your Cat Day,Elizabete,Betija,Lucija
05.06.,Pasaules vides diena,Igors,Margots,Ingvars
06.06.,Drive In Movie Day,Ingrida,Ardis,
07.06.,Chocolate Ice Cream Day,Gaida,Arnis,
08.06.,Best Friends Day,Fridis,Fridrihs,Munda
09.06.,Donald Duck Day,Ligita,Gita,
10.06.,Iced Tea Day,Anatols,Anatolijs,
11.06.,Corn on the Cob Day,Ingus,Mairis,Vidvuds
12.06.,Red Rose Day,Nora,Lenora,Ija
13.06.,Sewing Machine Day,Zigfrids,Ainars,Uva
14.06.,Genocida upuru diena,Tija,Saiva,Sentis
15.06.,Nature Photography Day,Baiba,Barbara,Barba
16.06.,Fudge Day,Justine,Juta,
17.06.,Okupacijas diena,Arturs,Artis,
18.06.,International Picnic Day,Alberts,Madis,
19.06.,Garfield Day,Viktors,Nils,
20.06.,Ice Cream Soda Day,Rasma,Maira,Rasa
21.06.,Pasaules muzikas diena,Emils,Egita,Monvids
22.06.,Varonu diena,Ludmila,Laimdots,
23.06.,Ligo diena,Liga,,
24.06.,Jani,Janis,,
25.06.,Color TV Day,Maiga,Milija,
26.06.,Chocolate Pudding Day,Ausma,Inguna,Inguna
27.06.,Sunglasses Day,Malvine,Malva,
28.06.,Caps Lock Day,Viesturs,Kitija,Viests
29.06.,Camera Day,Peteris,Pavils,Pauls
30.06.,Meteor Day,Talivaldis,Mareks,
01.07.,International Joke Day,Teodors,Imants,Rimants
02.07.,UFO Day,Lauma,Ilmars,
03.07.,Plastic Bag Free Day,Benita,Everita,Verita
04.07.,Ebreju upuru diena,Ulvis,Uva,Sandijs
05.07.,Workaholics Day,Andzs,Andzejs,Edite
06.07.,Fried Chicken Day,Anrijs,Arkadijs,
07.07.,World Chocolate Day,Aldis,Alfons,
08.07.,Video Games Day,Antra,Adele,Ada
09.07.,Sugar Cookie Day,Asne,Asna,
10.07.,Pina Colada Day,Lija,Olivija,
11.07.,Cheer Up Day,Leonora,Svens,
12.07.,Simplicity Day,Indrikis,Ints,Namejs
13.07.,French Fries Day,Margrieta,Margarita,
14.07.,Mac and Cheese Day,Oskars,Ritvars,Anvars
15.07.,Give Something Away Day,Egons,Egija,Egmonts
16.07.,World Snake Day,Hermine,Estere,
17.07.,World Emoji Day,Alekss,Aleksis,
18.07.,Listening Day,Rozalija,Roze,
19.07.,Ice Cream Day,Jautrite,Kamila,Digna
20.07.,Moon Day,Ritma,Ramona,
21.07.,Junk Food Day,Meldra,Meldris,Melisa
22.07.,Hammock Day,Marija,Marika,Marina
23.07.,Vanilla Ice Cream Day,Magda,Magone,Merija
24.07.,Cousins Day,Kristine,Kristiana,Krista
25.07.,Thread the Needle Day,Jekabs,Zaklina,
26.07.,Aunt and Uncle Day,Anna,Ance,Annija
27.07.,Pants for a Walk Day,Marta,Dita,Ditte
28.07.,Milk Chocolate Day,Cecilija,Cilda,
29.07.,Lasagna Day,Edmunds,Edzus,Vidmants
30.07.,Cheesecake Day,Valters,Renars,Regnars
31.07.,Mutt Day,Ruta,Ruta,Angelika
01.08.,Spider Man Day,Albins,Albina,
02.08.,Coloring Book Day,Normunds,Stefans,
03.08.,Watermelon Day,Augusts,,
04.08.,Chocolate Cookie Day,Romans,Romualds,Romualda
05.08.,Underwear Day,Osvalds,Arvils,
06.08.,Fresh Breath Day,Askolds,Aisma,
07.08.,Lighthouse Day,Alberts,Madis,
08.08.,International Cat Day,Mudite,Vladislavs,Vladislava
09.08.,Book Lovers Day,Madara,Genovefa,
10.08.,Lazy Day,Brencis,Audris,Inuta
11.08.,Brivibas cinitaju diena,Olga,Zita,Zigita
12.08.,Middle Child Day,Vizma,Klavs,
13.08.,Left Handers Day,Elvira,Velda,Madis
14.08.,Creamsicle Day,Zelma,Zemgus,Virma
15.08.,Relaxation Day,Zenta,Zelda,Dzelme
16.08.,Tell a Joke Day,Astra,Astrida,
17.08.,Black Cat Day,Vineta,Olegs,
18.08.,Fajita Day,Liene,Helena,Elena
19.08.,Photography Day,Melanija,Imanta,
20.08.,Radio Day,Bernards,Boriss,
21.08.,Konstitucionalais likums,Linda,Janina,
22.08.,Be an Angel Day,Rudite,Everts,
23.08.,Ride the Wind Day,Vitalijs,Ralfs,Valgudis
24.08.,Pluto Demoted Day,Bertulis,Boleslavs,
25.08.,Banana Split Day,Ludvigs,Ludis,Ivonna
26.08.,Dog Day,Natalija,Talija,Bronislavs
27.08.,Just Because Day,Zanis,Jens,Jorens
28.08.,Bow Tie Day,Auguste,Guste,
29.08.,More Herbs Day,Armins,Vismants,Aiga
30.08.,Frankenstein Day,Alvis,Jolanta,Samanta
31.08.,Eat Outside Day,Vilma,Aigars,
01.09.,Zinibu diena,Ilmars,Austrums,
02.09.,Coconut Day,Eliza,Lizete,Zete
03.09.,Skyscraper Day,Berta,Bella,
04.09.,Extra Dessert Day,Dzintra,Dzintara,Dzintars
05.09.,Cheese Pizza Day,Klaudija,Persijs,Vaida
06.09.,Read a Book Day,Maigonis,Magnuss,Mariuss
07.09.,Salami Day,Regina,Ermins,
08.09.,Fizioterapijas diena,Ilona,Adelheid,
09.09.,Teddy Bear Day,Bruno,Telma,
10.09.,Swap Ideas Day,Janis,Salins,
11.09.,Make Your Bed Day,Signe,Signija,
12.09.,Chocolate Milkshake Day,Erna,Evita,Eva
13.09.,Positive Thinking Day,Iza,Izabella,
14.09.,Cream Filled Donut Day,Sanita,Santa,Sanda
15.09.,Make a Hat Day,Sandra,Gunvaldis,Sondra
16.09.,Guacamole Day,Asja,Asnate,Dags
17.09.,Apple Dumpling Day,Vera,Vaira,Vairis
18.09.,Cheeseburger Day,Liesma,Elita,Alita
19.09.,Talk Like a Pirate Day,Verners,Muntis,
20.09.,Pepperoni Pizza Day,Guntra,Ginters,Marianna
21.09.,Miniature Golf Day,Modris,Mariss,Matiss
22.09.,Baltu vienibas diena,Maris,Marica,Maigurs
23.09.,Checkers Day,Vane,Vanda,Veneranda
24.09.,Punctuation Day,Agrita,Agrita,
25.09.,Comic Book Day,Rodrigo,Rauls,
26.09.,Pancake Day,Gundars,Kurts,Knuts
27.09.,Tourism Day,Adolfs,Ilgonis,
28.09.,Good Neighbor Day,Sergejs,Svetlana,Lana
29.09.,Coffee Day,Mihails,Mikus,Mikelis
30.09.,Mud Pack Day,Elma,Menarda,Elna
01.10.,Senioru diena,Zanda,Lasma,Zandis
02.10.,Name Your Car Day,Ilma,Skaidris,
03.10.,Techies Day,Elza,Ilizana,
04.10.,Taco Day,Francis,Dmitrijs,
05.10.,Skolotaju diena,Amalija,Amelija,
06.10.,Mad Hatter Day,Monika,Zilgma,
07.10.,Frappe Day,Daudze,Daumants,Druvis
08.10.,Octopus Day,Aina,Anete,
09.10.,Curious Events Day,Elviss,Elva,Elvijs
10.10.,Garigas veselibas diena,Arvids,Arvis,Druvis
11.10.,Sausage Pizza Day,Monta,Tince,Silva
12.10.,Farmers Day,Valfrids,Kira,
13.10.,Train Your Brain Day,Irma,Mirga,
14.10.,Dessert Day,Vilhelmine,Minna,
15.10.,Math Day,Eda,Hedviga,Helvijs
16.10.,Dictionary Day,Daiga,Dinija,
17.10.,Pasta Day,Gaits,Karina,Erlends
18.10.,No Beard Day,Rolands,Ronalds,Erlends
19.10.,Evaluate Your Life Day,Elina,Drosma,Drosmis
20.10.,Sloth Day,Leonids,Leonida,
21.10.,Reptile Awareness Day,Urzula,Severins,
22.10.,Nut Day,Irida,Irisa,
23.10.,iPod Day,Daina,Dainis,Dainida
24.10.,Bologna Day,Renate,Modrite,Mudrite
25.10.,Greasy Foods Day,Beate,Beatrise,
26.10.,Pumpkin Day,Amanda,Kaiva,Amanta
27.10.,Black Cat Day,Irita,Itija,Lilita
28.10.,Chocolate Day,Antonina,Nina,Oksana
29.10.,Internet Day,Laimonis,Elvijs,Elvis
30.10.,Candy Corn Day,Ulla,Adina,
31.10.,Helovins,Rinalds,Rinalda,Valts
01.11.,Visu sveto diena,Ikars,,
02.11.,Dveselu diena,Vivita,Dzile,Viva
03.11.,Sandwich Day,Erika,Dagnija,
04.11.,Candy Day,Atis,Oto,Otomars
05.11.,Doughnut Day,Sarlote,Lote,
06.11.,Nachos Day,Linards,Leons,Leonards
07.11.,Bittersweet Chocolate Day,Helma,Lotars,
08.11.,Tongue Twister Day,Aleksandra,Agra,
09.11.,Chaos Never Dies Day,Teodors,,
10.11.,Martindiena,Martins,Marcis,Markuss
11.11.,Lacplesa diena,Nellija,Cornelija,
12.11.,Chicken Soup Day,Kaija,Kornelijs,
13.11.,World Kindness Day,Eizenija,Eizens,
14.11.,Pickle Day,Fricis,Vikentijs,
15.11.,Clean Refrigerator Day,Leopolds,Undine,Unda
16.11.,Fast Food Day,Banga,Glorija,
17.11.,Take A Hike Day,Hugo,Uga,Ugis
18.11.,Latvijas dzimsanas diena,Aleksandrs,Doloresa,
19.11.,Viriesu diena,Elizabete,Betija,Liza
20.11.,Absurdity Day,Anda,Andina,
21.11.,World Television Day,Zeltite,Andis,
22.11.,Go For a Ride Day,Aldis,Alfons,Aldis
23.11.,Fibonacci Day,Zigrida,Zigra,Sigrida
24.11.,Unique Talent Day,Velta,Velda,
25.11.,Parfait Day,Katrina,Kate,Kadrija
26.11.,Cake Day,Konrads,Sebastians,
27.11.,Pins and Needles Day,Lauris,Norberts,
28.11.,French Toast Day,Rita,Vita,Olita
29.11.,Electronic Greetings Day,Ignats,Virginia,
30.11.,Computer Security Day,Andrievs,Andrejs,Andris
01.12.,Eat a Red Apple Day,Arnolds,Emanuels,
02.12.,National Mutt Day,Meta,Sniedze,
03.12.,Make a Gift Day,Evija,Raita,Jogita
04.12.,Cookie Day,Baiba,Barbara,Barba
05.12.,Ninja Day,Sabine,Sarma,Klaudijs
06.12.,Microwave Oven Day,Nikolajs,Niklavs,Niks
07.12.,Cotton Candy Day,Antonijs,Antons,
08.12.,Time Traveler Day,Gunars,Vladimirs,Gunis
09.12.,Pastry Day,Sarmite,Tabita,
10.12.,Cilvektiesibu diena,Guna,Judite,
11.12.,Mountain Day,Voldemars,Valdemars,Valdis
12.12.,Gingerbread House Day,Otilija,Iveta,
13.12.,Violin Day,Lucija,Velta,
14.12.,Monkey Day,Aivars,Valerijs,
15.12.,Cupcake Day,Johanna,Hanna,Jana
16.12.,Chocolate Covered Day,Alvine,,
17.12.,Maple Syrup Day,Hildegarde,Teiksma,
18.12.,Bake Cookies Day,Kristaps,Kristofers,Krists
19.12.,Oatmeal Muffin Day,Lelde,Sarmis,
20.12.,Go Caroling Day,Arta,Abrams,
21.12.,Ziemas saulgriezi,Tomass,Toms,Dzintra
22.12.,Mathematics Day,Saulcerite,,
23.12.,Festivus,Viktorija,Balva,
24.12.,Ziemassvetku vakars,Adams,Ieva,
25.12.,Pirmie Ziemassvetki,Stella,Larisa,
26.12.,Otrie Ziemassvetki,Dainuvite,Gija,Megija
27.12.,Fruitcake Day,Elmars,Helmars,Inita
28.12.,Card Playing Day,Inguna,Inguna,Gidons
29.12.,Tick Tock Day,Solveiga,Ilgona,
30.12.,Bacon Day,Davids,Davis,Daniela
31.12.,Vecgada vakars,Silvestrs,Silvis,Kalvis
)=====";

const int  BUZZER_PIN = 12;
const int  BUTTON_PIN = 4;
const byte DNS_PORT   = 53;

struct Config {
  String ssid      = "";
  String pass      = "";
  String cal_api   = "";
  String timey_api = "https://thetimey.com/api/public/rt/a2d8f81a31a3e33b3314cc2809fc8037?stopId=";
  String lat       = "56.953074";
  String lon       = "24.081515";
  int    utc       = 3;
  int    t_freq    = 2;
  int    w_ref     = 30;
  bool   rot180    = false; 
  String bdays     = "";
  String alarms    = "";
  String s1_id     = ""; String s1_nm = "Stop 1"; int s1_del = 0; String s1_flt = "";
  String s2_id     = ""; String s2_nm = "Stop 2"; int s2_del = 0; String s2_flt = "";
} cfg;

ESP8266WebServer server(80);
DNSServer        dnsServer;
bool             isConfigMode     = true;
bool             routesRegistered = false;

unsigned long buttonPressTime = 0;
bool          buttonHandled   = false;

WiFiClientSecure wifiClient;
WiFiUDP          udp;
NTPClient        ntp(udp, "pool.ntp.org", 3 * 3600);

void initPassword() {
  if (LittleFS.exists("/ap_pass.json")) {
    File file = LittleFS.open("/ap_pass.json", "r");
    DynamicJsonDocument doc(256);
    if (!deserializeJson(doc, file)) {
      apPassword = doc["pass"] | "";
    }
    file.close();
  }
  
  if (apPassword.length() != 10) {
    randomSeed(micros() ^ analogRead(0));
    apPassword = "";
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for(int i=0; i<10; i++) apPassword += charset[random(0, 62)];
    
    File file = LittleFS.open("/ap_pass.json", "w");
    DynamicJsonDocument doc(256);
    doc["pass"] = apPassword;
    serializeJson(doc, file);
    file.close();
  }
}

void playMelody(int id) {
  if      (id==1){ int m[]={262,330,392,523};     for(int i=0;i<4;i++){tone(BUZZER_PIN,m[i],150);delay(200);} noTone(BUZZER_PIN); }
  else if (id==2){ tone(BUZZER_PIN,880,80);delay(120);tone(BUZZER_PIN,880,80);delay(200);tone(BUZZER_PIN,1318,250);noTone(BUZZER_PIN); }
  else if (id==3){ for(int hz=400;hz<1000;hz+=100){tone(BUZZER_PIN,hz,50);delay(70);}noTone(BUZZER_PIN); }
  else if (id==4){ for(int i=0;i<2;i++){tone(BUZZER_PIN,349,150);delay(200);tone(BUZZER_PIN,440,150);delay(200);}noTone(BUZZER_PIN); }
  else if (id==5){ int m[]={262,330,392,494,523}; for(int i=0;i<5;i++){tone(BUZZER_PIN,m[i],120);delay(140);}noTone(BUZZER_PIN); }
  else if (id==6){ tone(BUZZER_PIN,523,100);delay(100);tone(BUZZER_PIN,659,100);delay(100);tone(BUZZER_PIN,784,100);delay(100);tone(BUZZER_PIN,1046,300);noTone(BUZZER_PIN); }
}

void checkAlarms(int h, int m, int s, int wday) {
  String alrmList = cfg.alarms + "|"; int start = 0;
  while (start < (int)alrmList.length()) {
    int end = alrmList.indexOf('|', start); if (end == -1) break;
    String a = alrmList.substring(start, end);
    int c1 = a.indexOf(','); int c2 = a.indexOf(',', c1+1);
    if (c1>0 && c2>0) {
      int melody = a.substring(0,c1).toInt();
      String tStr = a.substring(c1+1,c2); String days = a.substring(c2+1);
      int ah=tStr.substring(0,2).toInt(), am=tStr.substring(3,5).toInt(), as2=tStr.substring(6,8).toInt();
      String dc = String(wday==0?7:wday);
      if (h==ah && m==am && s==as2 && days.indexOf(dc)>=0) playMelody(melody);
    }
    start = end+1;
  }
}

void loadConfig() {
  if (!LittleFS.begin()) return;
  initPassword(); 
  
  if (!LittleFS.exists("/config.json")) return;
  File file = LittleFS.open("/config.json", "r");
  DynamicJsonDocument doc(2048);
  if (!deserializeJson(doc, file)) {
    cfg.ssid      = doc["ssid"]      | "";
    cfg.pass      = doc["pass"]      | "";
    cfg.cal_api   = doc["cal_api"]   | "";
    cfg.timey_api = doc["timey_api"] | "https://thetimey.com/api/public/rt/a2d8f81a31a3e33b3314cc2809fc8037?stopId=";
    cfg.lat       = doc["lat"]       | "56.953074";
    cfg.lon       = doc["lon"]       | "24.081515";
    cfg.utc       = doc["utc"]       | 3;
    cfg.t_freq    = doc["t_freq"]    | 2;
    cfg.w_ref     = doc["w_ref"]     | 30;
    cfg.rot180    = doc["rot180"]    | false;
    cfg.s1_id     = doc["s1_id"]     | ""; cfg.s1_nm = doc["s1_nm"] | "Stop 1";
    cfg.s1_del    = doc["s1_del"]    | 0;      cfg.s1_flt= doc["s1_flt"]| "";
    cfg.s2_id     = doc["s2_id"]     | ""; cfg.s2_nm = doc["s2_nm"] | "Stop 2";
    cfg.s2_del    = doc["s2_del"]    | 0;      cfg.s2_flt= doc["s2_flt"]| "";
    cfg.bdays     = doc["bdays"]     | "";
    cfg.alarms    = doc["alarms"]    | "";
    if (cfg.ssid.length() > 1) isConfigMode = false;
  }
  file.close();
}

void saveConfig() {
  DynamicJsonDocument doc(2048);
  doc["ssid"]=cfg.ssid; doc["pass"]=cfg.pass; doc["cal_api"]=cfg.cal_api; doc["timey_api"]=cfg.timey_api;
  doc["lat"]=cfg.lat;   doc["lon"]=cfg.lon;   doc["utc"]=cfg.utc;
  doc["t_freq"]=cfg.t_freq; doc["w_ref"]=cfg.w_ref; doc["rot180"]=cfg.rot180;
  doc["s1_id"]=cfg.s1_id; doc["s1_nm"]=cfg.s1_nm; doc["s1_del"]=cfg.s1_del; doc["s1_flt"]=cfg.s1_flt;
  doc["s2_id"]=cfg.s2_id; doc["s2_nm"]=cfg.s2_nm; doc["s2_del"]=cfg.s2_del; doc["s2_flt"]=cfg.s2_flt;
  doc["bdays"]=cfg.bdays; doc["alarms"]=cfg.alarms;
  File file = LittleFS.open("/config.json", "w"); serializeJson(doc, file); file.close();
}

String he(String s) {
  s.replace("&","&amp;"); s.replace("\"","&quot;"); s.replace("'","&#39;");
  s.replace("<","&lt;");  s.replace(">","&gt;");
  return s;
}

const char PAGE_CSS[] PROGMEM =
  "<!DOCTYPE HTML><html><head>"
  "<meta name='viewport' content='width=device-width,initial-scale=1'>"
  "<meta name='color-scheme' content='light dark'>"
  "<meta charset='UTF-8'>"
  "<title>KOJAS-SETUP</title><style>"
  "*{box-sizing:border-box;margin:0;padding:0;-webkit-font-smoothing:antialiased;}"
  "body{font-family:-apple-system,BlinkMacSystemFont,'SF Pro Text','Helvetica Neue',sans-serif;"
    "background:#f2f2f7;color:#1c1c1e;max-width:640px;margin:0 auto;padding:20px 16px 40px;}"
  "@media(prefers-color-scheme:dark){body{background:#1c1c1e;color:#f2f2f7;}}"
  "h1{font-size:28px;font-weight:700;letter-spacing:-.5px;margin-bottom:6px;color:#1c1c1e;}"
  "@media(prefers-color-scheme:dark){h1{color:#f2f2f7;}}"
  ".sub{font-size:14px;color:#8e8e93;margin-bottom:24px;}"
  ".card{background:#fff;border-radius:16px;padding:0;margin-bottom:12px;overflow:hidden;box-shadow:0 1px 0 rgba(0,0,0,.06);}"
  "@media(prefers-color-scheme:dark){.card{background:#2c2c2e;box-shadow:0 1px 0 rgba(0,0,0,.3);}}"
  ".card-hd{padding:14px 16px 0;font-size:13px;font-weight:600;text-transform:uppercase;letter-spacing:.5px;color:#8e8e93;}"
  ".row{display:flex;align-items:center;padding:12px 16px;border-bottom:.5px solid rgba(0,0,0,.08);gap:12px;}"
  "@media(prefers-color-scheme:dark){.row{border-bottom:.5px solid rgba(255,255,255,.06);}}"
  ".row:last-child{border-bottom:none;}"
  ".row label{font-size:15px;font-weight:400;color:#1c1c1e;min-width:120px;flex-shrink:0;display:flex;align-items:center;}"
  "@media(prefers-color-scheme:dark){.row label{color:#f2f2f7;}}"
  ".row input,.row select{flex:1;border:none;background:transparent;font-size:15px;color:#1c1c1e;text-align:right;outline:none;font-family:inherit;min-width:0;}"
  "@media(prefers-color-scheme:dark){.row input,.row select{color:#f2f2f7;}}"
  ".row input::placeholder{color:#c7c7cc;}"
  "@media(prefers-color-scheme:dark){.row input::placeholder{color:#48484a;}}"
  ".row input[type=number]{-moz-appearance:textfield;}"
  ".row input::-webkit-outer-spin-button,.row input::-webkit-inner-spin-button{-webkit-appearance:none;}"
  ".row input[type=checkbox]{flex:none;width:20px;height:20px;accent-color:#007aff;}"
  ".row-full{flex-direction:column;align-items:stretch;}"
  ".row-full label{margin-bottom:6px;text-align:left;min-width:auto;}"
  ".row-full input,.row-full textarea{text-align:left;background:#f2f2f7;border-radius:8px;padding:8px 12px;font-size:15px;border:none;outline:none;font-family:inherit;width:100%;}"
  "@media(prefers-color-scheme:dark){.row-full input,.row-full textarea{background:#3a3a3c;color:#f2f2f7;}}"
  "textarea{resize:vertical;min-height:80px;line-height:1.5;}"
  ".hint{font-size:12px;color:#8e8e93;margin-top:4px;}"
  ".split{display:flex;gap:0;width:100%;}"
  ".split .row{flex:1;min-width:0;padding:12px 10px;border-bottom:none;}"
  ".split .row:first-child{border-right:.5px solid rgba(0,0,0,.08);}"
  "@media(prefers-color-scheme:dark){.split .row:first-child{border-right:.5px solid rgba(255,255,255,.06);}}"
  ".split .row label{min-width:60px;}"
  ".alarm-item{background:#f2f2f7;border-radius:10px;padding:12px;margin:4px 16px 8px;position:relative;}"
  "@media(prefers-color-scheme:dark){.alarm-item{background:#3a3a3c;}}"
  ".del-btn{position:absolute;top:10px;right:10px;background:#ff3b30;color:#fff;border:none;border-radius:6px;padding:2px 10px;font-size:13px;cursor:pointer;font-family:inherit;}"
  ".days-row{display:flex;gap:6px;margin-top:8px;flex-wrap:wrap;}"
  ".day-lbl{display:flex;flex-direction:column;align-items:center;font-size:11px;gap:3px;color:#8e8e93;}"
  ".day-lbl input{width:18px;height:18px;accent-color:#007aff;margin:0;}"
  ".alarm-fields{display:flex;gap:10px;margin-bottom:4px;}"
  ".alarm-fields>div{flex:1;}"
  ".alarm-fields label{font-size:12px;color:#8e8e93;display:block;margin-bottom:3px;}"
  ".alarm-fields select,.alarm-fields input{width:100%;background:#fff;border:.5px solid rgba(0,0,0,.12);border-radius:8px;padding:7px 10px;font-size:14px;font-family:inherit;outline:none;color:#1c1c1e;}"
  "@media(prefers-color-scheme:dark){.alarm-fields select,.alarm-fields input{background:#2c2c2e;border-color:rgba(255,255,255,.1);color:#f2f2f7;}}"
  ".add-alarm{display:flex;align-items:center;gap:6px;padding:10px 16px 14px;font-size:15px;color:#007aff;cursor:pointer;border:none;background:transparent;font-family:inherit;width:100%;}"
  ".add-alarm:hover{opacity:.8;}"
  ".test-btn{background:transparent;border:.5px solid #007aff;color:#007aff;border-radius:6px;padding:3px 10px;font-size:12px;cursor:pointer;font-family:inherit;margin-top:6px;}"
  ".save-btn{width:100%;background:#007aff;color:#fff;border:none;border-radius:14px;font-size:17px;font-weight:600;padding:16px;margin-top:20px;cursor:pointer;font-family:inherit;letter-spacing:-.2px;}"
  ".save-btn:hover{background:#0071eb;}"
  ".save-btn:active{background:#0062cc;transform:scale(.99);}"
  ".info-box{display:none;padding:10px;background:#e5e5ea;border-radius:8px;font-size:13px;color:#1c1c1e;margin:0 16px 12px;line-height:1.4;}"
  "@media(prefers-color-scheme:dark){.info-box{background:#3a3a3c;color:#f2f2f7;}}"
  ".info-box a{color:#007aff;text-decoration:none;}"
  ".info-box a:hover{text-decoration:underline;}"
  "</style></head><body>";

void handleRoot() {
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html; charset=utf-8", "");
  server.sendContent(FPSTR(PAGE_CSS));

  String pg = "";
  pg += "<h1>KOJAS-" + he(deviceSerial) + "</h1>";
  pg += "<p class='sub'>Smart display settings</p>";
  pg += "<form id='mf' action='/save' method='POST'>";
  
  pg += "<div class='card'><div class='card-hd'>Wi-Fi</div>";
  pg += "<div class='row'><label>Network</label><input type='text' name='ssid' value='" + he(cfg.ssid) + "' placeholder='Network name'></div>";
  pg += "<div class='row'><label>Password</label><input type='password' name='pass' value='" + he(cfg.pass) + "' placeholder='Password'></div>";
  pg += "</div>";
  server.sendContent(pg);

  pg = "<div class='card'><div class='card-hd'>Location</div>";
  pg += "<div class='row'><div class='split'>";
  pg += "<div class='row'><label>Latitude</label><input type='text' name='lat' value='" + he(cfg.lat) + "' placeholder='56.946' pattern='^-?\\d{1,3}(\\.\\d{1,5})?$' title='Numbers only, max 5 decimal places' maxlength='10'></div>";
  pg += "<div class='row'><label>Longitude</label><input type='text' name='lon' value='" + he(cfg.lon) + "' placeholder='24.106' pattern='^-?\\d{1,3}(\\.\\d{1,5})?$' title='Numbers only, max 5 decimal places' maxlength='10'></div>";
  pg += "</div></div>";
  pg += "<div class='row'><label>UTC offset</label><input type='number' name='utc' value='" + String(cfg.utc) + "' min='-12' max='14' placeholder='3'></div>";
  pg += "</div>";
  server.sendContent(pg);

  pg = "<div class='card'><div class='card-hd'>Birthday list</div>";
  pg += "<div class='row row-full'><textarea name='bdays' placeholder=''>";
  String bdayVal = cfg.bdays; bdayVal.replace("</textarea>","");
  pg += he(bdayVal);
  pg += "</textarea><div class='hint'>One entry per line: Name (DD.MM.)</div></div></div>";
  server.sendContent(pg);

  pg = "<div class='card'><div class='card-hd'>Google Calendar</div>";
  pg += "<div class='row row-full'><label style='display:inline-block'>Apps Script URL <span style='color:#007aff;cursor:pointer;margin-left:8px;font-size:16px;' onclick='var el=document.getElementById(\"i_cal\");el.style.display=el.style.display===\"block\"?\"none\":\"block\";'>&#9432;</span></label>";
  pg += "<input type='password' name='cal_api' value='" + he(cfg.cal_api) + "' placeholder='https://script.google.com/macros/s/...'>";
  pg += "<div id='i_cal' class='info-box' style='margin:8px 0 0 0;'>1. Open script.google.com and create a new project.<br>2. Paste the code below, replacing the original.<br>3. Click \"Deploy\" -&gt; \"New deployment\".<br>4. Select type \"Web app\", set \"Who has access\" to \"Anyone\".<br>5. Copy the generated Web app URL and paste it here.</div>";
  pg += "<div class='hint'><br><button type='button' class='test-btn' onclick='cGas(this)'>Copy Google Apps Script code</button></div></div></div>";
  server.sendContent(pg);

  pg = "<div class='card'><div class='card-hd'>Transit Stop 1</div>";
  pg += "<div class='row'><div class='split'>";
  pg += "<div class='row'><label>ID <span style='color:#007aff;cursor:pointer;margin-left:6px;font-size:16px;' onclick='var el=document.getElementById(\"i_s1\");el.style.display=el.style.display===\"block\"?\"none\":\"block\";'>&#9432;</span></label><input type='text' name='s1_id' value='" + he(cfg.s1_id) + "' placeholder='0000' maxlength='8' pattern='[A-Za-z0-9]+'></div>";
  pg += "<div class='row'><label>Name</label><input type='text' name='s1_nm' value='" + he(cfg.s1_nm) + "' placeholder='Display name' maxlength='12'></div>";
  pg += "</div></div>";
  pg += "<div id='i_s1' class='info-box'>IDs can be obtained from the <a href='https://saraksti.lv' target='_blank'>saraksti.lv</a> link on the website. The ID is indicated in the URL when a stop is selected.</div>";
  pg += "<div class='row'><label>Walk time (min)</label><input type='number' name='s1_del' value='" + String(cfg.s1_del) + "' min='0' placeholder='0'></div>";
  pg += "<div class='row row-full'><label>Route filter</label><input type='text' name='s1_flt' value='" + he(cfg.s1_flt) + "' placeholder='bus13,trol25,tram11' pattern='^([a-zA-Z0-9]+(,[a-zA-Z0-9]+)*)?$' title='Format: bus13,tram1,25'></div></div>";
  server.sendContent(pg);

  pg = "<div class='card'><div class='card-hd'>Transit Stop 2</div>";
  pg += "<div class='row'><div class='split'>";
  pg += "<div class='row'><label>ID <span style='color:#007aff;cursor:pointer;margin-left:6px;font-size:16px;' onclick='var el=document.getElementById(\"i_s2\");el.style.display=el.style.display===\"block\"?\"none\":\"block\";'>&#9432;</span></label><input type='text' name='s2_id' value='" + he(cfg.s2_id) + "' placeholder='0000' maxlength='8' pattern='[A-Za-z0-9]+'></div>";
  pg += "<div class='row'><label>Name</label><input type='text' name='s2_nm' value='" + he(cfg.s2_nm) + "' placeholder='Display name' maxlength='12'></div>";
  pg += "</div></div>";
  pg += "<div id='i_s2' class='info-box'>IDs can be obtained from the <a href='https://saraksti.lv' target='_blank'>saraksti.lv</a> link on the website. The ID is indicated in the URL when a stop is selected.</div>";
  pg += "<div class='row'><label>Walk time (min)</label><input type='number' name='s2_del' value='" + String(cfg.s2_del) + "' min='0' placeholder='0'></div>";
  pg += "<div class='row row-full'><label>Route filter</label><input type='text' name='s2_flt' value='" + he(cfg.s2_flt) + "' placeholder='bus13,trol25,tram11' pattern='^([a-zA-Z0-9]+(,[a-zA-Z0-9]+)*)?$' title='Format: bus13,tram1,25'></div></div>";
  server.sendContent(pg);

  pg = "<div class='card'><div class='card-hd'>Alarms</div>";
  pg += "<div id='al'></div><button type='button' class='add-alarm' onclick='addA()'>&#65291; Add alarm</button>";
  pg += "<input type='hidden' name='alarms' id='ah'></div>";
  server.sendContent(pg);

  pg = "<div class='card'><div class='card-hd'>Refresh Intervals</div>";
  pg += "<div class='row'><label>Transit (per min)</label><input type='number' name='t_freq' value='" + String(cfg.t_freq) + "' min='1' max='6' placeholder='2'></div>";
  pg += "<div class='row'><label>Weather (min)</label><input type='number' name='w_ref' value='" + String(cfg.w_ref) + "' min='5' placeholder='30'></div></div>";
  server.sendContent(pg);

  pg = "<div class='card'><div class='card-hd'>Advanced</div>";
  pg += "<div class='row row-full'><label>Timey API base URL</label>";
  pg += "<input type='text' name='timey_api' value='" + he(cfg.timey_api) + "' placeholder='https://thetimey.com/api/...?stopId='>";
  pg += "<div class='row'><label>Rotate display 180&deg;</label><input type='checkbox' name='rot180' value='1' " + String(cfg.rot180 ? "checked" : "") + "></div>";
  pg += "</div><button type='submit' class='save-btn'>Save &amp; Upload</button></form>";
  server.sendContent(pg);

  String alarmRaw = cfg.alarms;
  alarmRaw.replace("\\","\\\\"); alarmRaw.replace("'","\\'");
  
  server.sendContent("<script>");
  server.sendContent("var gas=`function hexToRgb565(hex){\nhex=hex.replace('#','');\nif(hex.length===3){\nhex=hex[0]+hex[0]+hex[1]+hex[1]+hex[2]+hex[2];\n}\nvar r=parseInt(hex.substring(0,2),16);\nvar g=parseInt(hex.substring(2,4),16);\nvar b=parseInt(hex.substring(4,6),16);\nvar rgb565=((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3);\nvar hexString=rgb565.toString(16).toUpperCase();\nwhile(hexString.length<4)hexString='0'+hexString;\nreturn '0x'+hexString;\n}\nfunction getRgb565(gColorId){\nvar colors={'1':'0x7438','2':'0x35AF','3':'0x8914','4':'0xE3EE','5':'0xF5E4','6':'0xF283','7':'0x04DC','8':'0x630C','9':'0x3A96','10':'0x0808','11':'0xD000'};\nreturn colors[gColorId]||null;\n}\nfunction doGet(e){\nvar cal=CalendarApp.getDefaultCalendar();\nvar defaultCalHex=cal.getColor();\nvar defaultRgb565=hexToRgb565(defaultCalHex);\nvar now=new Date();\nvar nextWeek=new Date();\nnextWeek.setDate(now.getDate()+7);\nvar events=cal.getEvents(now,nextWeek);\nvar output='CAL:BEGIN\\n';\nvar today=new Date();\nvar tomorrow=new Date();\ntomorrow.setDate(today.getDate()+1);\nvar count=0;\nfor(var i=0;i<events.length&&count<5;i++){\nvar ev=events[i];\nvar start=ev.getStartTime();\nvar end=ev.getEndTime();\nvar dateStr='';\nif(start.getDate()==today.getDate()&&start.getMonth()==today.getMonth()){\ndateStr='Today';\n}else if(start.getDate()==tomorrow.getDate()&&start.getMonth()==tomorrow.getMonth()){\ndateStr='Tmrw.';\n}else{\ndateStr=Utilities.formatDate(start,'Europe/Riga','dd.MM');\n}\nvar timeStr='';\nif(ev.isAllDayEvent()){\ntimeStr='All Day '+dateStr;\n}else{\nvar t1=Utilities.formatDate(start,'Europe/Riga','HH:mm');\nvar t2=Utilities.formatDate(end,'Europe/Riga','HH:mm');\ntimeStr=t1+'-'+t2+' '+dateStr;\n}\nvar title=ev.getTitle().replace(/,/g,'');\nif(title.length>24){title=title.substring(0,21)+'...';}\nvar colorId=ev.getColor();\nvar finalColor=getRgb565(colorId)||defaultRgb565;\noutput+='CAL:EVT,'+timeStr+','+title+','+finalColor+'\\n';\ncount++;\n}\noutput+='CAL:END\\n';\nreturn ContentService.createTextOutput(output).setMimeType(ContentService.MimeType.TEXT);\n}`;\n");
  server.sendContent("function cGas(b){var t=document.createElement('textarea');t.value=gas;t.setAttribute('readonly','');t.style.position='absolute';t.style.left='-9999px';document.body.appendChild(t);t.select();t.setSelectionRange(0,99999);try{document.execCommand('copy');b.innerHTML='&#10003; Copied!';b.style.color='#34c759';b.style.borderColor='#34c759';setTimeout(function(){b.innerHTML='Copy Google Apps Script code';b.style.color='';b.style.borderColor='';},2000);}catch(e){}document.body.removeChild(t);}\n");
  server.sendContent("var M=[{v:1,l:'Chime'},{v:2,l:'Double beep'},{v:3,l:'Scale'},{v:4,l:'Two-tone'},{v:5,l:'Rising'},{v:6,l:'Fanfare'}];");
  server.sendContent("var D=['Mo','Tu','We','Th','Fr','Sa','Su'];var ad=[];var RAW='" + alarmRaw + "';");
  server.sendContent("function pa(r){if(!r||r.trim()=='')return[];return r.split('|').filter(function(s){return s.trim()!='';}).map(function(a){var p=a.split(',');if(p.length<3)return null;return{mel:parseInt(p[0]),tim:p[1],day:p[2]};}).filter(Boolean);}");
  server.sendContent("function addA(){ad.push({mel:1,tim:'08:00:00',day:'1234567'});ra();}");
  server.sendContent("function delA(i){ad.splice(i,1);ra();}");
  server.sendContent("function ra(){var c=document.getElementById('al');c.innerHTML='';for(var i=0;i<ad.length;i++){var a=ad[i];var div=document.createElement('div');div.className='alarm-item';var mo='';for(var k=0;k<M.length;k++){mo+='<option value=\"'+M[k].v+'\"'+(a.mel==M[k].v?' selected':'')+'>'+M[k].l+'</option>';}var dc='';for(var d=0;d<D.length;d++){var dn=d+1;var ch=(a.day.indexOf(String(dn))>=0)?'checked':'';dc+='<label class=\"day-lbl\"><span>'+D[d]+'</span><input type=\"checkbox\" data-d=\"'+dn+'\" '+ch+' onchange=\"sy('+i+')\"></label>';}div.innerHTML='<button type=\"button\" class=\"del-btn\" onclick=\"delA('+i+')\">Remove</button>';div.innerHTML+='<div class=\"alarm-fields\">';div.innerHTML+='<div><label>Melody  </label><select onchange=\"sy('+i+')\" class=\"ms\">'+mo+'</select></div>';div.innerHTML+='<div><label>Time  </label><input type=\"text\" value=\"'+a.tim+'\" placeholder=\"07:30:00\" class=\"ti\" onchange=\"sy('+i+')\"></div></div>';div.innerHTML+='<div class=\"days-row\">'+dc+'</div>';div.innerHTML+='<button type=\"button\" class=\"test-btn\" onclick=\"tm('+i+')\">&#9654; Test</button>';c.appendChild(div);}uh();}");
  server.sendContent("function sy(i){var e=document.querySelectorAll('.alarm-item')[i];ad[i].mel=parseInt(e.querySelector('.ms').value);ad[i].tim=e.querySelector('.ti').value;var cb=e.querySelectorAll('[data-d]');var dy='';for(var k=0;k<cb.length;k++){if(cb[k].checked)dy+=cb[k].dataset.d;}ad[i].day=dy;uh();}");
  server.sendContent("function uh(){var p=[];for(var i=0;i<ad.length;i++){p.push(ad[i].mel+','+ad[i].tim+','+ad[i].day);}document.getElementById('ah').value=p.join('|');}");
  server.sendContent("function tm(i){fetch('/t?id='+ad[i].mel);}");
  
  server.sendContent(
    "document.getElementById('mf').addEventListener('submit',function(e){"
      "function err(m){alert(m);e.preventDefault();return false;}"
      "var b=document.getElementsByName('bdays')[0].value.trim();"
      "if(b){var l=b.split('\\n');for(var i=0;i<l.length;i++){if(l[i].trim()&&!/^.+ \\(\\d{2}\\.\\d{2}\\.\\)$/.test(l[i].trim()))return err('Invalid Birthday format! Use: Name (DD.MM.)');}}"
      "uh();var ah=document.getElementById('ah').value;if(ah){var a=ah.split('|');for(var i=0;i<a.length;i++){var p=a[i].split(',');if(p.length==3&&!/^\\d{2}:\\d{2}:\\d{2}$/.test(p[1]))return err('Invalid alarm time! Use HH:MM:SS');}}"
      "var rx=/^([a-zA-Z0-9]+(,[a-zA-Z0-9]+)*)?$/; if((document.getElementsByName('s1_flt')[0].value&&!rx.test(document.getElementsByName('s1_flt')[0].value))||(document.getElementsByName('s2_flt')[0].value&&!rx.test(document.getElementsByName('s2_flt')[0].value)))return err('Filters must match format: tram1,bus23,15');"
      "var ll=/^-?\\d{1,3}(\\.\\d{1,5})?$/; if((document.getElementsByName('lat')[0].value&&!ll.test(document.getElementsByName('lat')[0].value))||(document.getElementsByName('lon')[0].value&&!ll.test(document.getElementsByName('lon')[0].value)))return err('Lat/Lon max 5 digits after comma');"
    "});ad=pa(RAW);ra();"
  );
  server.sendContent("</script></body></html>");
  server.sendContent(""); 
}

void handleCaptive() { server.sendHeader("Location","http://192.168.4.1/",true); server.send(302,"text/plain",""); }

void switchMode(bool toConfig);

String stripInj(String s) { s.replace(",", ""); s.replace("\n", ""); s.replace("\r", ""); return s; }
String stripNewlines(String s) { s.replace("\n", ""); s.replace("\r", ""); return s; }

void handleSave() {
  if(server.hasArg("ssid"))      cfg.ssid      = server.arg("ssid");
  if(server.hasArg("pass"))      cfg.pass      = server.arg("pass");
  if(server.hasArg("cal_api"))   cfg.cal_api   = server.arg("cal_api");
  if(server.hasArg("timey_api")) cfg.timey_api = server.arg("timey_api");
  if(server.hasArg("lat"))       cfg.lat       = server.arg("lat");
  if(server.hasArg("lon"))       cfg.lon       = server.arg("lon");
  if(server.hasArg("utc"))       cfg.utc       = server.arg("utc").toInt();
  if(server.hasArg("s1_id"))     cfg.s1_id     = stripInj(server.arg("s1_id")).substring(0, 8);
  if(server.hasArg("s1_nm"))     cfg.s1_nm     = stripInj(server.arg("s1_nm")).substring(0, 12);
  if(server.hasArg("s1_del"))    cfg.s1_del    = server.arg("s1_del").toInt();
  if(server.hasArg("s1_flt"))    cfg.s1_flt    = stripNewlines(server.arg("s1_flt"));
  if(server.hasArg("s2_id"))     cfg.s2_id     = stripInj(server.arg("s2_id")).substring(0, 8);
  if(server.hasArg("s2_nm"))     cfg.s2_nm     = stripInj(server.arg("s2_nm")).substring(0, 12);
  if(server.hasArg("s2_del"))    cfg.s2_del    = server.arg("s2_del").toInt();
  if(server.hasArg("s2_flt"))    cfg.s2_flt    = stripNewlines(server.arg("s2_flt"));
  if(server.hasArg("bdays"))     cfg.bdays     = server.arg("bdays");
  if(server.hasArg("alarms"))    cfg.alarms    = server.arg("alarms");
  if(server.hasArg("t_freq"))    cfg.t_freq    = server.arg("t_freq").toInt();
  if(server.hasArg("w_ref"))     cfg.w_ref     = server.arg("w_ref").toInt();
  cfg.rot180 = server.hasArg("rot180");
  
  saveConfig();
  server.send(200, "text/html",
    "<!DOCTYPE HTML><html><head><meta charset='UTF-8'><meta name='color-scheme' content='light dark'>"
    "<style>body{font-family:-apple-system,sans-serif;background:#f2f2f7;display:flex;align-items:center;"
    "justify-content:center;height:100vh;margin:0;}"
    "@media(prefers-color-scheme:dark){body{background:#1c1c1e;color:#f2f2f7;}}"
    ".card{background:#fff;border-radius:20px;padding:40px 48px;text-align:center;box-shadow:0 2px 20px rgba(0,0,0,.08);}"
    "@media(prefers-color-scheme:dark){.card{background:#2c2c2e;}}"
    "h2{font-size:22px;font-weight:700;margin-bottom:8px;color:#1c1c1e;}"
    "@media(prefers-color-scheme:dark){h2{color:#f2f2f7;}}"
    "p{color:#8e8e93;font-size:15px;}</style></head><body>"
    "<div class='card'><h2>Saved</h2><p>Check your KOJAS display!</p></div></body></html>");
  delay(500);
  switchMode(false);
}

void switchMode(bool toConfig) {
  isConfigMode = toConfig;

  if (isConfigMode) {
    server.stop(); dnsServer.stop();
    WiFi.softAPdisconnect(true);
    WiFi.disconnect(true); delay(100);
    WiFi.mode(WIFI_AP);    delay(100);

    IPAddress apIP(192, 168, 4, 1);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    
    String apName = "KOJAS-" + deviceSerial;
    WiFi.softAP(apName.c_str(), apPassword.c_str(), 6); delay(500);

    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

    if (!routesRegistered) {
      server.on("/",     HTTP_GET,  handleRoot);
      server.on("/save", HTTP_POST, handleSave);
      server.on("/t",    HTTP_GET,  [](){ playMelody(server.arg("id").toInt()); server.send(200,"text/plain","OK"); });
      server.onNotFound(handleCaptive);
      routesRegistered = true;
    }
    server.begin();
    Serial.println("CMD:MODE,CONFIG"); delay(20);
    Serial.println(cfg.rot180 ? "CMD:ROT,1" : "CMD:ROT,0"); delay(20);
    Serial.println("CMD:AP_INFO," + apName + "," + apPassword); 

  } else {
    server.stop(); dnsServer.stop();
    WiFi.softAPdisconnect(true);
    WiFi.disconnect(true); delay(100);
    WiFi.mode(WIFI_STA); delay(100);
    WiFi.setAutoReconnect(true);
    WiFi.begin(cfg.ssid.c_str(), cfg.pass.c_str());
    ntp.setTimeOffset(cfg.utc * 3600); ntp.begin();
    
    Serial.println("CMD:MODE,WORK"); delay(20);
    Serial.println(cfg.rot180 ? "CMD:ROT,1" : "CMD:ROT,0"); delay(20);
    Serial.println("NAME:0," + cfg.s1_nm); delay(20);
    Serial.println("NAME:1," + cfg.s2_nm); delay(20);
  }
}

String extractWX(const String& json, const String& sec, const String& key) {
  int s1 = json.indexOf(sec);
  if (s1 < 0) return "--";
  int k1 = json.indexOf(key, s1);
  if (k1 < 0) return "--";
  k1 += key.length();
  while (k1 < (int)json.length()) {
    char ch = json[k1];
    if (ch==':' || ch==' ' || ch=='\t' || ch=='\n' || ch=='\r' || ch=='[' || ch=='"') { k1++; continue; }
    break;
  }
  int e1 = k1;
  while (e1 < (int)json.length()) {
    char ch = json[e1];
    if (ch=='"' || ch==',' || ch==']' || ch=='}' || ch=='\n') break;
    e1++;
  }
  String v = json.substring(k1, e1); v.trim(); return v;
}

void sendWDAYS(int wdayIdx, int todayDay, int todayMonth) {
  const char* dN[] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};
  const char* mN[] = {"","JAN","FEB","MAR","APR","MAI","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
  String mStr = (todayMonth>=1 && todayMonth<=12) ? mN[todayMonth] : String(todayMonth);
  char dStr[8]; snprintf(dStr, sizeof(dStr), "%02d", todayDay);
  Serial.printf("WDAYS:%s,%s,%s\n", dN[wdayIdx], dStr, mStr.c_str());
  delay(20);
}

void fetchWeather() {
  String la = cfg.lat; la.replace(",", "."); la.trim();
  String lo = cfg.lon; lo.replace(",", "."); lo.trim();
  
  String url = "https://api.open-meteo.com/v1/forecast?latitude=" + la +
               "&longitude=" + lo +
               "&current=temperature_2m,relative_humidity_2m,is_day,weather_code,wind_speed_10m"
               "&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset,"
               "precipitation_probability_max,precipitation_sum,precipitation_hours"
               "&timezone=Europe%2FRiga&forecast_days=1";
               
  wifiClient.setInsecure(); 
  HTTPClient http; 
  http.begin(wifiClient, url); 
  http.addHeader("User-Agent", "Mozilla/5.0");
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.setTimeout(10000);
  
  if (http.GET() == HTTP_CODE_OK) {
    String body = http.getString();

    String temp   = extractWX(body, "\"current\":", "\"temperature_2m\":");
    String hum    = extractWX(body, "\"current\":", "\"relative_humidity_2m\":");
    String wind   = extractWX(body, "\"current\":", "\"wind_speed_10m\":");
    String wcode  = extractWX(body, "\"current\":", "\"weather_code\":");
    String isDay  = extractWX(body, "\"current\":", "\"is_day\":");
    String maxT   = extractWX(body, "\"daily\":",   "\"temperature_2m_max\":");
    String minT   = extractWX(body, "\"daily\":",   "\"temperature_2m_min\":");
    String pop    = extractWX(body, "\"daily\":",   "\"precipitation_probability_max\":");
    String precip = extractWX(body, "\"daily\":",   "\"precipitation_sum\":");
    String prech  = extractWX(body, "\"daily\":",   "\"precipitation_hours\":");
    String rise   = extractWX(body, "\"daily\":",   "\"sunrise\":");
    String sett   = extractWX(body, "\"daily\":",   "\"sunset\":");
    
    if (rise.length() >= 16) rise = rise.substring(11,16);
    if (sett.length() >= 16) sett = sett.substring(11,16);

    int code = wcode.toInt(); int dayFlag = isDay.toInt(); String desc = "Unknown";
    if      (code == 0)             desc = dayFlag ? "Sunny" : "Clear";
    else if (code <= 3)             desc = "Cloudy";
    else if (code==45||code==48)    desc = "Foggy";
    else if (code>=51&&code<=55)    desc = "Drizzle";
    else if (code>=61&&code<=65)    desc = "Rainy";
    else if (code>=71&&code<=77)    desc = "Snow";
    else if (code >= 95)            desc = "Storm";

    Serial.printf("W1:%s,%s,%s,%s,%s,%s\n",
      temp.c_str(), maxT.c_str(), minT.c_str(), pop.c_str(), precip.c_str(), wind.c_str()); delay(40);
    Serial.printf("W2:%s,%s,%s,%s,%s,%s\n",
      rise.c_str(), sett.c_str(), hum.c_str(), prech.c_str(), isDay.c_str(), desc.c_str()); delay(40);
  }
  http.end();
}

struct BdayEntry {
  String name;
  int    day;
  int    month;
  int    daysUntil; 
};

int daysUntilBday(int bDay, int bMonth, int todayDay, int todayMonth) {
  int bDayOfYear = 0, tDayOfYear = 0;
  int mDays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  for (int i=0; i<bMonth-1; i++) bDayOfYear += mDays[i]; bDayOfYear += bDay;
  for (int i=0; i<todayMonth-1; i++) tDayOfYear += mDays[i]; tDayOfYear += todayDay;
  int diff = bDayOfYear - tDayOfYear;
  if (diff < 0) diff += 365;
  return diff;
}

String removeLvDiacritics(String s) {
  s.replace("Ā", "Aa"); s.replace("ā", "aa");
  s.replace("Č", "C");  s.replace("č", "c");
  s.replace("Ē", "Ee"); s.replace("ē", "ee");
  s.replace("Ģ", "G");  s.replace("ģ", "g");
  s.replace("Ī", "Ii"); s.replace("ī", "ii");
  s.replace("Ķ", "K");  s.replace("ķ", "k");
  s.replace("Ļ", "L");  s.replace("ļ", "l");
  s.replace("Ņ", "N");  s.replace("ņ", "n");
  s.replace("Š", "S");  s.replace("š", "s");
  s.replace("Ū", "Uu"); s.replace("ū", "uu");
  s.replace("Ž", "Z");  s.replace("ž", "z");
  return s;
}

String fixUnicode(String s) {
  s.replace("\\u0100", "Aa"); s.replace("\\u0101", "aa");
  s.replace("\\u010C", "C");  s.replace("\\u010c", "c");
  s.replace("\\u010D", "c");  s.replace("\\u010d", "c");
  s.replace("\\u0112", "Ee"); s.replace("\\u0113", "ee");
  s.replace("\\u0122", "G");  s.replace("\\u0123", "g");
  s.replace("\\u012A", "Ii"); s.replace("\\u012a", "ii");
  s.replace("\\u012B", "ii"); s.replace("\\u012b", "ii");
  s.replace("\\u0136", "K");  s.replace("\\u0137", "k");
  s.replace("\\u013B", "L");  s.replace("\\u013b", "l");
  s.replace("\\u013C", "l");  s.replace("\\u013c", "l");
  s.replace("\\u0145", "N");  s.replace("\\u0146", "n");
  s.replace("\\u0160", "S");  s.replace("\\u0161", "s");
  s.replace("\\u016A", "Uu"); s.replace("\\u016a", "uu");
  s.replace("\\u016B", "uu"); s.replace("\\u016b", "uu");
  s.replace("\\u017D", "Z");  s.replace("\\u017d", "z");
  s.replace("\\u017E", "z");  s.replace("\\u017e", "z");
  return s;
}

void fetchBirthdays(int todayDay, int todayMonth) {
  if (todayDay == 0 || todayMonth == 0) return;

  BdayEntry entries[32]; int count = 0;
  String src = cfg.bdays + "\n";
  int pos = 0;
  while (pos < (int)src.length() && count < 32) {
    int nl = src.indexOf('\n', pos);
    String line = src.substring(pos, nl < 0 ? src.length() : nl);
    line.trim();
    int lp = line.lastIndexOf('(');
    int rp = line.lastIndexOf(')');
    if (lp >= 0 && rp > lp) {
      String name = line.substring(0, lp); name.trim();
      name = removeLvDiacritics(fixUnicode(name)); 
      if(name.length() > 10) name = name.substring(0, 10); 
      
      String date = line.substring(lp+1, rp); date.trim();
      int dot1 = date.indexOf('.');
      int dot2 = date.indexOf('.', dot1+1);
      if (dot1 >= 1 && dot2 > dot1) {
        entries[count].day   = date.substring(0, dot1).toInt();
        entries[count].month = date.substring(dot1+1, dot2).toInt();
        entries[count].name  = " " + name; 
        entries[count].daysUntil = daysUntilBday(entries[count].day, entries[count].month, todayDay, todayMonth);
        count++;
      }
    }
    if (nl < 0) break; pos = nl+1;
  }

  for (int i=0; i<count-1; i++) {
    for (int j=0; j<count-1-i; j++) {
      if (entries[j].daysUntil > entries[j+1].daysUntil) {
        BdayEntry tmp = entries[j]; 
        entries[j] = entries[j+1]; 
        entries[j+1] = tmp;
      }
    }
  }

  Serial.println("BDAY:BEGIN"); delay(30);
  int shown = min(count, 5); 
  for (int i=0; i<shown; i++) {
    char dateBuf[10];
    snprintf(dateBuf, sizeof(dateBuf), "%02d.%02d.", entries[i].day, entries[i].month);
    int todayFlag = (entries[i].daysUntil == 0) ? 1 : 0;
    
    Serial.printf("BDAY:EVT,%s,%s,%d\n", entries[i].name.c_str(), dateBuf, todayFlag);
    delay(30); 
  }
  Serial.println("BDAY:END"); delay(30);
}

void fetchLocalData(int todayDay, int todayMonth) {
  if (todayDay == 0 || todayMonth == 0) return;

  char searchStr[8];
  snprintf(searchStr, sizeof(searchStr), "%02d.%02d.", todayDay, todayMonth);

  String sday = "";
  String n1="", n2="", n3="";

  const char* p = DATASET_CSV;
  char line[128];
  while (pgm_read_byte(p) != 0) {
    int i = 0;
    char c;
    while ((c = pgm_read_byte(p)) != 0 && c != '\n' && i < 127) {
      line[i++] = c;
      p++;
    }
    line[i] = '\0';
    if (c == '\n') p++; 
    
    String lStr = String(line);
    if (lStr.startsWith(searchStr)) {
      int c1 = lStr.indexOf(',');
      int c2 = lStr.indexOf(',', c1+1);
      int c3 = lStr.indexOf(',', c2+1);
      int c4 = lStr.indexOf(',', c3+1);

      if (c1>=0 && c2>c1) sday = lStr.substring(c1+1, c2);
      if (c2>=0 && c3>c2) n1   = lStr.substring(c2+1, c3);
      if (c3>=0 && c4>c3) n2   = lStr.substring(c3+1, c4);
      if (c4>=0)          n3   = lStr.substring(c4+1);
      else if (c3>=0)     n3   = lStr.substring(c3+1);
      break; 
    }
  }

  auto cleanToken = [](String& t) {
    t.replace("\r", "");
    t.replace("\"", "");
    t.trim();
    t = removeLvDiacritics(t);
    return t;
  };

  cleanToken(sday); 
  cleanToken(n1); 
  cleanToken(n2); 
  cleanToken(n3);

  if(sday.length() > 0) {
    Serial.printf("SDAY:%s\n", sday.c_str());
    delay(30);
  } else {
    Serial.println("SDAY:");
    delay(30);
  }

  Serial.println("NDAY:BEGIN"); delay(30);
  int nIdx = 0;
  if (n1.length() > 0) { Serial.printf("NDAY:%d,%s\n", nIdx++, n1.c_str()); delay(30); }
  if (n2.length() > 0) { Serial.printf("NDAY:%d,%s\n", nIdx++, n2.c_str()); delay(30); }
  if (n3.length() > 0) { Serial.printf("NDAY:%d,%s\n", nIdx++, n3.c_str()); delay(30); }
  Serial.println("NDAY:END"); delay(30);
}

void fetchCalendar() {
  if (cfg.cal_api.length() < 10) return;
  wifiClient.setInsecure(); HTTPClient http;
  http.begin(wifiClient, cfg.cal_api);
  http.addHeader("User-Agent","Mozilla/5.0"); http.setTimeout(12000);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  if (http.GET() == HTTP_CODE_OK) {
    String body = http.getString();
    int pos = 0;
    while (pos < (int)body.length()) {
      int nl = body.indexOf('\n', pos);
      String line = (nl < 0) ? body.substring(pos) : body.substring(pos, nl);
      line.trim();
      if (line.startsWith("CAL:") || line == "CAL:BEGIN" || line == "CAL:END") {
        if (line.startsWith("CAL:EVT,")) {
          int c1 = line.indexOf(',');
          int c2 = line.indexOf(',', c1+1);
          int c3 = line.lastIndexOf(',');
          if (c1 > 0 && c2 > c1 && c3 > c2) {
             String timeStr = line.substring(c1+1, c2);
             String title = line.substring(c2+1, c3);
             String colorStr = line.substring(c3);
             if (title.length() > 24) {
                 title = title.substring(0, 21) + "...";
             }
             line = "CAL:EVT," + timeStr + "," + title + colorStr;
          }
        }
        Serial.println(line); delay(30);
      }
      if (nl < 0) break; pos = nl+1;
    }
  } else {
    Serial.println("CAL:BEGIN"); delay(30);
    Serial.println("CAL:END");  delay(30);
  }
  http.end();
}

bool passes(const String& type, const String& route, const String& filters) {
  String fStr = filters;
  fStr.trim();
  fStr.replace(" ", ""); 
  if (fStr == "") return true; 
  
  String f = "," + fStr + ",";
  
  String rStr = route;
  rStr.trim();
  rStr.replace(" ", "");

  String trStr = type + rStr;
  trStr.trim();
  trStr.replace(" ", "");
  
  return (f.indexOf("," + rStr + ",") >= 0) || (f.indexOf("," + trStr + ",") >= 0);
}

void fetchStop(int idx, const String& id, const String& filters, int delayMins, long nowSec) {
  if (id.length() == 0) return;
  struct DepBuf { char type[5]; char route[8]; long secs; };
  DepBuf buf[16]; int bufCount = 0;
  wifiClient.setInsecure(); HTTPClient http;

  String baseApi = cfg.timey_api;
  int eqPos = baseApi.lastIndexOf('=');
  if (eqPos > 0) {
    baseApi = baseApi.substring(0, eqPos + 1);
  }

  http.begin(wifiClient, baseApi + id);
  http.addHeader("User-Agent","Mozilla/5.0"); http.setTimeout(10000);
  if (http.GET() == HTTP_CODE_OK) {
    String body = http.getString(); int pos = 0;
    while (pos < (int)body.length() && bufCount < 16) {
      int nl = body.indexOf('\n', pos);
      String line = (nl<0) ? body.substring(pos) : body.substring(pos,nl);
      line.trim();
      if (line.length() && !line.startsWith("stop,")) {
        int c1=line.indexOf(','),c2=line.indexOf(',',c1+1),c3=line.indexOf(',',c2+1),c4=line.indexOf(',',c3+1);
        if (c4 > 0) {
          String type  = line.substring(0,c1);
          String route = line.substring(c1+1,c2);
          long secs    = line.substring(c3+1,c4).toInt();
          long left    = secs - nowSec;
          if (left < 0) left += 86400L;
          left -= delayMins * 60L;
          if (secs>0 && left>=0 && left<=3600L && passes(type, route, filters)) {
            type.toCharArray(buf[bufCount].type, 5);
            route.toCharArray(buf[bufCount].route, 8);
            buf[bufCount].secs = left; bufCount++;
          }
        }
      }
      if (nl < 0) break; pos = nl+1;
    }
  }
  http.end();
  Serial.print("CMD:BEGIN,"); Serial.println(idx); delay(30);
  for (int i=0; i<bufCount; i++) { Serial.printf("%s,%s,%ld\n", buf[i].type, buf[i].route, buf[i].secs); delay(30); }
  Serial.println("CMD:END"); delay(30);
}

void setup() {
  delay(500);
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  loadConfig();
  switchMode(isConfigMode);
}

void loop() {
  unsigned long now = millis();

  bool btn = digitalRead(BUTTON_PIN);
  if (btn == LOW) {
    if (buttonPressTime==0) buttonPressTime=now;
    else if ((now-buttonPressTime)>=400 && !buttonHandled) { switchMode(!isConfigMode); buttonHandled=true; }
  } else { buttonPressTime=0; buttonHandled=false; }

  if (isConfigMode) {
    dnsServer.processNextRequest();
    server.handleClient();
    delay(10);
    return;
  }

  static unsigned long offlineTimer = 0;
  if (offlineTimer == 0) offlineTimer = now;
  if (WiFi.status() != WL_CONNECTED) {
    if (now - offlineTimer > 15000) {
      offlineTimer = now;
      WiFi.disconnect(true); delay(50);
      WiFi.mode(WIFI_STA); WiFi.begin(cfg.ssid.c_str(), cfg.pass.c_str());
    }
    return;
  }
  offlineTimer = now;

  static unsigned long lastWx   = 0;
  static unsigned long lastBday = 0;
  static unsigned long lastData = 0;
  static unsigned long lastCal  = 0;
  static int lastMin = -1, lastSec = -1;

  ntp.update();
  unsigned long epoch = ntp.getEpochTime();
  if (epoch < 1000000000UL) return;

  time_t epochTime = epoch;
  struct tm *ptm = gmtime(&epochTime);
  int todayDay = ptm->tm_mday;
  int todayMonth = ptm->tm_mon + 1;
  int h = ptm->tm_hour;
  int m = ptm->tm_min;
  int s = ptm->tm_sec;
  int wday = ptm->tm_wday;

  if (m != lastMin) {
    Serial.printf("WTIME:%02d:%02d:%02d\n", h, m, s); delay(40);
    sendWDAYS(wday, todayDay, todayMonth);
    lastMin = m;
  }

  if (s != lastSec) {
    checkAlarms(h, m, s, wday);
    if (s % (60 / max(1, cfg.t_freq)) == 0) {
      long t = epoch % 86400L;
      fetchStop(0, cfg.s1_id, cfg.s1_flt, cfg.s1_del, t);
      fetchStop(1, cfg.s2_id, cfg.s2_flt, cfg.s2_del, t);
    }
    lastSec = s;
  }

  if      (lastWx==0   || (now-lastWx >= (unsigned long)cfg.w_ref*60000UL)) { lastWx=now; fetchWeather(); }
  else if (lastBday==0 || (now-lastBday >= 3600000UL))                      { lastBday=now; fetchBirthdays(todayDay, todayMonth); }
  else if (lastData==0 || (now-lastData >= 1800000UL))                      { lastData=now; fetchLocalData(todayDay, todayMonth); }
  else if (lastCal==0  || (now-lastCal >= 300000UL))                        { lastCal=now; fetchCalendar(); }

  delay(50);
}