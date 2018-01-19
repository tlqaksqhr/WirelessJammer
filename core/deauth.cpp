#include <string>
#include <tins/tins.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include "redox.hpp"
#include <unistd.h>

using namespace Tins;
using namespace std;
using redox::Redox;
using redox::Command;

class DeauthDeamon{
public:
	DeauthDeamon() { }
	RadioTap GenPacket(string apAddress,string stationAddress,string iface);
	void Run(string iface);
};

RadioTap DeauthDeamon::GenPacket(string apAddress,string stationAddress,string iface)
{
	cout << "AP Address : " << apAddress << endl;
	cout << "Station Address : " << stationAddress << endl;

	PacketSender Sender;

	Dot11::address_type AP = apAddress;
	Dot11::address_type ST = stationAddress;

	RadioTap Radio = RadioTap();
	Dot11Deauthentication Deauth = Dot11Deauthentication();

	Deauth.addr1(ST);
	Deauth.addr2(AP);
	Deauth.addr3(Deauth.addr2());

	Radio.inner_pdu(Deauth);
	Sender.send(Radio,iface);

	usleep(10000);

	return Radio;
}

void DeauthDeamon::Run(string iface)
{
	RadioTap Packet;
	PacketSender Sender;
	
	redox::Redox rdx;

	vector<string> apList;
	vector<string> deviceList;
	vector<string> allAPList;

	int apListLength = 0;
	int deviceListLength = 0;
	int sizeAllAP = 0; 

	string apMac,dstMac;

	Dot11::address_type AP;
	Dot11::address_type ST;
	RadioTap Radio;
	Dot11Deauthentication Deauth;
	
	if(!rdx.connect("localhost",6379))
		return ;

	while(true)
	{
		apListLength = 0;
		deviceListLength = 0;
		apList.clear();
		deviceList.clear();
		allAPList.clear();

		auto &c = rdx.commandSync<int>({"LLEN","blackaplist"});
		if(c.ok()){
			apListLength = c.reply();
		}
		c.free();
		
		auto &c2 = rdx.commandSync<int>({"LLEN","blackdevicelist"});
		if(c2.ok()){
			deviceListLength = c2.reply();
		}
		c2.free();
		
		auto &container1 = rdx.commandSync<vector<string>>({"LRANGE","blackaplist","0",to_string(apListLength)});
		if(container1.ok()){
			for(const string &s : container1.reply())
				apList.push_back(s);	
		}
		container1.free();

		auto &container2 = rdx.commandSync<vector<string>>({"LRANGE","blackdevicelist","0",to_string(deviceListLength)});
		if(container2.ok()){
			for(const string &s : container2.reply())
				deviceList.push_back(s);	
		}
		container2.free();

		auto &container3 = rdx.commandSync<vector<string>>({"HGETALL","beaconlist"});
		if(container3.ok()){
			for(const string &s : container3.reply())
				allAPList.push_back(s);
		}
		container3.free();

		// Blacklist Device
		int sizeAllAP = apList.size(); 
		for(int apCount = 0;apCount<sizeAllAP;apCount++)
		{
			for(auto dstMac : deviceList){

				cout << "AP Address : " << apList[apCount] << endl;
				cout << "Station Address : " << dstMac << endl;

				AP = apList[apCount];
				ST = dstMac;

				Radio = RadioTap();
				Deauth = Dot11Deauthentication();

				Deauth.addr1(ST);
				Deauth.addr2(AP);
				Deauth.addr3(Deauth.addr2());

				Radio.inner_pdu(Deauth);
				Sender.send(Radio,iface);

				usleep(10000);

				//GenPacket(apList[apCount],dstMac,iface);
				//Sender.send(Packet,iface);
			}
		}
	}
	/*
	 * Blacklist AP
	Packet = GenPacket(apMac,"FF:FF:FF:FF:FF:FF");
	Sender.send(Packet,iface);
	*/
}

int main(int argc,char **argv)
{
	if(argc != 2){
		cout << "Usage : " << argv[0] << " <interface>" << endl;
		return 1;
	}

	string interface = argv[1];
	DeauthDeamon deamon;
	deamon.Run(interface);

}
