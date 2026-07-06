# Aktualizace firmware

Všechny jednotky Majordomus podporují **aktualizaci firmware přímo po sběrnici RS-485**. Nemusíte nic demontovat ze zdi, nepotřebujete programátor — nový firmware nahrajete z webového rozhraní Majordomus Control a jednotka se aktualizuje na dálku.

Jak to funguje: každá jednotka obsahuje kromě aplikačního firmware i **zavaděč (bootloader)**, který se stará o přijetí nového firmware po sběrnici. Majordomus Control rozparsuje soubor `.hex`, po blocích ho odešle jednotce a po dokončení jednotku restartuje do nové aplikace.

---

## 1. Stažení firmware

Aktuální soubory firmware najdete na GitHubu v sekci Releases pod tagem **FW**:

→ [github.com/jirihusak/majordomus/releases](https://github.com/jirihusak/majordomus/releases)

Např. `RoomSens.v0.2.hex` pro RoomSensor nebo `RoomIO.v0.2.hex` pro RoomIO.

---

## 2. Umístění souborů do složky `updateFW`

Majordomus Control hledá firmware ve složce `updateFW` ve svém pracovním adresáři (u klasické instalace tedy typicky `~/majordomus/updateFW`). Uvnitř vytvořte podsložku **s přesným názvem typu zařízení** a do ní vložte soubor `.hex`:

```
~/majordomus/
├── config.xml
└── updateFW/
    ├── RoomSensor/
    │   └── RoomSens.v0.2.hex
    └── RoomIO/
        └── RoomIO.v0.2.hex
```

Název podsložky musí odpovídat typu zařízení tak, jak je uveden v konfiguraci:

| Typ zařízení | Název složky |
|---|---|
| RoomSensor | `updateFW/RoomSensor/` |
| RoomIO | `updateFW/RoomIO/` |
| BoxIO | `updateFW/BoxIO/` |
| TempOutBoard | `updateFW/TempOutBoard/` |
| RoomIR | `updateFW/RoomIR/` |

!!! note "Instalace v Dockeru"
    Pracovní adresář aplikace v kontejneru je `/app`. Přimapujte složku s firmwarem jako volume v `docker-compose.yml`:

    ```yaml
    volumes:
      - ./majordomus:/config
      - ./updateFW:/app/updateFW
    ```

---

## 3. Spuštění aktualizace

1. Otevřete webové rozhraní Majordomus Control (`http://<IP_ADRESA>:8899`).
2. Přejděte do sekce **Firmware Update**.
3. V poli **Device** vyberte jednotku, kterou chcete aktualizovat — nabídka firmware se automaticky omezí na soubory odpovídající jejímu typu.
4. V poli **Select Firmware** vyberte verzi.
5. Klikněte na **Start Update** a sledujte průběh na ukazateli.

Během aktualizace je jednotka mimo provoz (neodpovídá na dotazy, výstupy drží poslední stav). Aktualizace trvá řádově desítky sekund až jednotky minut.

!!! warning "Během aktualizace"
    Neodpojujte napájení jednotky ani převodník RS-485 a nerestartujte Majordomus Control. Pokud se aktualizace přesto přeruší, jednotka zůstane v zavaděči — nic se neztratí, aktualizaci jednoduše spusťte znovu.

---

## 4. Ověření

Po dokončení se jednotka restartuje do nového firmware. Verzi ověříte:

- ve **webovém rozhraní** Majordomus Control v detailu zařízení, nebo
- přes MQTT v tématu `<prefix>/<zařízení>/state/version`.

Verze obsahuje číslo a datum sestavení, např. `0.3(Nov  2 2024)`.
