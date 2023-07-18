# NetManProject
NetWork Management Project è un tool per cercare dei possibile Black Holes in una rete e tenere traccia degli host con cui questi comunicano.

## Requisites
Linux:
```
sudo apt-get install libpcap-dev libncurses5-dev librrd-dev rrdtool
```

## Example
```
make
sudo ./nmp -i NetworkAdapterName
```

## How it works
Il programma utilizza la libreria pcap, prende i pacchetti dalla scheda di rete data in input. 
Analizzando solo i pacchetti TCP, tramite una hashmap teniamo traccia di tutti gli host che hanno traffico rx o tx.

Ad ogni indirizzo nella hashmap associamo una struttura dati (struct DATA) per tenere le informazioni di:
- Tempo dell'ultimo pacchetto rx.
- Tempo dell'ultimo pacchetto tx.
- Bitmap compressa per l'elenco degli host con cui sono stati scambiati pacchetti.
- dst e src se l'host ha avuto traffico in entrata o uscita.
- il numero totale di pacchetti scambiati nel secondo precedente.

Usiamo una Bitmap compressa (bitmap_BH) per tenere traccia di possibili blackhole.
Ogni secondo stampiamo a schermo le varie informazioni contenute nell'hashmap.
Se un  host per 5 secondi non ha traffico in tx, sebbene avendolo in rx, viene segnalato come Black Hole, in modo da tenere aggiornato il suo stato nella tabella.
Se un BlackHole torna a funzionare il suo stato viene notificato e le strutture dati aggiornate.

Per ogni elemento contenuto nella bitmap viene creato un file rrd contenente 60 elementi, chiamato con il nome dell'inirizzo ip. Ogni secondo prenderà valori compresi tra 0 e 1000000 tenendo aggiornati i dati ogni minuto. Per fare l'aggiornamento dei dati utilizzeremo la differenza tra il numero dei pacchetti e il valore al secondo precedente per rendere la serie stazionaria.
Ogni 5 secondi aggiorneremo un grafico con la media dei valori contenuti nell'archivio.

Il programma ogni 10 minuti fa un ciclo di ottimizzazione delle compressed bitmap, e elimina gli host che non sono più attivi da 1 ora.

In caso di uscita, viene avviata la procedura per la free di tutte gli elementi nella hashmap, comprese le Bitmap.  

Per testare l'applicazione abbiamo creato uno script in python che consente di bloccare i pacchetti in uscita dal proprio device creando così un blackhole locale. 
Quando arriverà un pacchetto viene creato il file rrd, aggiunto alla bitmap_BH l'indirizzo IP associato e creato un grafico che si aggiornerà ogni 5 secondi. Se riceve il primo pacchetto nei primi 20 secondi in cui ne manda all'inizio viene segnalato visualizzando a schermo il colore giallo, se continua a ricevere pacchetti allora verrà visualizzato il colore rosso. 
Se riprende a funzionare verrà aggiornato lo stato in verde e sarà rimosso dalla bitmap_BH.