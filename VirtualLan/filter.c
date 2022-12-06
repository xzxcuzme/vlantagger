#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "filter.h"
#include <netinet/in.h>
#include <arpa/inet.h>

#define FILTER "/var/run/filter.txt"

struct filter *struct_initialization(struct filter *filters, int *scale)
{
    FILE *file;
    char rule_in_file[37] = {0};
    char str_ip[16] = {0};
    char str_mask[16] = {0};
    char str_vlan[5] = {0};

    if ((file = fopen(FILTER, "r")) == NULL) {
        perror("Failed to open file");
    }

    while (fscanf(file, "%s", rule_in_file) != EOF) {
        sscanf(rule_in_file, "%[^/]/%[^/]/%s", str_ip, str_mask, str_vlan);

        filters = realloc(filters, (*scale) * sizeof(struct filter));

        if (filters == NULL) {
            perror("Memory problem");
        }

        memcpy(filters[*scale-1].ip, str_ip, sizeof(str_ip));
        memcpy(filters[*scale-1].mask, str_mask, sizeof(str_mask));
        filters[*scale-1].vlan = atoi(str_vlan);

        (*scale)++;
    }
    fclose(file);
    return filters;

}

uint16_t filter_ip (struct filter *filters, int scale, char *ip)
{
    char init_ip[11] = {"0x"};
    strcat(init_ip, ip);
    char addr[4] = {0};
    char host[4] = {0};

    if (inet_aton(init_ip, (struct in_addr*)&host) == 0) {
        perror(__func__);
    }

    for (int i = 0; i < scale-1; i++) {
        if (inet_aton((const char *) filters[i].mask, (struct in_addr*)&addr) == 0) {
            perror(__func__);
        }

        inet_ntoa(*(struct in_addr*)&addr);

        for (int b = 0; b < 3; b++) {
            addr[b] = addr[b] & host[b];
        }

        if (strcmp(inet_ntoa(*(struct in_addr*)&addr), (char *)filters[i].ip) == 0) {
            return filters[i].vlan;
        }
    }
    return 0;
}