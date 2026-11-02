Ich bin im Ask-Modus und kann die Datei nicht selbst ändern. Hier ist der Inhalt für die `README.md` zum Einfügen:

````markdown
# TCP-Sockets: Sender & Receiver

## Übung

Ein Client (`sender.c`) baut eine TCP-Verbindung zum Server (`receiver.c`) auf und schickt in einer Endlosschleife Serien von Nachrichten:

- Der Benutzer gibt `count` (Anzahl) und `len` (Länge, `0 < len < 10000`) ein.
- Es werden `count` Nachrichten mit je `len` identischen Bytes gesendet. Der Inhalt jeder Nachricht ist ihre Sendefolgenummer modulo 256 (`00 00 ...`, `01 01 ...`, ...).
- Der Sender gibt während des Sendens nichts aus, damit er nicht ausgebremst wird.

Der Server empfängt „naiv“ mit einem 64-KB-Puffer und gibt jeden `recv`-Aufruf so aus:

```
RECV-NO: xxx, LEN: yyy, CONTENTS: 00..01..02
```

**Ziel:** zeigen, dass TCP ein **Bytestrom** ist und keine Nachrichtengrenzen kennt.

## Build & Start

```sh
gcc -Wall receiver.c -o receiver && ./receiver
gcc -Wall sender.c -o sender && ./sender
```

## Beobachtung: Warum `recv` mal mehr, mal weniger liefert

Es gibt zwei Puffer:

1. **Kernel-Empfangspuffer:** Das Betriebssystem legt ankommende TCP-Segmente sofort dort ab.
2. **Anwendungspuffer:** `recv` kopiert alles, was gerade im Kernel-Puffer liegt (höchstens `buflen` Bytes), in den eigenen Puffer.

Wie viele Bytes ein `recv` liefert, hängt also nur vom **Timing** ab:

- Wartet der Receiver bereits in `recv`, bekommt er nur das erste ankommende Segment.
- Ist der Receiver gerade mit dem langsamen `printf` beschäftigt, sammeln sich mehrere Nachrichten an, und das nächste `recv` holt sie alle auf einmal.
- Der Nagle-Algorithmus fasst kleine `send`-Aufrufe schon beim Sender zu einem Segment zusammen.
- Der Scheduler des Betriebssystems sorgt dafür, dass die Aufteilung bei jedem Lauf anders aussieht.

Nachrichten können deshalb zusammengefasst oder mitten durchgeschnitten ankommen. Wer einzelne Nachrichten unterscheiden will, muss selbst eine Rahmung einführen, z. B. eine feste Länge oder einen Längen-Header. Bei UDP liefert dagegen jedes `recv` genau ein Datagramm.

## Hinweis: `bind`-Fehler nach Neustart

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
````