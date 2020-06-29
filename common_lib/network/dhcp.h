#pragma once

#include <common_lib/utils.h>
#include <libopencm3/cm3/dwt.h>
#include <sched.h>
#include <stdlib.h>

#define DHCP_PACKET_MESSAGE_TYPE_BOOT_REQUEST 1

#define DHCP_PACKET_HARDWARE_TYPE_ETHERNET 0x01

#define DHCP_PACKET_BOOT_FLAGS_MULTICAST (1u << 15u)
#define DHCP_PACKET_BOOT_FLAGS_UNICAST (0u << 15u)

#define DHCP_STATE_INIT_REBOOT 0x00
#define DHCP_STATE_REBOOTING 0x01
#define DHCP_STATE_INIT 0x02
#define DHCP_STATE_SELECTING 0x03
#define DHCP_STATE_REQUESTING 0x04
#define DHCP_STATE_BOUND 0x05
#define DHCP_STATE_RENEWING 0x06
#define DHCP_STATE_REBINDING 0x07

typedef struct {
  uint8_t current_state;
  struct {

  } _internal;
} dhcp_state;

typedef struct {
  uint8_t id;
  uint8_t length;
  uint8_t *data;
} dhcp_option;

typedef struct __attribute__((packed)){
  uint8_t message_type;
  uint8_t hardware_type;
  uint8_t hardware_address_length;
  uint8_t hops;
  uint32_t transaction_id;
  uint16_t seconds_elapsed;
  uint16_t bootp_flags;
  uint32_t client_ip_address;
  uint32_t your_ip_address;
  uint32_t next_server_ip_address;
  uint32_t relay_agent_ip_address;
  uint8_t mac_address[16];
  uint8_t server_host_name[64];
  uint8_t boot_file_name[128];
  uint8_t dhcp_option[312];
} dhcp_packet;

error_t dhcp_init(dhcp_state *dhcp_state) {
  dhcp_state->current_state = DHCP_STATE_INIT;
  return E_SUCCESS;
}

void dhcp_discover(const uint8_t mac_addr[6], dhcp_packet *packet) {
  srand(dwt_read_cycle_counter());
  *packet = (dhcp_packet){
      .message_type = DHCP_PACKET_MESSAGE_TYPE_BOOT_REQUEST,
      .hardware_type = DHCP_PACKET_HARDWARE_TYPE_ETHERNET,
      .hardware_address_length = 6,
      .hops = 0,
      .transaction_id = rand(),
      .seconds_elapsed = 0,
      .bootp_flags = DHCP_PACKET_BOOT_FLAGS_UNICAST,
      .client_ip_address = 0x00000000,
      .your_ip_address = 0x00000000,
      .next_server_ip_address = 0x00000000,
      .relay_agent_ip_address = 0x00000000,
      .mac_address = {mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3],
                      mac_addr[4], mac_addr[5], 0, },
      .server_host_name = {0, },
      .boot_file_name = {0, },
      .dhcp_option = {0, },
  };
  // set options
  uint16_t opt_ptr = 0;
  packet->dhcp_option[opt_ptr++] = 0x63;
  packet->dhcp_option[opt_ptr++] = 0x82;
  packet->dhcp_option[opt_ptr++] = 0x53;
  packet->dhcp_option[opt_ptr++] = 0x63;

  packet->dhcp_option[opt_ptr++] = 53; // Option: DHCP Message Type
  packet->dhcp_option[opt_ptr++] = 1;  // Lenght: 1
  packet->dhcp_option[opt_ptr++] = 1;  // DHCP: Discover

  packet->dhcp_option[opt_ptr] = 0xff;  // Option: End
}
