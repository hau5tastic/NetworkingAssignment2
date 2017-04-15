#include "network.h"

const USHORT DEFAULT_PORT = 100;
const PCSTR SERVER_ADDRESS = "127.0.0.1";
const int NUMBER_OF_PACKETS = 700;
const int BUFFER_LENGTH = 100;

char buffer[BUFFER_LENGTH];
std::string serverOptions;
clock_t start;
std::vector<sockaddr_in> connectedClients;

int main()
{
	try
	{
		start = std::clock();
		WSASession Session;
		UDPSocket Socket;

		Socket.Bind(DEFAULT_PORT);
		while (serverOptions != "exit")
		{
			// recieve
			sockaddr_in add = Socket.RecvFrom(buffer, sizeof(buffer));

			std::cout << "Packet Recieved from "
				<< std::to_string(add.sin_addr.S_un.S_un_b.s_b1) << '.'
				<< std::to_string(add.sin_addr.S_un.S_un_b.s_b2) << '.'
				<< std::to_string(add.sin_addr.S_un.S_un_b.s_b3) << '.'
				<< std::to_string(add.sin_addr.S_un.S_un_b.s_b4)
				<< " at " << GetTime() << " - " << buffer << "\n";

			// send
			std::string input = GetTime();
			Socket.SendTo(add, input.c_str(), input.size());

			//This if statement is making it so that the server cannot accept multiple clients
			if (Socket.totalRecieved >= NUMBER_OF_PACKETS)
			{
				LogPacketInfo(Socket);
				Socket.totalRecieved = 0;
				std::cout << "Enter Input: ";
				std::cin >> serverOptions;
			}
			
			if (serverOptions == "sort")
			{
				Socket.GetSortedPingTimes();
				serverOptions = "";
			}
		}
	}
	catch (std::system_error& e)
	{
		std::cout << e.what();
	}

	std::cout << "Shutting down server.";
	char c;
	std::cin >> c;
	return 0;
}

// calculate packet loss
float PercentPackageLoss(int sent, int rec)
{
	float percent = 100.0f - (rec / sent * 100.0f);
	return percent;
}

// logs packet info to console window
void LogPacketInfo(UDPSocket Socket)
{
	std::cout << "\n" << "Total Packets Recieved: " << Socket.totalRecieved << "\n";
	std::cout << "Total Packets Sent: " << Socket.totalSent << "\n";
	std::cout << "Percentage Of Packets Lost: " << std::to_string(PercentPackageLoss(Socket.totalSent, Socket.totalRecieved)) << "\n";
	std::cout << "Average Ping: " << Socket.AveragePing() << "\n";
}

// return elapsed time since start in milliseconds
std::string GetTime()
{
	return std::to_string((int)round((std::clock() - start) / (double)CLOCKS_PER_SEC * 1000));
}

/*// asks server if they want to accept incoming connection
void AcceptClientConnection(sockaddr_in addr)
{
	std::cout	<< "Incoming connection from "
				<< std::to_string(addr.sin_addr.S_un.S_un_b.s_b1) << '.'
				<< std::to_string(addr.sin_addr.S_un.S_un_b.s_b2) << '.'
				<< std::to_string(addr.sin_addr.S_un.S_un_b.s_b3) << '.'
				<< std::to_string(addr.sin_addr.S_un.S_un_b.s_b4)
				<< ", Accept Connection? (y/n) ";

	char input;
	std::cin >> input;

	if (input == 'y' || input == 'yes')
	{
		connectedClients.push_back(addr);
		std::cout << "IP Address Added. \n";
	}
	else if (input == 'n' || input == 'no')
	{
		std::cout << "Connection Refused. \n";
	}
	else
	{
		std::cout << "Invalid Input. \n";
	}

}

// checks if the connected ip address has been approved for connection
bool AcceptedAddress(sockaddr_in addr)
{
	bool result = false;
	for (auto &address : connectedClients)
	{
		if(addr.sin_addr.S_un.S_addr == address.sin_addr.S_un.S_addr)
		{
			result = true;
		}
	}
	return result;
}

// searches for and removes client from accepted client list
void RemoveAddress(sockaddr_in addr)
{
	bool addrRemoved = false;

	for (std::vector<sockaddr_in>::iterator it = connectedClients.begin(); it < connectedClients.end(); it++)
	{
		if (it->sin_addr.S_un.S_addr == addr.sin_addr.S_un.S_addr)
		{
			connectedClients.erase(it);
			addrRemoved = true;
			break;
		}
	}

	if (!addrRemoved)
	{
		std::cout << "Could not find requested address in connected client list. \n";
	}
}*/