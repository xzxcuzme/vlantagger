//
// Created by denis on 24.11.22.
//

#ifndef UNTITLED1_FILTER_H
#define UNTITLED1_FILTER_H

struct filter
{
    unsigned int ip[33];
    unsigned int mask[33];
    uint16_t vlan;
};


struct filter *struct_initialization(struct filter *filters, int *scale);

uint16_t filter_ip(struct filter *filters, int scale, char *ip);

#endif //UNTITLED1_FILTER_H