# NetManProject
NetWork Management Project è un tool per cercare dei possibile Black Holes in una rete e tenere traccia degli host con cui questi comunicano.

## Requisites
/////////Linux:
```
sudo apt-get install libpcap-dev libncurses5-dev librrd-dev
```
////////Mac with Brew: 
////////```
////////brew install libcap rrdtool 
////////```

## Example
```
make
sudo ./nmp -i NetworkAdapterName
```

## How it work
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
