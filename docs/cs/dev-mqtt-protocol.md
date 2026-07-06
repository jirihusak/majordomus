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

### Kategorie témat

| Kategorie | Kdy se publikuje | Retained | Účel |
|---|---|---|---|
| `tele` | Periodicky (každý cyklus vyčítání RS-485) | **Ano** | Naměřené hodnoty senzorů |
| `evt` | Pouze při změně | **Ne** | Události: stisk tlačítka, pohyb |
| `state` | Při změně | **Ano** | Metadata zařízení a stavy výstupů |
| `cmd` | Publikuje klient (vy) | — | Příkazy pro zařízení |

Všechny zprávy se posílají s **QoS 0**. Payload je vždy UTF-8 řetězec — číslo (`22.5`, `420`), řetězec, nebo JSON pole.

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

Toto téma se používá jako `availability_topic` v Home Assistant discovery.

---

## Konvence hodnot

**NaN** — firmware používá hodnotu `INT16_MIN` (`-32768`) pro senzor, který není dostupný (nepřipojen, chyba čtení). Brána ji před publikováním převádí na řetězec `"NaN"`. Payload `"NaN"` vždy interpretujte jako nedostupnou hodnotu.

**Škálované hodnoty** — některá zařízení posílají po sběrnici celočíselné hodnoty, které je nutné dělit koeficientem. Převod provádí brána — přes MQTT vždy dostanete finální hodnotu jako float.

**Pocitová teplota** — pokud jsou platné `temperature0` i `humidity`, brána automaticky počítá a publikuje pocitovou teplotu (Steadmanův vzorec) do tématu `tele/apparentTemperature`.

---

## Mapy témat podle typu zařízení

### RoomIO

Univerzální I/O jednotka s teploměry, digitálními vstupy/výstupy, analogovými vstupy a čítači impulsů.

**Publikuje — `tele/` (retained):**

| Klíč | Jednotka | Typ | Poznámka |
|---|---|---|---|
| `temperature0` – `temperature2` | °C | float | Teploměry; `NaN` pokud nedostupný |
| `humidity` | % | float | Relativní vlhkost |
| `voc` | index | int | Kvalita vzduchu VOC |
| `co2` | ppm | int | Koncentrace CO₂ |
| `illuminance` | lux | int | Osvětlení |
| `distance` | — | float | Snímač vzdálenosti |
| `analog0`, `analog1` | V | float | Analogové vstupy |
| `counter0` – `counter7` | — | int | Čítače impulsů |
| `input0` – `input7` | — | `0`/`1` | Stavy digitálních vstupů |
| `apparentTemperature` | °C | float | Vypočtená pocitová teplota |

**Publikuje — `evt/` (bez retain):** `button0` – `button7` (`0`/`1`) při změně stavu tlačítka.

**Publikuje — `state/` (retained):** `version` (verze FW), `power` (napájecí napětí, V), `powerOut` (napětí za pojistkou výstupů, V), `online`.

**Příkazy — `cmd/`:**

| Klíč | Payload | Účinek |
|---|---|---|
| `do0` – `do7` | `0` / `1` | Nastavení digitálního výstupu |
| `dac0`, `dac1` | `0`–`255` | Úroveň analogového výstupu (DAC/PWM) |
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
| `voc` | index | int | VOC index (SGP41) |
| `nox` | index | int | NOx index (SGP41) |
| `co2` | ppm | int | Koncentrace CO₂ |
| `illuminance` | lux | int | Osvětlení (VEML7700) |
| `noise` | dB | int | Hladina hluku |
| `analog0`, `analog1` | V | float | Analogové vstupy |
| `input0` – `input3` | — | `0`/`1` | Stavy digitálních vstupů |
| `apparentTemperature` | °C | float | Vypočtená pocitová teplota |

**Publikuje — `evt/` (bez retain):**

| Klíč | Typ | Kdy |
|---|---|---|
| `motion` | `0`/`1` | Změna stavu PIR čidla |
| `lastMotion` | ISO datetime | Čas poslední detekce pohybu |
| `button0` – `button3` | `0`/`1` | Stisk kapacitního tlačítka |

**Publikuje — `state/` (retained):** `version`, `power`, `powerOut`, `requestedTemperature` (aktuální žádaná teplota), `online`.

**Příkazy — `cmd/`:**

| Klíč | Payload | Účinek |
|---|---|---|
| `do0` – `do3` | `0` / `1` | Nastavení digitálního výstupu |
| `dac0`, `dac1` | `0`–`255` | Úroveň analogového výstupu |
| `beep` | celé číslo | Pípnutí piezo pípáku |
| `reqT` | float `10`–`35` | Nastavení žádané teploty |
| `light` | `0`–`100` | Úroveň LED přísvitu (%) |
| `reboot` | cokoliv | Restart zařízení |

---

### TempOutBoard

Deska pro venkovní měření s až 8 teploměry DS18B20 a digitálními výstupy.

**Publikuje — `tele/` (retained):** `temperature0` – `temperature7` (°C, float).

**Publikuje — `state/` (retained):** `version`, `power`, `powerOut`, `online`.

**Příkazy — `cmd/`:** `do0` – `do9` (`0`/`1`), `ro` (`0`–`255`, PWM výstup).

---

### RoomIR

Modul termální infrakamery (MLX90640, 32×24 pixelů).

**Publikuje — `tele/` (retained):** `irImage` — JSON pole 768 hodnot (32 sloupců × 24 řádků, po řádcích zleva shora), teploty v °C:

```json
[25.1, 25.3, 24.8, 26.0, ...]
```

**Publikuje — `state/` (retained):** `version`, `resolution` (škálovací faktor), `offset`, `online`.

---

## Home Assistant discovery

Po připojení k brokeru publikuje brána automaticky discovery zprávy pro všechna nakonfigurovaná zařízení — Home Assistant si díky nim vytvoří entity bez ručního YAML.

**Formát tématu:** `<ha_prefix>/<typ_entity>/<unique_id>/config` — např. `homeassistant/sensor/living_room_t0/config`.

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

Discovery zprávy se znovu publikují při každém obnovení spojení s brokerem (řeší restart brokeru).

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
    ```
