# KOJAS – Koju Optimizācijas Jaunumu Apvienotā Sistēma

![Status](https://img.shields.io/badge/Status-Prototips-yellow)
![Authors](https://img.shields.io/badge/Authors-Rolands_%26_Aleksis-red)

Projekta ietvaros tiek izstrādāta autonoma informācijas pasniegšanas stacija, kuras mērķis ir centralizēt un vizualizēt lietotājiem kritiski svarīgu informāciju vienuviet, mazinot nepieciešamību pēc viedtālruņa lietošanas.

## Tehniskais raksturojums
Sistēmas pamatā ir pie sienas stiprināms mikrokontrolieris ar energoefektīvu **4.2 collu E-ink displeju**, kas nodrošina salasāmību jebkurā apgaismojumā un minimalizē enerģijas patēriņu. Integrētais **Wi-Fi modulis** ļauj iegūt datus no ārējām datubāzēm un API.

Pateicoties iebūvētajiem sensoriem, ierīce kalpo arī kā lokāla **meteoroloģiskā stacija**, analizējot telpas mikroklimatu un vizualizējot to ar intuitīviem logrīkiem.

## Sistēmas funkcionalitāte
Lietotājam reāllaikā uz ekrāna ir pieejama šāda informācija:

* **Laiks un kalendārs:** Datums, precīzs laiks, vārda dienas (API/datubāze) un personalizēti dzimšanas dienu atgādinājumi.
* **Transports:** Aktuālais Rīgas Satiksmes kustības saraksts konkrētā pieturā (API).
* **Punktualitāte:** Laika atskaite līdz lekciju vai stundu sākumam, lai veicinātu laicīgu iziešanu no kopmītnēm.
* **Laikapstākļi:** Aktuālās prognozes (API) un lokālie sensora dati (telpas temperatūra un mitrums).
