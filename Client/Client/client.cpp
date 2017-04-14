#include "network.h"

const USHORT DEFAULT_PORT = 100;
const PCSTR CLIENT_ADDRESS = "127.0.0.1";
const int NUMBER_OF_PACKETS = 700;
const int BUFFER_LENGTH = 100;

int numOfPacketsSent, numOfPacketsRec = 0;
std::string data = "";
char buffer[BUFFER_LENGTH];
clock_t start;
std::string clientInput;

int main()
{		
	try
	{
		WSASession Session;
		UDPSocket Socket;

		start = std::clock();

		SendAndRecievePackets(Socket);

		numOfPacketsRec = Socket.RecieveCount();

		LogPacketInfo(Socket);
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what();
	}

	char c;
	std::cin >> c;
}

float PercentPackageLoss(int sent, int rec)
{
	float percent = 100.0f - (rec / sent * 100.0f);

	return percent;
}

void LogPacketInfo(UDPSocket Socket)
{
	std::cout << "\n" << "Total Packets Recieved: " << Socket.totalRecieved << "\n";
	std::cout << "Total Packets Sent: " << Socket.totalSent << "\n";
	std::cout << "Percentage Of Packets Lost: " << std::to_string(PercentPackageLoss(Socket.totalSent, Socket.totalRecieved)) << "\n";
	std::cout << "Average Ping: " << Socket.AveragePing() << "\n";
}

void SendAndRecievePackets(UDPSocket Socket)
{
	for (int i = 0; i < NUMBER_OF_PACKETS; i++)
	{
		data = std::to_string((int)round((std::clock() - start) / (double)CLOCKS_PER_SEC * 1000)); // milliseconds
		std::cout << "Packet " << i << " Sent at: " << data << "\n"; // milliseconds

		Socket.SendTo(CLIENT_ADDRESS, DEFAULT_PORT, data.c_str(), data.size());
		numOfPacketsSent++;

		Socket.RecvFrom(buffer, DEFAULT_PORT);
		std::cout << "Packet Recieved at: " << buffer << "\n";
	}
}