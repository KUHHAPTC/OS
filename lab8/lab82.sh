g++ -c lab82.cpp
g++ -o lab82 lab82.o -lpthread -lrt
sudo ./lab82
sudo setcap cap_sys_resource=eip lab82