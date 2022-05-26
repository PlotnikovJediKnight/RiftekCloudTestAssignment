#include <iostream>
using namespace std;

//Swap two bytes
uint16_t task0(uint16_t arg) {
	return (arg << 8) | (arg >> 8);
}

//Power of 2
size_t task1(size_t power) {
	return 1 << power;
}

//Get broadcast IP address
uint32_t task2(uint32_t address, uint32_t mask) {
	if (address == 0) 
		throw runtime_error("Encountered any-network IP address!");

	if ((address & 0xFFFFFFFF) == 0xFFFFFFFF)
		throw runtime_error("Encountered limited broadcast IP address!");

	uint32_t save = mask;
	uint32_t saveShiftBitMask = 0x1;
	size_t shiftCount = 0;
	while ((save & saveShiftBitMask) == 0) {
		if (shiftCount == 32) 
			throw runtime_error("Incorrect mask format!");
		save >>= 1;
		shiftCount++;
	}

	size_t onesCount = 0;
	while (save > 0) {
		if ((save & saveShiftBitMask) == 0) 
			throw runtime_error("Incorrect mask format!");
		else {
			onesCount++;
		}
		save >>= 1;
	}

	if (onesCount != 32-shiftCount)
		throw runtime_error("Incorrect mask format!");

	uint32_t inverseMask = ~mask;
	uint32_t networkAddress = address & mask;
	uint32_t hostAddress = address & inverseMask;

	if (hostAddress == inverseMask)
		throw runtime_error("Host address cannot be 1s only!");

	if ((hostAddress & inverseMask) == 0)
		throw runtime_error("Host address cannot be 0s only!");

	if (networkAddress == mask)
		throw runtime_error("Network address cannot be 1s only!");

	if ((networkAddress & mask) == 0)
		throw runtime_error("Network address cannot be 0s only!");

	return networkAddress + inverseMask;
}

int main() {

	uint32_t broadcast = task2(0xC0A86419, 0xFFF00000);
	cout << ((broadcast & 0xFF000000) >> 24) << "."
		 << ((broadcast & 0x00FF0000) >> 16) << "."
		 << ((broadcast & 0x0000FF00) >> 8) << "."
		 <<  (broadcast & 0x000000FF) << endl;

	return 0;
}