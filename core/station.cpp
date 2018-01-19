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

class StationSniffer{
public:
    void run(const string& iface);
private:
    typedef Dot11::address_type address_type;
    typedef set<address_type> stations_type;

    Redox rdx;

    stations_type stations;

    bool callback(PDU& pdu);

};

void StationSniffer::run(const std::string& iface)
{
    if(!rdx.connect())
	return ;

    rdx.del("devicelist");

    SnifferConfiguration config;
    config.set_promisc_mode(true);
    config.set_filter("type mgt subtype probe-resp");
    config.set_rfmon(true);
    Sniffer sniffer(iface,config);
    sniffer.sniff_loop(make_sniffer_handler(this,&StationSniffer::callback));
}

bool StationSniffer::callback(PDU& pdu)
{
    const Dot11ProbeResponse& probe = pdu.rfind_pdu<Dot11ProbeResponse>();
    string cmd = "";
    
    if(!probe.from_ds() && !probe.to_ds())
    {
        address_type addr = probe.addr1();

        stations_type::iterator it = stations.find(addr);

        if(it == stations.end())
        {
            try{
		cmd = "LPUSH devicelist " + addr.to_string();

                stations.insert(addr);
                
		rdx.commandSync(rdx.strToVec(cmd));
		
		//cout << "device : " << addr << endl;
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
	StationSniffer sniffer;
	sniffer.run(interface);

}
