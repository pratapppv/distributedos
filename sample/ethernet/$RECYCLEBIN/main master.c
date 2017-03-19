#include "ppv.h"
#include "prot.c"

u8 data[4] = { 1, 2, 3, 4 };
u8 data1[4] = { 5, 6, 7, 8 };
u8 Buffer[USPI_FRAME_BUFFER_SIZE];
u8 len=0;

u8 IP_ADDRESS[4] = {192,168,0,251};

 int main(void)
{
	init();

	u8 OwnMACAddress[MAC_ADDRESS_SIZE];
	u8 RMACAddress[MAC_ADDRESS_SIZE]={0xff};
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

		memcpy(pARPFrame->ARP.ProtocolAddressSender,data,IP_ADDRESS_SIZE);
		memcpy(pARPFrame->ARP.HWAddressSender,data1,MAC_ADDRESS_SIZE);

		memcpy(pARPFrame->ARP.ProtocolAddressTarget,IP_ADDRESS,IP_ADDRESS_SIZE);
		memcpy(pARPFrame->ARP.HWAddressTarget, RMACAddress, MAC_ADDRESS_SIZE);
		
		memcpy(pARPFrame->Ethernet.MACSender,OwnMACAddress,MAC_ADDRESS_SIZE);
		memcpy(pARPFrame->Ethernet.MACReceiver,RMACAddress,MAC_ADDRESS_SIZE);	

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
	}
	USPiEnvClose();
	return EXIT_HALT;
}
