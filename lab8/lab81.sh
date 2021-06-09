g++ -c lab81.cpp
g++ -o lab81 lab81.o -lpthread -lrt
sudo ./lab81
sudo setcap cap_sys_resource=eip lab81