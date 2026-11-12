#include "ft_traceroute.h"

int main(int argc, char **argv)
{
    t_traceroute data;

    memset(&data, 0, sizeof(t_traceroute));
    
    parse_args(&data, argc, argv);

    init_sockets(&data);

    printf("traceroute to %s (%s), %d hops max, 60 bytes packets\n", 
           data.hostname, data.ip_str, MAX_TTL);

    run_traceroute(&data);

    close(data.udp_sockfd);
    close(data.icmp_sockfd);
    return (0);
}