# KOJAS – Koju Optimizācijas Jaunumu Apvienotā Sistēma

![Status](https://img.shields.io/badge/Status-Aktīvā_Izstrādē-brightgreen)
![Authors](https://img.shields.io/badge/Authors-Rolands_%26_Aleksis-red)

Projekta ietvaros tiek izstrādāta autonoma informācijas pasniegšanas stacija, kuras mērķis ir centralizēt un vizualizēt lietotājiem kritiski svarīgu informāciju vienuviet, mazinot nepieciešamību pēc viedtālruņa lietošanas rīta rutīnas laikā.

## Tehniskais raksturojums
Sistēmas pamatā ir pie sienas stiprināms mikrokontrolieris (ESP8266), kas aprīkots ar **krāsainu TFT LCD displeju**, nodrošinot pārskatāmu un dinamisku informācijas attēlojumu. 

Integrētais **Wi-Fi modulis** ļauj iekārtai darboties autonomi, iegūstot reāllaika datus no ārējām datubāzēm un API. Ierīce ir aprīkota ar **skaņas moduli (Buzzer)** paziņojumiem un modinātājiem, kā arī izmanto iekšējo atmiņu (*LittleFS*) lokālo konfigurāciju drošai saglabāšanai pat pēc strāvas zuduma.

## Sistēmas funkcionalitāte
Lietotājam reāllaikā uz ekrāna ir pieejama šāda informācija un iespējas:

* **Pulkstenis un datums:** Automātiska laika sinhronizācija, izmantojot NTP serverus.
* **Laikapstākļi:** Detalizēta aktuālā prognoze (*Open-Meteo API*) – temperatūra (tai skaitā dienas min/max), gaisa mitrums, vēja ātrums, nokrišņu daudzums un saullēkta/saulrieta laiks.
* **Sabiedriskais transports:** Reāllaika *Rīgas Satiksmes* atiešanas laiki līdz pat divām lietotāja izvēlētām pieturām. Integrēts maršrutu filtrs (piemēram, `bus13,tram11,trol25`), lai attēlotu tikai vajadzīgos transportlīdzekļus.
* **Google kalendāra integrācija:** Sinhronizācija ar lietotāja kalendāru, attēlojot gaidāmās lekcijas, stundas un citus pasākumus.
* **Dienas notikumi un svētki:** Iebūvēta Latvijas vārdadienu datubāze un lietotāja definējams dzimšanas dienu atgādinājumu saraksts, kas izceļ šodienas gaviļniekus.
* **Modinātāji un skaņas signāli:** Vairāki iestatāmi modinātāji ar dažādām melodijām, ko var pielāgot konkrētām nedēļas dienām (piemēram, atgādinājums iziet no mājas).

## Uzstādīšana un konfigurācija
Iekārta ir izstrādāta maksimāli ērtai lietošanai bez nepieciešamības pārrakstīt kodu:
1. **Pirmajā ieslēgšanas reizē** ierīce izveido savu Wi-Fi piekļuves punktu (Captive Portal), piemēram, `KOJAS-AR0001`.
2. Pievienojoties šim tīklam ar viedtālruni vai datoru, lietotājam atveras **vizuāls Web interfeiss**.
3. Caur pārlūkprogrammu ir iespējams ievadīt mājas Wi-Fi datus, koordinātas (laikapstākļiem), sabiedriskā transporta pieturu ID, dzimšanas dienas un modinātāju laikus.
4. Saglabājot iestatījumus, ierīce automātiski pārslēdzas darba režīmā un sāk attēlot datus.