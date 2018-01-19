#include <string>
#include "utility.hpp"

using namespace std;

namespace Utility
{
	string Base64Encoding(string input)
	{
		static string table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		
		int size = input.size();
		string result = "";
		
		for(int i=0;i<size;i+=3)
		{
			unsigned int tmp = 0;
			
			if(i<size)
				tmp |= ((unsigned int)input[i]) << 16;
			if(i+1<size)
				tmp |= (((unsigned int)input[i+1]) << 8);
			if(i+2<size)
				tmp |= (((unsigned int)input[i+2]) & 0xFF);

			if(i<size){
				result += table[(tmp >> 18) & 0x3F];
				result += table[(tmp >> 12) & 0x3F];
			}
			
			if(i+1<size)
				result += table[(tmp >> 6) & 0x3F];
				
			if(i+2<size)
				result += table[(tmp & 0x3F)];
		}
		
		if(size%3==1)
			result += "==";
		else if(size%3==2)
			result += "=";
			
		return result;
	}
}

/*
int main() {
	
	string test;
	
	cin >> test;
	
	cout << Utility::Base64Encoding(test);
	
	return 0;
}
*/
