# TCP-Sockets: Sender & Receiver

| Datei | Inhalt |
|---|---|
| `common.h` | Gemeinsame Konstanten (IP, Port, Puffergrößen), `make_addr()`, Prototypen |
| `sender.c` / `receiver.c` | Teil 1: naiver Austausch mit `send` / `recv` |
| `sendx.c` / `recx.c` | Teil 2: Austausch mit eigener Protokollschicht `sendx` / `recvx` |

## Build & Start

Zuerst den Receiver starten, dann den Sender (jeweils in eigenem Terminal):

```sh
# Teil 1
gcc -Wall receiver.c -o receiver && ./receiver
gcc -Wall sender.c -o sender && ./sender

# Teil 2
gcc -Wall recx.c -o recx && ./recx
gcc -Wall sendx.c -o sendx && ./sendx
```

Der Sender fragt in einer Endlosschleife `count` und `len` ab (z. B. `5 10`). Mit Strg+C beenden.

---

## Teil 1: Naiver Empfang

Der Client baut eine TCP-Verbindung zum Server auf und schickt in einer Endlosschleife Serien von Nachrichten:

- Der Benutzer gibt `count` (Anzahl) und `len` (Länge, `0 < len < 10000`) ein.
- Es werden `count` Nachrichten mit je `len` identischen Bytes gesendet. Der Inhalt jeder Nachricht ist ihre Sendefolgenummer modulo 256 (`00 00 ...`, `01 01 ...`, ...).
- Der Sender gibt während des Sendens nichts aus, damit er nicht ausgebremst wird.

Der Server empfängt „naiv“ mit einem 64-KB-Puffer und gibt jeden `recv`-Aufruf so aus:

```
RECV-NO: xxx, LEN: yyy, CONTENTS: 00..01..02
```

**Ziel:** zeigen, dass TCP ein **Bytestrom** ist und keine Nachrichtengrenzen kennt.

### Beobachtung: Warum `recv` mal mehr, mal weniger liefert

Es gibt zwei Puffer:

1. **Kernel-Empfangspuffer:** Das Betriebssystem legt ankommende TCP-Segmente sofort dort ab.
2. **Anwendungspuffer:** `recv` kopiert alles, was gerade im Kernel-Puffer liegt (höchstens `buflen` Bytes), in den eigenen Puffer und entfernt es aus dem Kernel-Puffer.

Wie viele Bytes ein `recv` liefert, hängt also nur vom **Timing** ab:

- Wartet der Receiver bereits in `recv`, bekommt er nur das erste ankommende Segment.
- Ist der Receiver gerade mit dem langsamen `printf` beschäftigt, sammeln sich mehrere Nachrichten an, und das nächste `recv` holt sie alle auf einmal.
- Der Nagle-Algorithmus fasst kleine `send`-Aufrufe schon beim Sender zu einem Segment zusammen.
- Der Scheduler des Betriebssystems sorgt dafür, dass die Aufteilung bei jedem Lauf anders aussieht.

Nachrichten können deshalb zusammengefasst oder mitten durchgeschnitten ankommen. Bei UDP liefert dagegen jedes `recv` genau ein Datagramm.

---

## Teil 2: Protokollschicht 4b mit `sendx` / `recvx`

Zwischen Anwendung und Socket-API wird eine eigene Schicht eingezogen, die das Bytestrom-Verhalten von TCP vor der Anwendung verbirgt:

```
Anwendung          send/recv  ->  sendx/recvx
Schicht 4b         sendx() / recvx()   (Rahmung per Längenfeld)
Schicht 4a         Socket-API / TCP
```

Die Anwendung bleibt gleich; nur `send` wird durch `sendx` und `recv` durch `recvx` ersetzt (gleiche Parameter, gleicher Returnwert).

### PDU-Format

Jede Nachricht bekommt einen 2-Byte-Header mit der Nutzdatenlänge in Netzwerk-Byte-Order (Big Endian):

```
+--------+--------+------------------------+
| len_hi | len_lo |  Nutzdaten (len Bytes) |
+--------+--------+------------------------+
```

Die PDU ist also `len + 2` Bytes lang. Beispiel für `len = 300`: Header `01 2c`.

### `sendx`

1. Puffer der Größe `len + 2` anlegen.
2. Länge mit `htons` in die ersten 2 Bytes schreiben.
3. Nutzdaten mit `memcpy` dahinter kopieren.
4. Alles mit **einem** `send` verschicken. Zwei getrennte `send`-Aufrufe für Header und Daten können durch das Zusammenspiel von Nagle-Algorithmus und verzögertem ACK zu Verzögerungen führen.

### `recvx`

1. Genau 2 Bytes Header lesen und die Länge zusammensetzen: `len = (buf[0] << 8) | buf[1]`.
2. Genau `len` Bytes Nutzdaten lesen.
3. `len` zurückgeben (`0` = Verbindung geschlossen, `-1` = Fehler).

Ein einzelnes `recv(..., len, ...)` reicht **nicht**, weil `recv` auch weniger Bytes liefern kann. Deshalb liest die Hilfsfunktion `recv_all` in einer Schleife mit Restlänge:

```c
while (remaining > 0) {
    int r = recv(fd, buf + (n - remaining), remaining, flags);
    if (r <= 0) return r;
    remaining -= r;
}
```

- `buf + (n - remaining)` schreibt jeweils hinter die bereits empfangenen Bytes.
- Jedes `recv` fordert **alle noch fehlenden** Bytes an, es wird also nicht byteweise gelesen.
- Es werden nie mehr als `remaining` Bytes gelesen, also keine Bytes der nachfolgenden Nachricht.

**Ergebnis:** Jede Ausgabezeile entspricht genau einer gesendeten Nachricht. `LEN` ist immer gleich dem eingegebenen `len`, und `CONTENTS` enthält nur einen Wert (`00`, `01`, `02`, ...).

---

## Hinweise

### `bind`-Fehler nach Neustart

Wird der Receiver bei offener Verbindung beendet, schließt er die Verbindung als Erster und geht in den TCP-Zustand **TIME_WAIT** (unter Linux ca. 60 s). So lange ist der Port belegt, und `bind` schlägt mit `EADDRINUSE` fehl. Danach verschwindet der Fehler von selbst.

TIME_WAIT gibt es, damit verspätete Segmente der alten Verbindung keine neue Verbindung stören und das letzte ACK bei Verlust wiederholt werden kann.

Zustand prüfen:

```sh
ss -tan | grep 3322
```

Abhilfe: vor `bind` die Option `SO_REUSEADDR` setzen.

```c
int opt = 1;
setsockopt(receiver_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```

### Binärdaten ausgeben

- Puffer als `unsigned char` anlegen, sonst werden Bytes ab `0x80` als negative Zahlen gelesen und `%02x` gibt z. B. `ffffff80` aus.
- `%02x` gibt jedes Byte zweistellig in Hex aus (`5` → `05`).
- `%s` ist für Binärdaten ungeeignet: Bei `0x00` endet die Ausgabe, und ohne Nullbyte am Ende wird über den Puffer hinaus gelesen.