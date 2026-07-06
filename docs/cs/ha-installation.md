# Instalace Home Assistant

Home Assistant lze provozovat dvěma základními způsoby a pro oba existuje připravená instalace Majordomus Control:

| Varianta | Pro koho | Majordomus Control |
|---|---|---|
| **Home Assistant OS (HAOS)** | Doporučeno pro většinu uživatelů | Doplněk (add-on) přímo v Home Assistantu |
| **Docker** | Pro pokročilé, kteří už Docker používají | Samostatný kontejner přes Docker Compose |

---

## Varianta A: Home Assistant OS + doplněk Majordomus

Home Assistant OS je kompletní operační systém — nainstalujete ho na Raspberry Pi (nebo jiný mini PC) a o nic dalšího se nestaráte. Aktualizace, zálohy i instalace doplňků probíhají přímo z webového rozhraní.

### 1. Instalace Home Assistant OS

1. Stáhněte si [Raspberry Pi Imager](https://www.raspberrypi.com/software/).
2. Zvolte **Choose OS → Other specific-purpose OS → Home assistants and home automation → Home Assistant**.
3. Zapište systém na SD kartu (doporučujeme min. 32 GB) a vložte ji do Raspberry Pi.
4. Po několika minutách bude Home Assistant dostupný na adrese `http://homeassistant.local:8123`.

Podrobný oficiální návod: [home-assistant.io/installation](https://www.home-assistant.io/installation/)

### 2. Instalace MQTT brokeru (Mosquitto)

Majordomus komunikuje s Home Assistantem přes MQTT. V HAOS nainstalujete broker jedním kliknutím:

1. Přejděte do **Nastavení → Aplikace → Obchod s doplňky**.
2. Vyhledejte **Mosquitto broker** a klikněte na **Nainstalovat**.
3. Po instalaci doplněk **Spusťte** a zapněte **Spustit při startu**.
4. V **Nastavení → Lidé → Uživatelé** vytvořte nového uživatele (např. `mqtt`) — jeho jméno a heslo použijete pro připojení Majordomu k brokeru.

### 3. Přidání repozitáře Majordomus

1. Přejděte do **Nastavení → Aplikace → Obchod s doplňky**.
2. Klikněte na menu **⋮** (vpravo nahoře) → **Repozitáře**.
3. Vložte adresu repozitáře a potvrďte:

```
https://github.com/jirihusak/majordomus-ha-addon
```

### 4. Instalace doplňku Majordomus

1. V obchodě s doplňky se nyní objeví sekce **Majordomus** — klikněte na doplněk **Majordomus**.
2. Klikněte na **Nainstalovat**.
3. Na záložce **Konfigurace** vyplňte přihlašovací údaje k MQTT (uživatel vytvořený v kroku 2). Výchozí `mqtt_host: core-mosquitto` ukazuje na doplněk Mosquitto — ten neměňte.
4. Doplněk **Spusťte** a zapněte **Spustit při startu** a **Zobrazit v bočním panelu**.

V bočním panelu Home Assistantu se objeví položka **Majordomus** — webové rozhraní Majordomus Control, kde nastavíte sériové porty a HW jednotky (viz [Konfigurace SW](sw-configuration.md)).

!!! note "Převodník USB → RS-485"
    Doplněk má povolený přístup k sériovým portům (UART). Převodník stačí připojit do USB — v rozhraní Majordomus Control ho pak najdete typicky jako `/dev/ttyUSB0`.

---

## Varianta B: Docker

Pokud provozujete Home Assistant v Dockeru (Home Assistant Container), doplňky nejsou k dispozici — Majordomus Control spustíte jako samostatný kontejner. Následující `docker-compose.yml` obsahuje kompletní sestavu: MQTT broker, Home Assistant i Majordomus Control.

```yaml
services:

  mosquitto:
    image: eclipse-mosquitto:2
    container_name: mosquitto
    restart: unless-stopped
    ports:
      - "1883:1883"
    volumes:
      - ./mosquitto/config:/mosquitto/config
      - ./mosquitto/data:/mosquitto/data

  homeassistant:
    image: ghcr.io/home-assistant/home-assistant:stable
    container_name: homeassistant
    restart: unless-stopped
    network_mode: host
    environment:
      - TZ=Europe/Prague
    volumes:
      - ./homeassistant:/config

  majordomus:
    image: ghcr.io/jirihusak/majordomus-control-amd64:latest
    container_name: majordomus
    restart: unless-stopped
    ports:
      - "8899:8899"
    devices:
      - /dev/ttyUSB0:/dev/ttyUSB0
    volumes:
      - ./majordomus:/config
    environment:
      MQTT_BROKER: "tcp://mosquitto:1883"
      MQTT_USERNAME: ""
      MQTT_PASSWORD: ""
      MQTT_TOPIC: "majordomus/"
      HA_DISCOVERY: "true"
      HA_TOPIC: "homeassistant/"
```

!!! warning "Zvolte správnou architekturu"
    Image existuje ve dvou variantách — použijte tu, která odpovídá vašemu zařízení:

    - `ghcr.io/jirihusak/majordomus-control-amd64` — běžné PC, NUC, x86 servery
    - `ghcr.io/jirihusak/majordomus-control-aarch64` — Raspberry Pi 4/5 a další ARM 64bit zařízení

Spuštění celé sestavy:

```bash
docker compose up -d
```

Po spuštění bude dostupné:

- **Home Assistant** — `http://<IP_ADRESA>:8123`
- **Majordomus Control** — `http://<IP_ADRESA>:8899`

!!! note "Mosquitto v Dockeru vyžaduje konfiguraci"
    Vytvořte soubor `mosquitto/config/mosquitto.conf` s tímto obsahem (pro první vyzkoušení bez hesla):

    ```
    persistence false
    allow_anonymous true
    listener 1883
    ```

---

## Jak pokračovat

1. V rozhraní Majordomus Control (`:8899` nebo panel v HA) nastavte sériové porty a HW jednotky → [Konfigurace SW](sw-configuration.md)
2. Propojte Home Assistant s MQTT brokerem → [Konfigurace Home Assistant](ha-configuration.md)
