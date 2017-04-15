#include "network.h"

const USHORT DEFAULT_PORT = 100;
const PCSTR SERVER_ADDRESS = "127.0.0.1";
const int NUMBER_OF_PACKETS = 700;
const int BUFFER_LENGTH = 100;

char buffer[BUFFER_LENGTH];
std::string serverOptions;

int main()
{

	try
	{
		WSASession Session;
		UDPSocket Socket;

		Socket.Bind(DEFAULT_PORT);
		while (serverOptions != "exit")
		{
			sockaddr_in add = Socket.RecvFrom(buffer, sizeof(buffer));
			std::string input(buffer);

			std::cout << "Packet Recieved " << buffer << " at " << std::clock() << "\n";
			Socket.SendTo(add, input.c_str(), input.size());

			
			//This if statement is making it so that the server cannot accept multiple clients
			if (Socket.totalRecieved >= NUMBER_OF_PACKETS)
			{		
				LogPacketInfo(Socket);
				Socket.totalRecieved = 0;
				std::cin >> serverOptions;
			}
			
		}


	}
	catch (std::system_error& e)
	{
		std::cout << e.what();
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