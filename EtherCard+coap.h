// EtherCard+coap.h -- boilerplate for connecting microcoap and EtherCard

#pragma once

#ifndef COAP_PORT
#define COAP_PORT 5683
#endif

#ifndef PACKET_BUF_SIZE
#define PACKET_BUF_SIZE 256
#endif

uint8_t packetbuf[PACKET_BUF_SIZE];
static uint8_t scratch_raw[32];
static coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};

void coap_udp_callback(word myport, byte ip[4], unsigned int port, const char *data, word len) {
	uint8_t rc;
	coap_packet_t pkt;
	if (0 != (rc = coap_parse(&pkt, (uint8_t*)data, len))) {
#ifdef DEBUG
		Serial.print("badpkt rc=");
		Serial.println(rc, DEC);
#endif
	} else {
		size_t rsplen = sizeof(packetbuf);
		coap_packet_t rsppkt;
		coap_handle_req(&scratch_buf, &pkt, &rsppkt);
		memset(packetbuf, 0, rsplen);
		if (0 != (rc = coap_build(packetbuf, &rsplen, &rsppkt))) {
#ifdef DEBUG
			Serial.print("buildfail rc=");
			Serial.println(rc, DEC);
#endif
		} else {
			ether.sendUdp((char*)packetbuf, rsplen, myport, ip, port);
		}
	}
}

void coap_ethercard_begin() {
	ether.udpServerListenOnPort(&coap_udp_callback, COAP_PORT);
	// coap_setup(); // It's empty in microcoap
}
