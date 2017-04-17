#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <system_error>
#include <string>
#include <iostream>

#include <ctime> //Added
#include <vector> //Added
#include <numeric> //Added
#include <algorithm>

#pragma once

// prototyping
class UDPSocket;
float PercentPackageLoss(int sent, int rec);
void LogPacketInfo(UDPSocket Socket);
std::string GetTime();
void Listen(UDPSocket Socket);
void SendAndRecievePackets(UDPSocket Socket);
bool AcceptedAddress(sockaddr_in addr);
void RemoveAddress(sockaddr_in addr);
uint8_t GetHeaderCode(char* buffer);

struct TimeStamp
{
	int index;
	int time;

	bool operator<(const TimeStamp& a) const
	{
		return time < a.time;
	}
};

class WSASession
{
public:
	WSASession()
	{
		int ret = WSAStartup(MAKEWORD(2, 2), &data);
		if (ret != 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "WSAStartup Failed");
	}
	~WSASession()
	{
		WSACleanup();
	}
private:
	WSAData data;
};

class UDPSocket
{
public:
	int totalRecieved = 0; // Added
	int totalSent = 0; //Added
	std::vector<int> timeStamp; //Added

	UDPSocket()
	{
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock == INVALID_SOCKET)
			throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
	}
	~UDPSocket()
	{
		closesocket(sock);
	}

	void SendTo(const std::string& address, unsigned short port, const char* buffer, int len, int flags = 0)
	{
		sockaddr_in add;
		add.sin_family = AF_INET;
		add.sin_addr.s_addr = inet_addr(address.c_str());
		add.sin_port = htons(port);
		int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&add), sizeof(add));

		totalSent++; //Added

		if (ret < 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
	}
	void SendTo(sockaddr_in& address, const char* buffer, int len, int flags = 0)
	{
		int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&address), sizeof(address));

		totalSent++; //Added

		if (ret < 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
	}
	sockaddr_in RecvFrom(char* buffer, int len, int flags = 0)
	{
		sockaddr_in from;
		int size = sizeof(from);
		int ret = recvfrom(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&from), &size);
		if (ret < 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");

		//timeStamp.push_back(std::clock());

		totalRecieved++; // Added

						 // make the buffer zero terminated
		buffer[ret] = 0;
		return from;
	}
	void Bind(unsigned short port)
	{
		sockaddr_in add;
		add.sin_family = AF_INET;
		add.sin_addr.s_addr = htonl(INADDR_ANY);
		add.sin_port = htons(port);

		int ret = bind(sock, reinterpret_cast<SOCKADDR *>(&add), sizeof(add));
		if (ret < 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "Bind failed");
	}

	double AveragePing()
	{
		std::vector<double> difference;

		for (int i = 1; i < timeStamp.size(); i++)
		{
			difference.push_back(timeStamp[i] - timeStamp[i - 1]);
		}

		int sum = std::accumulate(difference.begin(), difference.end(), 0);
		double average = sum / difference.size();
		return average;
	}

	void GetSortedPingTimes()
	{
		std::vector<TimeStamp> difference;
		TimeStamp tmp;

		for (int i = 1; i < timeStamp.size(); i++)
		{
			tmp.index = i;
			tmp.time = timeStamp[i] - timeStamp[i - 1];
			difference.push_back(tmp);
		}

		std::sort(difference.begin(), difference.end());

		std::cout << "Sorted Ping Times: \n";
		for (auto iter = difference.begin(); iter != difference.end(); ++iter)
		{
			std::cout << "Packet - " << iter->index << " Time - " << iter->time << std::endl;
		}
	}

private:
	SOCKET sock;
};