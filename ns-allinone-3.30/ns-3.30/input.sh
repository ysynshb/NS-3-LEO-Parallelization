#!/bin/bash
sudo ./waf && sudo mpirun --allow-run-as-root -np 6 ./waf --run "scratch/leochannal-6an-mn --numorbits=6 --numsatellites=8 --duration=150 --interval=0.01 --nummpi=6 --traceFile=pcap/leomn1.log"
sudo ./waf && sudo mpirun --allow-run-as-root -np 6 ./waf --run "scratch/leochannal-6an-mn --numorbits=6 --numsatellites=8 --duration=200 --interval=0.01 --nummpi=6 --traceFile=pcap/leomn2.log"
sudo ./waf && sudo mpirun --allow-run-as-root -np 6 ./waf --run "scratch/leochannal-6an-mn --numorbits=6 --numsatellites=8 --duration=250 --interval=0.01 --nummpi=6 --traceFile=pcap/leomn3.log"
sudo ./waf && sudo mpirun --allow-run-as-root -np 6 ./waf --run "scratch/leochannal-6an-mn --numorbits=6 --numsatellites=8 --duration=150 --interval=0.1 --nummpi=6 --traceFile=pcap/leomn4.log"
sudo ./waf && sudo mpirun --allow-run-as-root -np 6 ./waf --run "scratch/leochannal-6an-mn --numorbits=6 --numsatellites=8 --duration=150 --interval=0.05 --nummpi=6 --traceFile=pcap/leomn5.log"
sudo ./waf && sudo mpirun --allow-run-as-root -np 6 ./waf --run "scratch/leochannal-6an-mn --numorbits=6 --numsatellites=8 --duration=150 --interval=0.0001 --nummpi=6 --traceFile=pcap/leomn6.log"
read -r time1 < pcap/leomn1.log
read -r time2 < pcap/leomn2.log
read -r time3 < pcap/leomn3.log
read -r time4 < pcap/leomn4.log
read -r time5 < pcap/leomn5.log
read -r time6 < pcap/leomn6.log
echo "time1:$time1"
echo "time2:$time2"
echo "time3:$time3"
echo "time4:$time4"
echo "time5:$time5"
echo "time6:$time6"
