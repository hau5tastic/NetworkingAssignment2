#include "network.h"

const USHORT DEFAULT_PORT = 100;
const int NUMBER_OF_PACKETS = 700;
const int BUFFER_LENGTH = 100;

std::string serverIP;
std::string data = "";
char buffer[BUFFER_LENGTH];
clock_t start;
std::string clientInput;

int main()
{
	try
	{
		start = std::clock();
		WSASession Session;
		UDPSocket Socket;

		HandleClientInput();
		SendAndRecievePackets(Socket);
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what();
	}

	std::cout << "Shutting down client.";
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
	std::cout << "\n" << "Total Packets Recieved: " << Socket.totalRecieved  << "\n";
	std::cout << "Total Packets Sent: " << Socket.totalSent << "\n";
	std::cout << "Percentage Of Packets Lost: " << std::to_string(PercentPackageLoss(Socket.totalSent, Socket.totalRecieved)) << "\n";
	std::cout << "Average Ping: " << Socket.AveragePing() << "\n";
}

// sends and recieves stream of packets
void SendAndRecievePackets(UDPSocket Socket)
{
	for (int i = 0; i < NUMBER_OF_PACKETS; i++)
	{
		data = GetTime();
		std::cout << "Packet " << i + 1 << " Sent at: " << data << "\n"; // milliseconds

		Socket.SendTo(serverIP, DEFAULT_PORT, data.c_str(), data.size());

		Socket.RecvFrom(buffer, DEFAULT_PORT);
		std::cout << "Packet Recieved at: " << buffer << "\n";
	}

	LogPacketInfo(Socket);
}

// return elapsed time since start in milliseconds
std::string GetTime()
{
	return std::to_string((int)round((std::clock() - start) / (double)CLOCKS_PER_SEC * 1000));
}

// handles client input for connection to server
void HandleClientInput()
{
	bool getIP = false;

	while (!getIP)
	{
		std::cout << "Please Enter the IP Address of the server you wish to connect to: ";
		std::cin >> serverIP;

		if (ValidateIP(serverIP))
		{
			getIP = true;
		}
		else
		{
			std::cout << "IP Invalid, ";
		}
	}

}

// helper funcs for IP validation
bool CheckIfNumber(int key)
{
	if (key >= 48 && key <= 57) // ascii keycodes for numbers 0-9
		return true;
	else
		return false;
}

bool CheckPeriod(int key)
{
	if (key == 46) // ascii code for '.'
		return true;
	else
		return false;
}

// makes sure the entered ip is valid
bool ValidateIP(std::string ip)
{
	bool result = true;

	if (ip.length() <= 15)
	{
		bool canBePeriod = false;
		int periodCount = 0;
		int numCount = 0;

		for (int i = 0; i < ip.length(); ++i)
		{
			int character = ip[i];

			if (CheckIfNumber(character) && numCount < 3)
			{
				canBePeriod = true;
				numCount++;
			}
			else if (canBePeriod && CheckPeriod(character) && periodCount < 3)
			{
				canBePeriod = false;
				periodCount++;
				numCount = 0;
			}
			else
			{
				result = false;
			}
		}

		if (periodCount != 3)
			result = false;
	}
	else
	{
		result = false;
	}

	return result;
}