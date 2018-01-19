#include <iostream>
#include <set>
#include <string>
#include <tins/tins.h>
#include <iostream>
#include "redox.hpp"
#include "utility.hpp"

using std::string;
using std::set;
using std::endl;
using std::cout;
using std::runtime_error;

using namespace Tins;
using namespace redox;
using namespace Utility;

class BeaconSniffer{
public:
    void run(const string& iface);
private:
    typedef Dot11::address_type address_type;
    typedef set<address_type> ssids_type;

    Redox rdx;

    ssids_type ssids;

    bool callback(PDU& pdu);

};

void BeaconSniffer::run(const std::string& iface)
{
    if(!rdx.connect())
	return ;

    rdx.del("beaconlist");

    SnifferConfiguration config;
    config.set_promisc_mode(true);
    config.set_filter("type mgt subtype beacon");
    config.set_rfmon(true);
    Sniffer sniffer(iface,config);
    sniffer.sniff_loop(make_sniffer_handler(this,&BeaconSniffer::callback));
}

bool BeaconSniffer::callback(PDU& pdu)
{
    const Dot11Beacon& beacon = pdu.rfind_pdu<Dot11Beacon>();
    string cmd = "";

    if(!beacon.from_ds() && !beacon.to_ds())
    {
        address_type addr = beacon.addr2();

        ssids_type::iterator it = ssids.find(addr);

        if(it == ssids.end())
        {
            try{
		string ssid = beacon.ssid();
                
		if(ssid.size() != 0)
			cmd = "HMSET beaconlist " + addr.to_string() + " " + Utility::Base64Encoding(ssid);
		else
			cmd = "HMSET beaconlist " + addr.to_string() + " " + Utility::Base64Encoding("(hidden ssid beacon)");

                ssids.insert(addr);
                
		rdx.commandSync(rdx.strToVec(cmd));

            }
            catch(runtime_error&){

            }
        }
    }
    return true;
}

int main(int argc,char **argv)
{
	if(argc != 2){
		cout << "Usage : " << argv[0] << " <interface>" << endl;
		return 1;
	}

	string interface = argv[1];
	BeaconSniffer sniffer;
	sniffer.run(interface);

}
