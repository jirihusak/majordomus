# Majordomus

## Chytrý dům, který dává smysl

Majordomus je open-source systém pro řízení chytré domácnosti. Postavený na spolehlivé drátové sběrnici RS485, modulárních jednotkách a otevřeném softwaru. Navržený tak, aby vydržel desítky let — stejně jako váš dům.

---

## Jak to celé začalo

V roce 2018 jsem začal s rekonstrukcí rodinného domu a řešil otázku, kterou řeší každý, kdo rekonstruuje: **jak na chytrý dům?**

Mám vystudovaný obor vestavěných systémů na VUT FIT. Mít chytrý dům je sen každého nadšence do IT a elektroniky. Jenže jaký systém vybrat?

**Komerční systémy** jako KNX nebo Loxone mají vyšší pořizovací cenu, ale hlavním problémem je uzavřenost. Proprietární ekosystém, závislost na jednom výrobci a k tomu několik krabiček do každé místnosti, pokud chcete pokrýt všechno. A typicky obří rozvaděč v technické místnosti. Uživatel pokud chce něco trošku jinak, než je standard, tak může narazit na limity.

**Bastlířské projekty** na Arduinu a ESP32 řešily vždy jen jeden konkrétní problém — třeba senzor CO2 nebo chytré relé. Pokud bych chtěl v místnosti měřit teplotu, vlhkost, CO2, osvětlení, přítomnost, ovládat světla a žaluzie, skončil bych s hromadou malých krabiček, spletencem kabelů a nulovou zárukou, že to za rok bude ještě fungovat.

**Řekl jsem si: musí to jít jinak.**

Programování firmware a software pro mě nebyl problém. Začal jsem se ale ponořit do návrhu elektroniky — protože jsem chtěl jediné kompaktní zařízení do každé místnosti, které pokryje všechno. A spolehlivou drátovou infrastrukturu, která vydrží do další rekonstrukce.

Po několika měsících vývoje a iterací prototypů vznikl funkční ekosystém. A rozhodl jsem se ho nabídnout dalším.

---

## Pro koho je Majordomus

Majordomus je pro vás, pokud:

- **Rekonstruujete nebo stavíte dům** a chcete chytrou domácnost, která vydrží desítky let.
- **Nechcete být závislí na jednom výrobci** — chcete otevřený systém, který si můžete upravit.
- **Chcete spolehlivost drátového řešení**, ne závislost na Wi-Fi a baterky v senzorech.
- **Hledáte rozumný poměr cena/výkon** bez kompromisů na kvalitě komponent.
- **Jste elektrikář nebo instalatér**, který chce nabídnout klientům moderní řešení bez vendor lock-in.

---

## Základní filozofie

### Hloupé zařízení + chytrý mozek

Majordomus staví na jednoduchém principu: **použijte klasická, osvědčená zařízení a přidejte k nim chytré řízení.**

Příklad? Obyčejná žárovka za pár korun. Spíná ji chytrý modul Majordomu. Když žárovka praskne, vyměníte žárovku — ne celé chytré svítidlo za tisíce, které za tři roky možná ani nekoupíte. Stejný princip platí pro ventily topení, ovládání žaluzií, čerpadla a další zařízení.

### Jeden modul na místnost

Žádná hromada krabiček po zdech. Jedna kompaktní jednotka RoomSensor v instalační krabici pokryje vše — teplotu, vlhkost, CO2, VOC, intenzita osvětlení, hluk, akustické upozornění, přítomnost osob, ovládání stmívání světel i ovládací tlačítka.

### Drát, který vydrží

Čtyři vodiče sběrnice RS485 (12V, GND, A, B) vedou po domě a spojují všechny jednotky v patře. Žádné Wi-Fi, žádné baterie, žádné výpadky. Infrastruktura, která bude fungovat tak dlouho jako elektroinstalace. 

### Otevřenost bez kompromisů

Celý projekt je open-source — hardware i software. Komunikace přes standardní MQTT protokol. Integrace s Node-RED a Home Assistant. Žádný vendor lock-in, žádné cloudové závislosti.

---

## Jak to funguje — v kostce

```
┌─────────────┐     USB      ┌──────────────┐    RS485 sběrnice
│  Mini PC    │◄────────────►│  RS485       │◄──── 4 dráty ────►  několik RoomSensor (místnost)
│  (Rpi...)   │              │  převodník   │◄──── 4 dráty ────►  RoomIO (rozvaděč výstupů)
│             │              └──────────────┘
│  ┌────────┐ │
│  │Majord. │ │     MQTT
│  │Control │ │◄──────────►  Node-RED (logika: topení, světla, žaluzie)
│  └────────┘ │◄──────────►  Home Assistant (UI, integrace, uživatelské scény)
└─────────────┘
```

**Majordomus Control** — brána, která překládá RS485 komunikaci na MQTT zprávy.
**Node-RED** — zpracovává kritickou logiku domu (topení, osvětlení, žaluzie).
**Home Assistant** — uživatelské rozhraní, mobilní aplikace, integrace s dalšími zařízeními (TV, FVE, auto...).

---

