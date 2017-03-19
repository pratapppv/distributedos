#define OWN_IP_ADDRESS {192,168,0,250}
#include "ppv.h"
#include "prot.c"

u8 data[8];
u8 dat[4];
u8 Buffer[USPI_FRAME_BUFFER_SIZE];
u8 len=0;

int prime(int a)
{
	int flag = 1;
	for (int i = 2; i < a; i++)
	{
		if (a%i == 0)
		{
			flag = 0;
			break;
		}
	}
	return flag;
}

void prep(TARPFrame *pARPFrame)
{
	u8 OwnMACAddress[MAC_ADDRESS_SIZE];
	USPiGetMACAddress(OwnMACAddress);

	memcpy(pARPFrame->Ethernet.MACReceiver, pARPFrame->ARP.HWAddressSender, MAC_ADDRESS_SIZE);
	memcpy(pARPFrame->Ethernet.MACSender, OwnMACAddress, MAC_ADDRESS_SIZE);
	pARPFrame->ARP.nOPCode = BE(ARP_REPLY);

	memcpy(pARPFrame->ARP.HWAddressTarget, pARPFrame->ARP.HWAddressSender, MAC_ADDRESS_SIZE);
	memcpy(pARPFrame->ARP.ProtocolAddressTarget, pARPFrame->ARP.ProtocolAddressSender, IP_ADDRESS_SIZE);

	memcpy(pARPFrame->ARP.HWAddressSender, OwnMACAddress, MAC_ADDRESS_SIZE);
}

void send(TARPFrame *pARPFrame, u8 data[4])
{

	memcpy(pARPFrame->ARP.ProtocolAddressSender, data, IP_ADDRESS_SIZE);

	if (!USPiSendFrame(pARPFrame, sizeof *pARPFrame))
	{
		LogWrite(FromSample, LOG_ERROR, "USPiSendFrame failed");

		return;
	}

	LogWrite(FromSample, LOG_NOTICE, "PPVP DATA successfully sent");
}



 int main(void)
{
	init();

	u8 OwnMACAddress[MAC_ADDRESS_SIZE];
	USPiGetMACAddress(OwnMACAddress);

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
			|| pARPFrame->ARP.nOPCode != BE(ARP_REQUEST))
		{
			continue;
		}

		LogWrite(FromSample, LOG_NOTICE, "Valid PPVP request from %u.%u.%u.%u received",
			(unsigned)pARPFrame->ARP.ProtocolAddressSender[0],
			(unsigned)pARPFrame->ARP.ProtocolAddressSender[1],
			(unsigned)pARPFrame->ARP.ProtocolAddressSender[2],
			(unsigned)pARPFrame->ARP.ProtocolAddressSender[3]);

LogWrite(FromSample, LOG_NOTICE, "MAC RECEIVER request from %u.%u.%u.%u.%u.%u received",
			(unsigned)pARPFrame->Ethernet.MACReceiver[0],
			(unsigned)pARPFrame->Ethernet.MACReceiver[1],
			(unsigned)pARPFrame->Ethernet.MACReceiver[2],
			(unsigned)pARPFrame->Ethernet.MACReceiver[3],
			(unsigned)pARPFrame->Ethernet.MACReceiver[4],
			(unsigned)pARPFrame->Ethernet.MACReceiver[5]);

LogWrite(FromSample, LOG_NOTICE, "MAC SENDER request from %u.%u.%u.%u.%u.%u received",
			(unsigned)pARPFrame->Ethernet.MACSender[0],
			(unsigned)pARPFrame->Ethernet.MACSender[1],
			(unsigned)pARPFrame->Ethernet.MACSender[2],
			(unsigned)pARPFrame->Ethernet.MACSender[3],
			(unsigned)pARPFrame->Ethernet.MACSender[4],
			(unsigned)pARPFrame->Ethernet.MACSender[5]);


		if (memcmp(pARPFrame->ARP.ProtocolAddressTarget, OwnIPAddress, IP_ADDRESS_SIZE) != 0)
		{
			continue;
		}
		
		LogWrite(FromSample, LOG_NOTICE, "PPVP request is to us");
		u8 data[4] = { 0, 0, 0, 0 };
		

		prep(pARPFrame);
		for (int i = 0; i < 250; i++)
		{
			if (prime(i))
			{
				data[3] = i;
				send(pARPFrame, data);
			}
		}
		
	}
	USPiEnvClose();
	return EXIT_HALT;
}
