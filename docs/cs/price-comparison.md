# Cenové srovnání

Představte si, že kupujete tričko. V obchodě máte tři možnosti: levné tričko za 50 Kč, které se po třech praních roztáhne. Kvalitní tričko za 500 Kč z poctivého materiálu, které vám vydrží roky. A značkové tričko za 1 500 Kč — stejný materiál jako to za 500, ale platíte za logo a krabičku.
Se systémy pro chytrou domácnost je to stejné.

Majordomus je jako to tričko za 500 Kč. Kvalitní komponenty, spolehlivá drátová komunikace, kompaktní návrh — ale bez příplatku za značku a uzavřený ekosystém. Platíte za reálnou hodnotu, ne za krabičku a logo.

Pojďme se podívat na příklad nákupu HW pro modelový dům s 5 zónami od různých výrobců.

## Modelový dům / byt s 5 zónami

Scénář: **5 místností**, v každé pokrýváme vnitřní klima, osvětlení, žaluzie a detekci přítomnosti, integrace s rekuperací, apod.

### Co potřebujeme v každé zóně

**Senzory (měření):** teplota + vlhkost, CO₂, intenzita osvětlení, pohybový senzor, 4 tlačítka na zdi, vstupy na záplavový senzor a okenní kontakty

**Akční členy (ovládání):** spínání 230VAC pro topné ventily, osvětlení, spínání žaluzií, ovládání výstupu 0-10V pro stmívání osvětlení

**Rozhraní pro integraci** Digitální rozhraní ModBus pro integraci rekuperace, klimatizace apod. zařízení třetích stran.

---

### Cena s využitím HW Loxone — 5 zón

| Komponenta | Ks | Cena/ks s DPH (Kč) | Celkem (Kč) |
|---|---|---|---|
| Centrální jednotka (miniserver) | 1 | 19 842 | 19 842 |
| Teplotní + vlhkostní senzor + CO₂ + tlačítka | 5 | 7 350 | 36 750 |
| PIR / přítomnostní senzor + intenzita osvětlení | 5 | 3 321 | 16 605|
| Vstupy níkonapěťové (okenní kontakty, záplavový senzor, tlačítka) | 5 | 2 478 | 12 390 |
| Výstupy 230 VAC (akční člen) | 2 | 10 222 | 20 444 |
| Ovládání AO (0-10V) | 2 | 8 202 | 16 404 |
| Rozhraní ModBus | 2 | 7577 | 15 154 |
| **Celkem** | | | **137 589** |

Cena vychází z oficiálních stránek shop.loxone.com, 2Q/26. V ceně není zahrnut projekt, instalace, SW apod. Cena je pouze za klíčové prvky HW.

---

### Levná asijská řešení ( ZigBee + logika na cloudu) — 5 zón

| Komponenta | Ks | Cena/ks (Kč) | Celkem (Kč) |
|---|---|---|---|
| ZigBee brána (Gateway) | 1 | 500 | 500 |
| Senzor teploty + vlhkosti (ZigBee) | 5 | 300 | 1 500 |
| Senzor CO₂ (ZigBee, většinou pouze eCO₂) | 5 | 1 200 | 6 000 |
| PIR pohybový senzor (ZigBee) | 5 | 250 | 1 250 |
| Senzor osvětlení (ZigBee) | 5 | 350 | 1 750 |
| Tlačítkový ovladač / scény (ZigBee) | 5 | 400 | 2 000 |
| Relé modul 230 V | 10 | 350 | 3 500 |
| **Celkem** | | | **16 500** |

Ceny z AliExpress, 2Q/26. Na první pohled nejlevnější varianta — ale je třeba počítat s tím, co v ceně není vidět:

- **5 krabiček na zeď v každé místnosti** — senzor T+RH, CO₂, PIR, lux a tlačítka jsou samostatná zařízení.
- **Bezdrátové spojení** — závislost na Wi-Fi/ZigBee, výpadky signálu, rušení.
- **Baterie** — většina senzorů běží na CR2450/CR2032, výměna každých 6–12 měsíců × 25 senzorů = nekonečný kolotoč.
- **eCO₂ ≠ CO₂** — levné senzory neměří skutečný CO₂, pouze odhadují z VOC. Přesnost je nesrovnatelná.
- **Žádná záruka dlouhodobé dostupnosti** — výrobce z AliExpressu může kdykoliv zmizet.
- **0–10 V výstupy** — v ZigBee ekosystému prakticky neexistují, nelze ovládat stmívání profesionálních svítidel.
- **Cloudová závislost** — většina Tuya zařízení vyžaduje cloudové připojení. Server vypnou = nefunguje.

---

### Cena s komponenty Majordomus

| Komponenta | Ks | Cena/ks (Kč) | Celkem (Kč) |
|---|---|---|---|
| Mini PC / Raspberry Pi + zdroj | 1 | 4 500 | 4 500 |
| RoomSensor (teplota, vlhkost, CO₂, VOC, světlo, PIR, 4 tlačítka — vše v 1 ks) | 5 | 5 900| 29 500|
| BoxIO (spínání 230 VAC výstupů, AO) | 4 | 3 500 | 14 000 |
| RS485 / ModBus převodník USB | 3 | 850 | 2 550 |
| **Celkem** | | | **50 550**|
