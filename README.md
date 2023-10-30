# Overview
This is our project file code about NS-3-LEO-Parallelization, we uploaded the whole folder of ns-3, so we need to declare the configuration of the dependent environments needed to run the project and the method to run NS-3-LEO-Parallelization. Our contribution in ns-3 is also described.The project is performed on a 32 Intel(R) Xeon(R) Gold 5220 CPU @ 2.20 GHz, 16 GB of RAM and the Ubuntu 18.04 operating system.
# Environment and dependence
The following commands are from the official ns-3 website.[https://www.nsnam.org/wiki/Installation](https://www.nsnam.org/wiki/Installation)
```Shell
sudo apt-get install gcc g++ python python3
sudo apt-get install gcc g++ python python3 python3-dev
sudo apt-get install python3-setuptools git mercurial
sudo apt-get install qt5-default mercurial
sudo apt-get install gir1.2-goocanvas-2.0 python-gi python-gi-cairo python-pygraphviz python3-gi python3-gi-cairo python3-pygraphviz gir1.2-gtk-3.0 ipython ipython3  
sudo apt-get install openmpi-bin openmpi-common openmpi-doc libopenmpi-dev
sudo apt-get install autoconf cvs bzr unrar
sudo apt-get install gdb valgrind 
sudo apt-get install uncrustify
sudo apt-get install doxygen graphviz imagemagick
sudo apt-get install texlive texlive-extra-utils texlive-latex-extra texlive-font-utils dvipng latexmk
sudo apt-get install python3-sphinx dia 
sudo apt-get install gsl-bin libgsl-dev libgsl23 libgslcblas0
sudo apt-get install tcpdump
sudo apt-get install sqlite sqlite3 libsqlite3-dev
sudo apt-get install libxml2 libxml2-dev
sudo apt-get install cmake libc6-dev libc6-dev-i386 libclang-6.0-dev llvm-6.0-dev automake 
sudo apt-get install libgtk2.0-0 libgtk2.0-dev
sudo apt-get install vtun lxc uml-utilities
sudo apt-get install libboost-signals-dev libboost-filesystem-dev
```
# Download project and compile
```Shell
git clone https://github.com/ysynshb/NS-3-LEO-Parallelization.git
cd ns-allinone-3.30/ns-3.30/
sudo ./waf -d debug --enable-examples --enable-tests --enable-mpi configure         
sudo ./waf
```
# Run our example
Accelerating mega-scale satellite network constellation simulation via MPI-based parallelization
```Shell
sudo ./waf && sudo mpirun --allow-run-as-root -np 6 ./waf --run scratch/leochannal-6an-mn
```
control experiment
```Shell
sudo ./waf && sudo ./waf --run scratch/leochannal-6an-mn-compare
```
# Run the script that automatically generates the optimal partitioning policy
Outputs the partitioning strategy with the best acceleration according to different input parameters
```Shell
sudo ./input.sh
```
Output the optimal partitioning scheme based on the number of partitions and topology size
```Shell
python partitionbyweight.py
```
