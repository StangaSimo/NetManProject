# NetManProject
NetWork Management Project for educational purposes in University, it's a small tool for searching Black Holes on the network and keep track of the hosts that send the packets.
It runs a live inspection of the packets that the network adapter receive with the pcap library.

## Requisites
Linux:
```
sudo apt-get install libpcap-dev libncurses5-dev 
```
Mac with Brew: 
```
brew install libcap
```

## Example
```
make
sudo ./nmp -i NetworkAdapterName
```

## how it work
Il programma utilizza la libreria pcap, prende i pacchetti dalla scheda di rete data in input. 
Analizzando solo i pacchetti TCP, tramite una HashMap teniamo traccia di tutti gli host che hanno traffico rx o tx.

Ad ogni indirizzo nella hashmap associamo una struttura dati (struct DATA) per tenere le informazioni di:
- Tempo dell'ultimo pacchetto rx.
- Tempo dell'ultimo pacchetto tx.
- Bitmap compressa per l'elenco degli host con cui sono stati scambiati pacchetti.
- dst e src se l'host ha avuto traffico in entrata o uscita.

Usiamo una Bitmap compressa per tenere traccia di possibili blackhole, che teniamo aggiornata per poter stampare le informazioni a schermo.
Se un  host per 10 secondi non ha traffico in tx sebbene avendolo in rx allora viene segnalato come Black Hole, in modo da tenere aggiornato il suo stato nella tabella.
Se un BlackHole torna a funzionare il suo stato viene notificato e le strutture dati aggiornate.

Il programma ogni 10 minuti fa un ciclo di ottimizzazione delle compressed bitmap, e elimina gli host che non sono più attivi da 1 ora.

In caso di uscita, viene avviata la procedura per la free di tutte gli elementi nella HashMap, comprese le Bitmap.  
