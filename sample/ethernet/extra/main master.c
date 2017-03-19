#include "ppv.h"
#include "prot.c"

u8 data[8];
u8 dat[4];
u8 Buffer[USPI_FRAME_BUFFER_SIZE];
u8 len=0;

u8 IP_ADDRESS[4] = {192,168,0,251};
#define OWN_IP_ADDRESS {192, 168, 0, 250}

 int main(void)
{
	init();

	u8 OwnMACAddress[MAC_ADDRESS_SIZE];

	u8 RMACAddress[MAC_ADDRESS_SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	u8 RAMACAddress[MAC_ADDRESS_SIZE] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	USPiGetMACAddress(OwnMACAddress);

	TARPFrame *pARPFrame;
	pARPFrame = (TARPFrame*)malloc(sizeof(TARPFrame));

	while (1)
	{
			 
		     pARPFrame->Ethernet.nProtocolType = BE(ETH_PROT_ARP);
			 pARPFrame->ARP.nHWAddressSpace = BE(HW_ADDR_ETHER);
			 pARPFrame->ARP.nProtocolAddressSpace = BE(PROT_ADDR_IP);
			 pARPFrame->ARP.nHWAddressLength = MAC_ADDRESS_SIZE;
			 pARPFrame->ARP.nProtocolAddressLength = IP_ADDRESS_SIZE;
			 pARPFrame->ARP.nOPCode = BE(ARP_REQUEST);

		memcpy(pARPFrame->ARP.ProtocolAddressSender,OwnIPAddress,IP_ADDRESS_SIZE);
		memcpy(pARPFrame->ARP.HWAddressSender,OwnMACAddress,MAC_ADDRESS_SIZE);
		memcpy(pARPFrame->ARP.ProtocolAddressTarget,IP_ADDRESS,IP_ADDRESS_SIZE);
		memcpy(pARPFrame->ARP.HWAddressTarget, RMACAddress, MAC_ADDRESS_SIZE);
		
		memcpy(pARPFrame->Ethernet.MACSender,OwnMACAddress,MAC_ADDRESS_SIZE);
		memcpy(pARPFrame->Ethernet.MACReceiver,RAMACAddress,MAC_ADDRESS_SIZE);	
		
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
			break;
		}

		if (!USPiSendFrame(pARPFrame, sizeof *pARPFrame))
		{
			LogWrite(FromSample, LOG_ERROR, "SendFrame failed");
			break;
		}
	 float ppv=0;
		while (ppv++ < sizeof(ppv));
	}
	USPiEnvClose();
	return EXIT_HALT;
}
