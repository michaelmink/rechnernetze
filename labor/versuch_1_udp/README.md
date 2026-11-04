# Versuch 1: UDP-Statistikserver

Clients schicken Textzeilen per UDP an einen Server. Der Server berechnet für jeden Client getrennt die Länge sowie Min, Max und Durchschnitt aller bisherigen Nachrichten und schickt das Ergebnis zurück.

## Dateien
- `server.c`: Server (Port 3322, 127.0.0.1)
- `client.c`: Client (liest Zeilen von stdin, beendet sich bei `Exit`)
- `stats.c`: Statistik pro Client, identifiziert über `<IP>_<Port>`
- `common.h`: Gemeinsame Includes, Konstanten und das `Stats`-Struct

## Build & Run
```sh
gcc -Wall server.c stats.c -o server && ./server
gcc -Wall client.c -o client && ./client   # mehrere Terminals = mehrere Clients
```

## Netzwerkverkehr mitschneiden
```sh
sudo tcpdump -i lo -n -X udp port 3322
```

## Hauptpunkte
- **Minimale Nachrichtenlänge:** Gesendet werden nur die Nutzbytes, ohne `\n` und ohne `\0`. Die Länge kommt aus `strcspn`, nicht aus `sizeof(buffer)`.
- **Nullbyte:** `recvfrom` hängt kein `\0` an. Deshalb wird mit `BUFSIZE - 1` empfangen und danach `buf[len] = '\0'` gesetzt.
- **Clients unterscheiden:** Jeder Client wird über IP und Port identifiziert (`client_id`). Es gibt höchstens `MAX_CLIENTS = 10`, danach antwortet der Server mit "Server voll".
- **Durchschnitt:** `(double)sum / count`. Ohne den Cast wäre es eine Ganzzahldivision.