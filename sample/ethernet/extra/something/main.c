#define OWN_IP_ADDRESS {192, 168, 0, 250}
#include "ppv.h"
#include "prot.c"

u8 data[8];
u8 dat[4];
u8 Buffer[USPI_FRAME_BUFFER_SIZE];
u8 len=0;



void ping(u8 IP_ADDRESS[4])
{
	u8 OwnMACAddress[MAC_ADDRESS_SIZE];

	u8 RMACAddress[MAC_ADDRESS_SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	u8 RAMACAddress[MAC_ADDRESS_SIZE] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	USPiGetMACAddress(OwnMACAddress);

	TARPFrame *pARPFrame;
	pARPFrame = (TARPFrame*)malloc(sizeof(TARPFrame));

	pARPFrame->Ethernet.nProtocolType = BE(ETH_PROT_ARP);
	pARPFrame->ARP.nHWAddressSpace = BE(HW_ADDR_ETHER);
	pARPFrame->ARP.nProtocolAddressSpace = BE(PROT_ADDR_IP);
	pARPFrame->ARP.nHWAddressLength = MAC_ADDRESS_SIZE;
	pARPFrame->ARP.nProtocolAddressLength = IP_ADDRESS_SIZE;
	pARPFrame->ARP.nOPCode = BE(ARP_REQUEST);

	memcpy(pARPFrame->ARP.ProtocolAddressSender, OwnIPAddress, IP_ADDRESS_SIZE);
	memcpy(pARPFrame->ARP.HWAddressSender, OwnMACAddress, MAC_ADDRESS_SIZE);
	memcpy(pARPFrame->ARP.ProtocolAddressTarget, IP_ADDRESS, IP_ADDRESS_SIZE);
	memcpy(pARPFrame->ARP.HWAddressTarget, RMACAddress, MAC_ADDRESS_SIZE);

	memcpy(pARPFrame->Ethernet.MACSender, OwnMACAddress, MAC_ADDRESS_SIZE);
	memcpy(pARPFrame->Ethernet.MACReceiver, RAMACAddress, MAC_ADDRESS_SIZE);

	LogWrite(FromSample, LOG_NOTICE, " ARP request frm %u.%u.%u.%u",
		(unsigned)pARPFrame->ARP.ProtocolAddressSender[0],
		(unsigned)pARPFrame->ARP.ProtocolAddressSender[1],
		(unsigned)pARPFrame->ARP.ProtocolAddressSender[2],
		(unsigned)pARPFrame->ARP.ProtocolAddressSender[3]);
	LogWrite(FromSample, LOG_NOTICE, " ARP request to %u.%u.%u.%u",
		(unsigned)pARPFrame->ARP.ProtocolAddressTarget[0],
		(unsigned)pARPFrame->ARP.ProtocolAddressTarget[1],
		(unsigned)pARPFrame->ARP.ProtocolAddressTarget[2],
		(unsigned)pARPFrame->ARP.ProtocolAddressTarget[3]);

	if (pARPFrame->Ethernet.nProtocolType != BE(ETH_PROT_ARP)
		|| pARPFrame->ARP.nHWAddressSpace != BE(HW_ADDR_ETHER)
		|| pARPFrame->ARP.nProtocolAddressSpace != BE(PROT_ADDR_IP)
		|| pARPFrame->ARP.nHWAddressLength != MAC_ADDRESS_SIZE
		|| pARPFrame->ARP.nProtocolAddressLength != IP_ADDRESS_SIZE)
	{
		LogWrite(FromSample, LOG_ERROR, "Send assesment error");
		
	}

	if (!USPiSendFrame(pARPFrame, sizeof *pARPFrame))
	{
		LogWrite(FromSample, LOG_ERROR, "SendFrame failed");
		
	}
	free(pARPFrame);
}

int main(void)
{
	u8 IP_ADDRESS[4] = { 192, 168, 0, 251 };
	init();
	for (u8 i = 1; i <= 100; i++)
	{
		ping(IP_ADDRESS);
	}

	while (1)
	{
		u8 Buffer[USPI_FRAME_BUFFER_SIZE];
		unsigned nFrameLength;
		if (!USPiReceiveFrame(Buffer, &nFrameLength))
		{
			continue;
		}

		LogWrite(FromSample, LOG_NOTICE, "Frame received (length %u)", nFrameLength);

		if (nFrameLength < sizeof(TARPFrame))
		{
			continue;
		}

		TARPFrame *pARPFrame = (TARPFrame *)Buffer;
		if (pARPFrame->Ethernet.nProtocolType != BE(ETH_PROT_ARP)
			|| pARPFrame->ARP.nHWAddressSpace != BE(HW_ADDR_ETHER)
			|| pARPFrame->ARP.nProtocolAddressSpace != BE(PROT_ADDR_IP)
			|| pARPFrame->ARP.nHWAddressLength != MAC_ADDRESS_SIZE
			|| pARPFrame->ARP.nProtocolAddressLength != IP_ADDRESS_SIZE
			|| pARPFrame->ARP.nOPCode != BE(ARP_REPLY))
		{
			continue;
		}

		LogWrite(FromSample, LOG_NOTICE, "Valid PPVP request from %u.%u.%u.%u received",
			(unsigned)pARPFrame->ARP.ProtocolAddressSender[0],
			(unsigned)pARPFrame->ARP.ProtocolAddressSender[1],
			(unsigned)pARPFrame->ARP.ProtocolAddressSender[2],
			(unsigned)pARPFrame->ARP.ProtocolAddressSender[3]);

		if (memcmp(pARPFrame->ARP.ProtocolAddressTarget, OwnIPAddress, IP_ADDRESS_SIZE) != 0)
		{
			continue;
		}

		LogWrite(FromSample, LOG_NOTICE, "PPVP request is to us");

		LogWrite(FromSample, LOG_NOTICE, "DATA:  %u.%u.%u.%u ",
			(unsigned)pARPFrame->ARP.ProtocolAddressTarget[0],
			(unsigned)pARPFrame->ARP.ProtocolAddressTarget[1],
			(unsigned)pARPFrame->ARP.ProtocolAddressTarget[2],
			(unsigned)pARPFrame->ARP.ProtocolAddressTarget[3]);

	}

	USPiEnvClose();
	return EXIT_HALT;
}
