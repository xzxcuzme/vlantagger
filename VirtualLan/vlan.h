#ifndef UNTITLED1_FILTER_H
#define UNTITLED1_FILTER_H


void errorExit(char err[]);

void iface_initialization(char *iface_in,
                          char *iface_out);

void vlan_tagger(char *buff1,
                 char *buff2,
                 uint16_t vlan);

int vlan_tegger();

void signal_handler();

void signal_rule();

void pid_handler();
//void raw_socket(char *iface_out,
//                struct sockaddr_ll *addr,
//                int size_addr);

#endif //UNTITLED1_FILTER_H