#include "tcpsocket.h"

#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

using std::string;

int main() {
    LOG(INFO) << "start testing" << "!";
    const char * msg = "0123456789";
    int pid = fork();
    if (pid > 0) { //parent: server
        InitializeLogger("../logs/server_info.log", "../logs/server_warn.log", "../logs/server_erro.log");

        TCPSocket server;
        TCPSocket client;

        string cl_ip;
        short cl_port;
        char serbuff[10];
        memset(serbuff, '\0', 10);

        LOG(INFO) << "bing-listen-accept" << "!";

        server.Bind("127.0.0.1", 11223);
        server.Listen(3);
        server.Accept(&client, &cl_ip, &cl_port);

        LOG(INFO) << "connection established" << "!";

        int tmp;

        int recieved_bytes = 0;

        while (recieved_bytes < 10) {
          tmp = client.Receive(&serbuff[recieved_bytes], 10);
          recieved_bytes += tmp;
        }

        LOG(INFO) << "received [" << recieved_bytes << "] bytes: " << string(serbuff,10) << "!";

        int sent_bytes = 0;
        while (sent_bytes < 10) {
          tmp = client.Send(&msg[sent_bytes], 3);
          sent_bytes += tmp;
        }
        LOG(INFO) << "sent [" << sent_bytes << "] bytes!";

    } else { // child: client
        InitializeLogger("../logs/client_info.log", "../logs/client_warn.log", "../logs/client_erro.log");
        sleep(3);   // wait for server
        TCPSocket client;
        client.Connect("127.0.0.1", 11223);
        char clibuff[10];
        memset(clibuff, '\0', 10);

        int tmp;
        int sent_bytes = 0;
        while (sent_bytes < 10) {
          tmp = client.Send(&msg[sent_bytes], 3);
          sent_bytes += tmp;
        }
        LOG(INFO) << "client sent: [" << sent_bytes << "] bytes!";
        int recieved_bytes = 0;
        while (recieved_bytes < 10) {
          tmp = client.Receive(&clibuff[recieved_bytes], 10);
          recieved_bytes += tmp;
        }
        LOG(INFO) << "client received: [" << recieved_bytes << "] bytes!";
        LOG(INFO) << "client received: [" << string(clibuff,10) << "]!";
    }
    wait(0);
    LOG(INFO) << "end testing" << "!";

return 0; }

