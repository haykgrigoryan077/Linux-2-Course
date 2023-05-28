#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1)
  {
    std::cerr << "Failed to create socket." << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string serverIP = "127.0.0.1";
  int serverPort = 8080;

  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(serverPort);
  inet_pton(AF_INET, serverIP.c_str(), &(serverAddress.sin_addr));

  if (connect(clientSocket, (struct sockaddr *)(&serverAddress), sizeof(serverAddress)) == -1)
  {
    std::cerr << "Failed to connect to the server." << std::endl;
    close(clientSocket);
    exit(EXIT_FAILURE);
  }

  std::cout << "What do you want to do?" << std::endl
            << "Here are the options:" << std::endl
            << "0: ADD" << std::endl
            << "1: SUBTRACT" << std::endl
            << "2: MULTIPLY" << std::endl
            << "3: DIVIDE" << std::endl;

  int userDecisionID;
  std::cin >> userDecisionID;

  int firstNumber, secondNumber;
  std::cout << "Please enter two numbers:" << std::endl;
  std::cin >> firstNumber >> secondNumber;

  std::string message = std::to_string(userDecisionID) + " " + std::to_string(firstNumber) + " " + std::to_string(secondNumber);

  if (send(clientSocket, message.c_str(), message.size(), 0) == -1)
  {
    std::cerr << "Failed to send data to the server." << std::endl;
    close(clientSocket);
    exit(EXIT_FAILURE);
  }

  char buffer[1024];
  int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
  if (bytesRead == -1)
  {
    std::cerr << "Failed to receive data from the server." << std::endl;
    close(clientSocket);
    exit(EXIT_FAILURE);
  }
  else if (bytesRead == 0)
  {
    std::cout << "Server disconnected." << std::endl;
    close(clientSocket);
    exit(EXIT_SUCCESS);
  }

  buffer[bytesRead] = '\0';
  std::cout << "Result received from server: " << buffer << std::endl;

  close(clientSocket);

  return 0;
}
