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
