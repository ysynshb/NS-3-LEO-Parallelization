#!/bin/bash
sudo ./waf && sudo ./waf --run "leodelay  --orbitFile=contrib/leo/data/orbits/starlink1.csv  --traceFile=pcap/dsai1.log  --interval=10"
sudo ./waf && sudo ./waf --run "leodelay  --orbitFile=contrib/leo/data/orbits/starlink1.csv  --traceFile=pcap/dsai2.log --interval=1"
sudo ./waf && sudo ./waf --run "leodelay  --orbitFile=contrib/leo/data/orbits/starlink1.csv  --traceFile=pcap/dsai3.log  --interval=0.1"
sudo ./waf && sudo ./waf --run "leodelay  --orbitFile=contrib/leo/data/orbits/starlink1.csv  --traceFile=pcap/dsai4.log  --interval=0.01"
sudo ./waf && sudo ./waf --run "leodelay  --orbitFile=contrib/leo/data/orbits/starlink1.csv  --traceFile=pcap/dsai5.log  --interval=0.001"
read -r time1 < pcap/dsai1.log
read -r time2 < pcap/dsai2.log
read -r time3 < pcap/dsai3.log
read -r time4 < pcap/dsai4.log
read -r time5 < pcap/dsai5.log

echo "time1:$time1"
echo "time2:$time2"
echo "time3:$time3"
echo "time4:$time4"
echo "time5:$time5"


