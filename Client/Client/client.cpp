#include "network.h"

const USHORT DEFAULT_PORT = 100;
const PCSTR CLIENT_ADDRESS = "127.0.0.1";
const int BUFFER_LENGTH = 100;
const int NUMBER_OF_PACKETS = 700;

int numOfPacketsSent, numOfPacketsRec = 0;
std::string data = "";
char buffer[BUFFER_LENGTH];
clock_t start;

int main()
{		
	try
	{
		WSASession Session;
		UDPSocket Socket;

		start = std::clock();

		for (int i = 0; i < NUMBER_OF_PACKETS; i++)
		{
			data = std::to_string((std::clock() - start) / (double)CLOCKS_PER_SEC);// std::to_string(i);

			Socket.SendTo(CLIENT_ADDRESS, DEFAULT_PORT, data.c_str(), data.size());
			numOfPacketsSent++;

			Socket.RecvFrom(buffer, DEFAULT_PORT);
			std::cout << buffer << "\n";
		}

		numOfPacketsRec = Socket.RecieveCount();

	}
	catch (std::exception &ex)
	{
		std::cout << ex.what();
	}

	std::cout << "\n" << "Percentage of Packets Lost: " << std::to_string(PercentPackageLoss(numOfPacketsSent, numOfPacketsRec)); //added

	char c;
	std::cin >> c;
}

float PercentPackageLoss(int sent, int rec)
{
	float percent = 100.0f - (rec / sent * 100.0f);

	return percent;
}