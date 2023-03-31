# NetManProject
NetWork Management Project for educational purposes, it's a small tool for search Black Hole Hosts on the network.
It runs a live inspection of the packets that the network adapter receive with the pcap library.

## Requisites
```
sudo apt-get install libpcap-dev
```

## Example
```
make
sudo ./nmp -i en0
```
