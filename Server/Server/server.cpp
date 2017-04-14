#include "network.h"

const USHORT DEFAULT_PORT = 100;
const PCSTR SERVER_ADDRESS = "127.0.0.1";
const int NUMBER_OF_PACKETS = 700;
const int BUFFER_LENGTH = 100;

char buffer[BUFFER_LENGTH];
std::string serverOptions;

int main()
{
	//http://stackoverflow.com/questions/3220477/how-to-use-clock-in-c

	//int numOfPacketsSent = 700;
	//int numOfPacketsRec = 0;

	try
	{
		WSASession Session;
		UDPSocket Socket;


		Socket.Bind(DEFAULT_PORT);
		while (serverOptions != "exit")
		{
			sockaddr_in add = Socket.RecvFrom(buffer, sizeof(buffer));
			std::string input(buffer);

			//std::cout << buffer << "  " << std::clock() << "\n";
			Socket.SendTo(add, input.c_str(), input.size());

			if (Socket.totalRecieved >= NUMBER_OF_PACKETS)
			{
				std::cin >> serverOptions;
			}
		}

		LogPacketInfo(Socket);
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
	std::cout << "Total Packets Recieved: " << Socket.totalRecieved << "\n";
	std::cout << "Total Packets Sent: " << Socket.totalSent << "\n";
	std::cout << "Percentage Of Packets Lost: " << std::to_string(PercentPackageLoss(Socket.totalSent, Socket.totalRecieved)) << "\n";
	std::cout << "Average Ping: " << Socket.AveragePing() << "\n";
}