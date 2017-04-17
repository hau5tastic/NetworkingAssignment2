#include "network.h"

const USHORT DEFAULT_PORT = 100;
const PCSTR SERVER_ADDRESS = "127.0.0.1";
const int NUMBER_OF_PACKETS = 500;
const int BUFFER_LENGTH = 100;

/***************************************
* IP Header Codes
****************************************/
const int HEADER_CODE_SIZE = 3;
const uint8_t CONNECTION_REQUEST = 201;
const uint8_t CONNECTION_ACCEPTED = 202;
const uint8_t CONNECTION_REFUSED = 203;
const uint8_t PACKET_STREAM = 204;

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
			Listen(Socket);

			std::cout << "Awaiting Input: ";
			std::cin >> serverOptions;

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

// asks server if they want to accept incoming connection
void Listen(UDPSocket Socket)
{
	try
	{
		sockaddr_in addr = Socket.RecvFrom(buffer, sizeof(buffer));

		std::cout	<< "Incoming connection from "
					<< std::to_string(addr.sin_addr.S_un.S_un_b.s_b1) << '.'
					<< std::to_string(addr.sin_addr.S_un.S_un_b.s_b2) << '.'
					<< std::to_string(addr.sin_addr.S_un.S_un_b.s_b3) << '.'
					<< std::to_string(addr.sin_addr.S_un.S_un_b.s_b4)
					<< ", Accept Connection? (y/n) ";

		char input;
		std::cin >> input;

		std::string answer = "";

		if (input == 'y' || input == 'yes')
		{
			connectedClients.push_back(addr);
			answer = std::to_string(CONNECTION_ACCEPTED);
			std::cout << "IP Address Added. Connection Accepted. \n";
			Socket.SendTo(addr, answer.c_str(), answer.size());
			SendAndRecievePackets(Socket);
		}
		else if (input == 'n' || input == 'no')
		{
			std::cout << "Connection Refused. \n";
			answer = std::to_string(CONNECTION_REFUSED);
			Socket.SendTo(addr, answer.c_str(), answer.size());
		}
		else
		{
			std::cout << "Invalid Input. \n";
			answer = std::to_string(CONNECTION_REFUSED);
			Socket.SendTo(addr, answer.c_str(), answer.size());
		}
	}
	catch (std::system_error& e)
	{
		//std::cout << e.what();
	}
}

// sends and recieves stream of packets
void SendAndRecievePackets(UDPSocket Socket)
{
	//Socket.totalSent = 0;

	for (int i = 0; i < NUMBER_OF_PACKETS; ++i)
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
	}

	LogPacketInfo(Socket);
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

	for (std::vector<sockaddr_in>::iterator it = connectedClients.begin(); it < connectedClients.end(); ++it)
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
}

// parses the header and returns the header as a unit8
uint8_t GetHeaderCode(char* buffer)
{
	std::string answer = std::string(buffer);
	int ans = 0;

	for (int i = 0, j = 100; i < HEADER_CODE_SIZE; ++i, j /= 10)
	{
		ans += (answer[i] - '0') * j;
	}

	return uint8_t(ans);
}