#include <uspienv.h>
#include <uspi.h>
#include <uspios.h>
#include <uspienv/util.h>
#include <uspienv/macros.h>
#include <uspienv/types.h>

#define	OWN_IP_ADDRESS		{192, 168, 0, 250}	// must be a valid IP address on your LAN
#define	DEST_IP_ADDRESS		{192, 168, 0, 171}
#define MAC_ADDRESS_SIZE	6
#define IP_ADDRESS_SIZE		4

typedef struct header
{
	u8 IPSender[IP_ADDRESS_SIZE];
	u8	IPReceiver[IP_ADDRESS_SIZE];
	u8  PADD;
    #define protocol 0x11
	u16 UDP_len;
	u16 source_port;
	u16 dest_port;
	u16 length;
	u16 chksum;
	int data;
}PACKED header;

header *UDP;
static const u8 OwnIPAddress[] = OWN_IP_ADDRESS;
static const u8 ToIPAddress[] = DEST_IP_ADDRESS;

static const char frm[] = "PRATAP->P->VANGOL";




int main(void)
{
	if (!USPiEnvInitialize())
	{
		return EXIT_HALT;
	}

	if (!USPiInitialize())
	{
		LogWrite(frm, LOG_ERROR, "Cannot initialize USPi");

		USPiEnvClose();

		return EXIT_HALT;
	}

	if (!USPiEthernetAvailable())
	{
		LogWrite(frm, LOG_ERROR, "Ethernet device not found");

		USPiEnvClose();

		return EXIT_HALT;
	}

	LogWrite("ppv", LOG_NOTICE, "gonna get the mac address");
	u8 OwnMACAddress[MAC_ADDRESS_SIZE];
	USPiGetMACAddress(OwnMACAddress);
	LogWrite("ppv", LOG_NOTICE, "got the mac address");

	UDP = (header *)malloc(sizeof(header));

	while (1)
	{
		UDP->PADD = 0x0000000000;
		LogWrite("ppv", LOG_NOTICE, "gonna assign the ip(sender ip)");
		(*UDP).IPSender[0] = OwnIPAddress[0];
		(*UDP).IPSender[1] = OwnIPAddress[1];
		(*UDP).IPSender[2] = OwnIPAddress[2];
		(*UDP).IPSender[3] = OwnIPAddress[3];
		LogWrite("ppv", LOG_NOTICE, "finished assigning the ip  : %u.%u.%u.%u", UDP->IPSender[0], UDP->IPSender[1], UDP->IPSender[2], UDP->IPSender[3]);

		LogWrite("ppv", LOG_NOTICE, "assigning the to ip ");
		(*UDP).IPReceiver[0] = ToIPAddress[0];
		(*UDP).IPReceiver[1] = ToIPAddress[1];
		(*UDP).IPReceiver[2] = ToIPAddress[2];
		(*UDP).IPReceiver[3] = ToIPAddress[3];
		LogWrite("ppv", LOG_NOTICE, "finished assigning the ip : %u.%u.%u.%u", UDP->IPReceiver[0], UDP->IPReceiver[1], UDP->IPReceiver[2], UDP->IPReceiver[3]);

		LogWrite("ppv", LOG_NOTICE, "gonna assign the port");
		(*UDP).source_port = 912;
		(*UDP).dest_port = 912;

		LogWrite("ppv", LOG_NOTICE, "finished assigning the ports sender %u receiver %u", UDP->source_port, UDP->dest_port);
		UDP->data = 912;
		LogWrite("ppv", LOG_NOTICE, "assigned the daTA %u", UDP->data);

		UDP->length = sizeof(UDP->source_port) + sizeof(UDP->dest_port) + sizeof(UDP->length) + sizeof(UDP->chksum) + sizeof(UDP->data);
		LogWrite("ppv", LOG_NOTICE, "set the size %u", UDP->length);

		UDP->chksum = 0;
		LogWrite("ppv", LOG_NOTICE, "set the checksum to 0 as im not using it: %u", UDP->chksum);
		LogWrite("ppv", LOG_NOTICE, "gonna send the frame nw!!! fingers crossed.");

		if (!USPiSendFrame(UDP, sizeof *UDP))
		{
			LogWrite(frm, LOG_ERROR, "USPiSendFrame failed");
			break;
		}

		LogWrite(frm, LOG_ERROR, "SENT SUCCESSFULLY :P");
	}
return 0;
}