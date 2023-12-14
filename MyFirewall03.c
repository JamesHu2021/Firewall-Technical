//
//  MyFirewall03.c
//  MyFirewall03
//
//  Created by Huafang Zhang on 2023-12-13.
//

#include <stdio.h>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

void packet_handler(unsigned char *user, const struct pcap_pkthdr *pkthdr, const unsigned char *packet) {
    
    // Debug: Print packet length
    printf("Packet captured, length: %d\n", pkthdr->len);
    
    
    struct ip *ip_header = (struct ip*)(packet + 14); // Ethernet header is 14 bytes
    struct tcphdr *tcp_header = (struct tcphdr*)(packet + 14 + ip_header->ip_hl * 4); // IP header size is in 4-byte words

    // Check if it's a TCP packet
    if (ip_header->ip_p == IPPROTO_TCP) {
        // Check if it's a connection request (SYN flag set)
        if (tcp_header->th_flags & TH_SYN) {
            // Print information about the connection request
            printf("Connection request from %s:%d to %s:%d\n",
                   inet_ntoa(ip_header->ip_src), ntohs(tcp_header->th_sport),
                   inet_ntoa(ip_header->ip_dst), ntohs(tcp_header->th_dport));

            // TODO: Implement your own logic to allow or block the connection request based on your requirements
            // For demonstration purposes, this example just prints the connection information.
        }
    }
}

int main(int argc, const char * argv[]) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    // Open a network device for packet capture (replace "eth0" with your network interface)
    handle = pcap_open_live("en0", BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device eth0: %s\n", errbuf);
        return 2;
    }

    // Set a filter to capture only TCP packets
    struct bpf_program fp;
    char filter_exp[] = "ip";//"tcp";
    bpf_u_int32 net;
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return 2;
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return 2;
    }

    // Start capturing packets
    pcap_loop(handle, 0, packet_handler, NULL);

    // Close the capture session
    pcap_close(handle);

    return 0;
}

