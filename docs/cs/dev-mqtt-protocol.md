# MQTT protokol

Referenční popis MQTT rozhraní, které poskytuje Majordomus Control. Hodí se všem, kdo chtějí systém integrovat mimo Home Assistant — do Node-RED, openHAB, vlastních skriptů nebo čehokoliv dalšího, co umí MQTT.

Majordomus Control funguje jako brána: čte data z jednotek po RS-485 a publikuje je do MQTT témat. Zároveň odebírá příkazová témata a přijaté příkazy předává zpět jednotkám.

```
[RS-485 jednotky] <--serial--> [Majordomus Control] <--MQTT--> [Broker] <--> [HA / Node-RED / klient]
```

Anglický originál této reference: [MQTT_PROTOCOL.md](https://github.com/jirihusak/majordomus/blob/main/SW/MajordomusControl/MQTT_PROTOCOL.md)

---

## Struktura témat

Každé téma má tvar:

```
<prefix>/<zařízení>/<kategorie>/<klíč>
```

| Segment | Popis | Příklad |
|---|---|---|
| `<prefix>` | Nastavený MQTT prefix včetně koncového `/` | `majordomus/` |
| `<zařízení>` | Název zařízení dle konfigurace | `living_room` |
| `<kategorie>` | `tele`, `evt`, `state` nebo `cmd` | `tele` |
| `<klíč>` | Konkrétní vlastnost | `temperature0` |

**Příklad:** `majordomus/living_room/tele/temperature0`

!!! warning "Prefix musí být jedna úroveň"
    Brána směruje příkazy podle pozice v tématu — `cmd` musí být **třetí** úroveň. Prefix proto zadávejte jako jediný segment (`majordomus/`), ne víceúrovňový (`dum/prizemi/`).

### Kategorie témat

| Kategorie | Kdy se publikuje | Retained | Účel |
|---|---|---|---|
| `tele` | Při **změně** naměřené hodnoty | **Ano** | Naměřené hodnoty senzorů |
| `evt` | Při každé události (i opakované) | **Ne** | Události: stisky tlačítek, pohyb |
| `state` | Při změně | **Ano** | Metadata zařízení a stavy výstupů |
| `cmd` | Publikuje klient (vy) | — | Příkazy pro zařízení |

!!! note "Publikuje se jen změna"
    Retained témata (`tele`, `state`) se neposílají v každém cyklu vyčítání, ale až když se hodnota liší od naposledy odeslané. Aktuální hodnotu proto vždy získáte z retained zprávy hned po přihlášení k odběru. Naopak `evt` témata se posílají při každé události, i když má payload stejnou hodnotu.

Všechny zprávy se posílají s **QoS 0**. Payload je vždy UTF-8 řetězec — číslo (`22.5`, `420`), řetězec, nebo JSON pole. Jedinou výjimkou je `tele/irImagePng` (binární PNG).

---

## Odesílání příkazů

Příkaz zařízení pošlete publikováním do tématu:

```
<prefix>/<zařízení>/cmd/<klíč_příkazu>
```

**Příklad — zapnutí digitálního výstupu 0 na zařízení `living_room`:**

```
Téma:    majordomus/living_room/cmd/do0
Payload: 1
```

Brána odebírá `<prefix>/#` a zprávu zpracuje, jen když je třetí úroveň tématu `cmd`, druhá odpovídá názvu existujícího zařízení a čtvrtá je známý příkazový klíč. Ostatní zprávy pod prefixem (včetně vlastních `tele`/`evt`/`state`) ignoruje.

### Zpětné hlášení příkazů do `state/`

Každý přijatý příkaz brána okamžitě publikuje zpět do odpovídajícího retained tématu v kategorii `state/`. Právě tato témata používá Home Assistant jako `state_topic` pro spínače a číselné vstupy.

| Příkaz | Zpětné téma |
|---|---|
| `cmd/do0` … `cmd/do9` | `state/output0` … `state/output9` |
| `cmd/dac0`, `cmd/dac1` | `state/dac0`, `state/dac1` |
| `cmd/setCnt0` … `cmd/setCnt7` | `state/setCounter0` … `state/setCounter7` |
| `cmd/reqT` | `state/requestedTemperature` |
| `cmd/reqL` | `state/requestedLight` |
| `cmd/light` | `state/light` |
| `cmd/beep` | `state/beep` (bez retain) |
| `cmd/ro` | `state/ro` |

!!! note "Jde o potvrzení příjmu, ne o potvrzení z jednotky"
    Hodnota se do `state/` publikuje ve chvíli, kdy brána příkaz přijme. Do jednotky se přenese až v následujícím cyklu po RS-485. U `requestedTemperature` a `requestedLight` publikuje brána do stejného tématu i hodnotu, kterou si uživatel nastaví přímo na jednotce.

---

## Dostupnost zařízení

Každé zařízení publikuje svůj stav do retained tématu:

```
<prefix>/<zařízení>/state/online
```

| Payload | Význam |
|---|---|
| `online` | Zařízení odpovídá na dotazy po RS-485 |
| `offline` | Bez odpovědi déle než 3 sekundy |

Publikuje se pouze při změně stavu. Toto téma se používá jako `availability_topic` v Home Assistant discovery.

---

## Konvence hodnot

**NaN** — firmware používá hodnotu `INT16_MIN` (`-32768`) pro senzor, který není dostupný (nepřipojen, chyba čtení). Brána ji před publikováním převádí na řetězec `"NaN"`. Payload `"NaN"` vždy interpretujte jako nedostupnou hodnotu.

**Škálované hodnoty** — některá zařízení posílají po sběrnici celočíselné hodnoty, které je nutné dělit koeficientem (např. TempOutBoard posílá `225` = 22,5 °C). Převod provádí brána — přes MQTT vždy dostanete finální hodnotu jako float.

**Pocitová teplota** — pokud jsou platné `temperature0` i `humidity`, brána automaticky počítá a publikuje pocitovou teplotu (Steadmanův vzorec) do tématu `tele/apparentTemperature`:

```
e  = (rh / 100) × 6.105 × exp(17.27 × T / (237.7 + T))
AT = T + 0.33 × e − 4.00
```

**Stavová témata** (`version`, `power`, `powerOut`) se aktualizují ze status dotazu, který brána posílá jednotce jednou za **30 sekund**. Ostatní data se čtou v každém cyklu.

---

## Události tlačítek

Tlačítka a kapacitní plošky nehlásí stav (stisknuto/uvolněno), ale **rozpoznané gesto**. Firmware jednotky rozlišuje krátký stisk, dvojklik, trojklik a dlouhé podržení a brána každé gesto publikuje jako jednorázový puls do samostatného tématu.

| Téma (`evt/`) | Gesto |
|---|---|
| `button<i>` | Krátký stisk |
| `button<i>Double` | Dvojklik |
| `button<i>Triple` | Trojklik |
| `button<i>Long` | Dlouhé podržení |

- Payload je vždy `1`, retain **ne**. Žádná zpráva s `0` se neposílá — téma je čistě událostní.
- Rozsah `<i>`: RoomIO a BoxIO `0`–`7`, RoomSensor `0`–`3`.
- Pokud potřebujete v automatizaci setrvalý stav sepnutého vstupu (a ne gesto), použijte `tele/input<i>`.

**Příklad:** dvojklik na tlačítku 2 v obýváku → `majordomus/living_room/evt/button2Double` s payloadem `1`.

---

## Mapy témat podle typu zařízení

### RoomIO

Univerzální I/O jednotka s teploměry, digitálními vstupy/výstupy, analogovými vstupy a čítači impulsů.

**Publikuje — `tele/` (retained):**

| Klíč | Jednotka | Typ | Poznámka |
|---|---|---|---|
| `temperature0`, `temperature1` | °C | float | Teploměry; `NaN` pokud nedostupný |
| `temperature2` | °C | float | Volitelný, jen pokud jednotka hodnotu hlásí |
| `humidity` | % | float | Volitelná relativní vlhkost |
| `voc` | index | float | Volitelná kvalita vzduchu VOC |
| `co2` | ppm | float | Volitelná koncentrace CO₂ |
| `illuminance` | lux | float | Volitelné osvětlení |
| `distance` | — | float | Volitelný snímač vzdálenosti |
| `analog0`, `analog1` | V | float | Analogové vstupy |
| `counter0` – `counter7` | impulsy | int | Čítače impulsů |
| `input0` – `input7` | — | `0`/`1` | Stavy digitálních vstupů |
| `apparentTemperature` | °C | float | Vypočtená pocitová teplota |

**Publikuje — `evt/` (bez retain):** `button0` – `button7` a jejich varianty `…Double`, `…Triple`, `…Long` — viz [Události tlačítek](#udalosti-tlacitek).

**Publikuje — `state/` (retained):** `version` (verze FW), `power` (napájecí napětí, V), `powerOut` (napětí za pojistkou výstupů, V), `online`, plus zpětná hlášení `output0`–`output7`, `dac0`, `dac1`, `setCounter0`–`setCounter7`.

**Příkazy — `cmd/`:**

| Klíč | Payload | Účinek |
|---|---|---|
| `do0` – `do7` | `0` / `1` | Nastavení digitálního výstupu |
| `dac0`, `dac1` | `0`–`10` (float) | Napětí analogového výstupu ve voltech |
| `setCnt0` – `setCnt7` | celé číslo | Přednastavení hodnoty čítače |
| `reboot` | cokoliv | Restart zařízení |

---

### BoxIO

Jednotka do rozvaděče — 8 digitálních vstupů, 6 silových výstupů, 2 analogové vstupy a výstupy. Sada témat je stejná jako u RoomIO, liší se počtem výstupů.

**Publikuje — `tele/` (retained):** `temperature0`, `temperature1`, volitelně `temperature2`, `humidity`, `voc`, `co2`, `illuminance`, `distance`, dále `analog0`, `analog1`, `counter0`–`counter7`, `input0`–`input7`, `apparentTemperature`.

**Publikuje — `evt/` (bez retain):** `button0` – `button7` včetně variant `…Double`, `…Triple`, `…Long`.

**Publikuje — `state/` (retained):** `version`, `power`, `powerOut`, `online`, `output0`–`output5`, `dac0`, `dac1`, `setCounter0`–`setCounter7`.

**Příkazy — `cmd/`:**

| Klíč | Payload | Účinek |
|---|---|---|
| `do0` – `do5` | `0` / `1` | Nastavení digitálního výstupu |
| `dac0`, `dac1` | `0`–`10` (float) | Napětí analogového výstupu ve voltech |
| `setCnt0` – `setCnt7` | celé číslo | Přednastavení hodnoty čítače |
| `reboot` | cokoliv | Restart zařízení |

---

### RoomSensor

Pokojová jednotka se senzory prostředí, detekcí pohybu, kapacitními tlačítky a výstupy.

**Publikuje — `tele/` (retained):**

| Klíč | Jednotka | Typ | Poznámka |
|---|---|---|---|
| `temperature0` – `temperature3` | °C | float | Až 4 teploměry |
| `humidity` | % | float | Relativní vlhkost |
| `voc` | index | int | VOC index (SGP40/41) |
| `nox` | index | int | NOx index (SGP41) |
| `co2` | ppm | int | Koncentrace CO₂ |
| `illuminance` | lux | int | Osvětlení (VEML7700) |
| `noise` | dB | int | Hladina hluku |
| `analog0`, `analog1` | V | float | Analogové vstupy |
| `input0` – `input3` | — | `0`/`1` | Stavy digitálních vstupů |
| `apparentTemperature` | °C | float | Vypočtená pocitová teplota |

**Publikuje — `evt/`:**

| Klíč | Typ | Retain | Kdy |
|---|---|---|---|
| `motion` | `0`/`1` | Ne | Změna stavu PIR čidla |
| `lastMotion` | `yyyy-MM-dd HH:mm:ss` | **Ano** | Čas poslední detekce pohybu |
| `button0` – `button3` | `1` | Ne | Krátký stisk kapacitního tlačítka |
| `button0Double` – `button3Long` | `1` | Ne | Dvojklik / trojklik / dlouhé podržení |

**Publikuje — `state/` (retained):** `version`, `power`, `powerOut`, `online`, `requestedTemperature` (žádaná teplota), `requestedLight` (žádaná úroveň přísvitu), `light`, `output0`–`output3`, `dac0`, `dac1`.

**Příkazy — `cmd/`:**

| Klíč | Payload | Účinek |
|---|---|---|
| `do0` – `do3` | `0` / `1` | Nastavení digitálního výstupu |
| `dac0`, `dac1` | `0`–`10` (float) | Napětí analogového výstupu ve voltech |
| `beep` | celé číslo | Pípnutí piezo pípáku (délka / kód vzoru) |
| `reqT` | float `10`–`35` | Nastavení žádané teploty (krok 0,5 °C) |
| `reqL` | `0`–`100` | Žádaná úroveň přísvitu (%) |
| `light` | `0` / `1` | Zapnutí/vypnutí LED přísvitu |
| `reboot` | cokoliv | Restart zařízení |

---

### TempOutBoard

Deska pro venkovní měření s až 8 teploměry DS18B20 a digitálními výstupy.

**Publikuje — `tele/` (retained):** `temperature0` – `temperature7` (°C, float; surová hodnota z jednotky dělená 10).

**Publikuje — `state/` (retained):** `version`, `power`, `powerOut`, `online`, `output0`–`output9`, `ro`.

**Příkazy — `cmd/`:** `do0` – `do9` (`0`/`1`), `ro` (`0`–`255`, PWM výstup).

---

### RoomIR

Modul termální infrakamery (MLX90642, 32×24 = 768 pixelů). Kromě surového snímku publikuje i výsledky vyhodnocení obrazu, které provádí brána.

**Publikuje — `tele/` (retained):**

| Klíč | Typ | Popis |
|---|---|---|
| `irImage` | JSON pole | 768 hodnot teplot v °C (32 sloupců × 24 řádků, po řádcích zleva shora) |
| `irImagePng` | binární PNG | Teplotní mapa 320×240 px (paleta iron-bow), pro MQTT kameru v HA |
| `maxTemperature` | float | Nejvyšší teplota ve snímku (°C, 1 desetinné místo) |
| `minTemperature` | float | Nejnižší teplota ve snímku (°C, 1 desetinné místo) |
| `fireAlarm` | `0`/`1` | `1`, pokud `maxTemperature` překročí nakonfigurovaný práh požáru |
| `personCount` | int | Počet detekovaných osob |
| `zone0Presence` – `zone3Presence` | `0`/`1` | Přítomnost osoby v definované zóně (jen pro povolené zóny) |

```json
[25.1, 25.3, 24.8, 26.0, ...]
```

**Publikuje — `state/` (retained):** `version`, `online`.

**Příkazy — `cmd/`:** `reboot`.

!!! info "Jak vzniká snímek"
    Jednotka posílá po RS-485 jeden snímek v 6 blocích po 128 pixelech, base64 kódovaných, spolu s hodnotami `tmin`/`tmax` (v setinách °C). Pixely jsou normalizované do rozsahu 0–255, brána je převádí zpět na teplotu podle `T = tmin + px × (tmax − tmin) / 255`. Snímek se publikuje, až když dorazí všech 6 bloků se stejným pořadovým číslem (`seq`). PNG se publikuje nejvýše **jednou za 2 sekundy**, aby nezahltilo broker.

**Vyhodnocení obrazu** se konfiguruje pro každé zařízení zvlášť (sekce `<irConfig>` v `config.xml` nebo přes webové rozhraní):

| Parametr | Výchozí | Význam |
|---|---|---|
| `fireThreshold` | 100 °C | Práh pro `fireAlarm` |
| `personMinTemp` | 28 °C | Spodní pojistka teplotního pásma osoby |
| `personMaxTemp` | 38 °C | Horní mez teplotního pásma osoby |
| `minClusterSize` | 8 px | Menší shluky se považují za šum |
| `zones` | — | Až 4 obdélníkové zóny (`x`, `y`, `w`, `h`, `name`, `enabled`) |

Práh pro detekci osoby se odvozuje dynamicky od pozadí scény (medián snímku + 2 °C), `personMinTemp` slouží jen jako spodní pojistka — oblečený člověk bývá jen 2–4 °C nad pozadím. Osoby slepené v jednom shluku se rozlišují podle počtu teplotních vrcholů.

---

## Home Assistant discovery

Po připojení k brokeru publikuje brána automaticky discovery zprávy pro všechna nakonfigurovaná zařízení — Home Assistant si díky nim vytvoří entity bez ručního YAML. Zapíná se v konfiguraci (`<HomeAssistant enable="true" topic="homeassistant/">`).

**Formát tématu:** `<ha_prefix>/<typ_entity>/<unique_id>/config` — např. `homeassistant/sensor/living_room_t0/config`.

**Typy vytvářených entit:**

| Typ entity | Použití |
|---|---|
| `sensor` | Teploty, vlhkost, VOC/NOx/CO₂, osvětlení, hluk, analogové vstupy, čítače, napájecí napětí (jako diagnostika), `personCount`, `min`/`maxTemperature` |
| `binary_sensor` | Digitální vstupy, pohyb, `fireAlarm`, přítomnost v zónách RoomIR |
| `switch` | Digitální výstupy (`output<i>`), přísvit RoomSensoru |
| `number` | DAC výstupy (0–10 V, krok 0,1), `requestedTemperature` (10–35 °C, krok 0,5), `requestedLight` (0–100 %) |
| `camera` | Teplotní mapa RoomIR (`tele/irImagePng`) |
| `device_automation` | Události tlačítek jako device triggers |

**Payload (JSON, retained):**

```json
{
  "name": "temperature0",
  "unique_id": "living_room_t0",
  "state_topic": "majordomus/living_room/tele/temperature0",
  "unit_of_measurement": "°C",
  "device_class": "temperature",
  "state_class": "measurement",
  "availability_topic": "majordomus/living_room/state/online",
  "payload_available": "online",
  "payload_not_available": "offline",
  "device": {
    "identifiers": ["living_room"],
    "manufacturer": "Majordomus",
    "model": "RoomSensor",
    "name": "living_room"
  }
}
```

### Tlačítka jako device triggers

Gesta tlačítek se nepublikují jako entity, ale jako **device triggers** — v Home Assistantu je najdete přímo v automatizacích pod zařízením ("Když … stisknuto tlačítko 1").

```json
{
  "automation_type": "trigger",
  "type": "button_double_press",
  "subtype": "button_3",
  "topic": "majordomus/living_room/evt/button2Double",
  "payload": "1",
  "device": { "identifiers": ["living_room"], "...": "..." }
}
```

Typy: `button_short_press`, `button_double_press`, `button_triple_press`, `button_long_press`. `subtype` je číslované od 1 (`button_1` odpovídá tématu `button0`).

### Znovupublikování a úklid

- Discovery zprávy se znovu publikují při každém navázání i obnovení spojení s brokerem (řeší restart brokeru).
- Zóny RoomIR, které už nejsou povolené, brána maže publikováním **prázdné retained zprávy** do jejich discovery tématu — entita tak z Home Assistantu zmizí.

---

## Připojení k brokeru

| Parametr | Hodnota |
|---|---|
| Verze MQTT | 3.1.1 |
| URL brokeru | `tcp://host:1883` nebo `ssl://host:8883` |
| QoS | 0 (všechny zprávy) |
| Timeout připojení | 30 s |
| Keep-alive | 5 s |
| Automatický reconnect | Ano |
| Opakování prvního připojení | Každých 30 s, dokud se nepodaří |
| Odběr brány | `<prefix>/#` (obnoví se po každém reconnectu) |
| TLS | Systémový Java TrustStore, nebo vlastní PEM CA certifikát |

---

## Rychlý tahák

```
ČTENÍ hodnoty senzoru:
  Subscribe: <prefix>/<zařízení>/tele/<klíč>
  Subscribe: <prefix>/<zařízení>/state/<klíč>

POSLECH událostí:
  Subscribe: <prefix>/<zařízení>/evt/<klíč>

PŘÍKAZ:
  Publish:   <prefix>/<zařízení>/cmd/<klíč>   payload: <hodnota>

STAV VÝSTUPU (potvrzení příkazu):
  Subscribe: <prefix>/<zařízení>/state/output<i>

DOSTUPNOST:
  Subscribe: <prefix>/<zařízení>/state/online   → "online" | "offline"

VŠE NAJEDNOU:
  Subscribe: <prefix>/#
```

!!! tip "Rychlý test z příkazové řádky"
    ```bash
    # sledování všech dat
    mosquitto_sub -h localhost -t "majordomus/#" -v

    # zapnutí výstupu do0 na zařízení living_room
    mosquitto_pub -h localhost -t "majordomus/living_room/cmd/do0" -m "1"

    # reakce na dvojklik tlačítka 2
    mosquitto_sub -h localhost -t "majordomus/living_room/evt/button2Double"
    ```
