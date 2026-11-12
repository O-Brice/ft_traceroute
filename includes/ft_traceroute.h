#ifndef FT_TRACEROUTE_H
# define FT_TRACEROUTE_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <netinet/udp.h>
# include <sys/time.h>
# include <sys/select.h>

# define MAX_TTL 30
# define PROBES_PER_TTL 3
# define BASE_PORT 33434

typedef struct s_traceroute {
    char                *hostname;
    char                ip_str[INET_ADDRSTRLEN];
    struct sockaddr_in  target_sa;
    int                 udp_sockfd;
    int                 icmp_sockfd;
    uint16_t            udp_port;
    uint8_t             curttl;

    int                 max_ttl;     // -m (défaut: 30)
    int                 nqueries;    // -q (défaut: 3)
    int                 base_port;   // -p (défaut: 33434)
    int                 wait_time;   // -w
    int                 first_ttl;   // -f
} t_traceroute;

void    parse_args(t_traceroute *data, int argc, char **argv);
void    resolve_hostname(t_traceroute *data, const char *host);
void    print_usage(void);
void    init_sockets(t_traceroute *data);
void    run_traceroute(t_traceroute *data);
void    send_probe(t_traceroute *data, struct timeval *send_time);
int     receive_probe(t_traceroute *data, struct timeval *send_time, char *last_ip, uint16_t expected_port, int *target_reached);

#endif