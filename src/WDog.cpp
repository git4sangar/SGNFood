/*sgn
 * WDog.cpp
 *
 *  Created on: 29-May-2020
 *      Author: tstone10
 */


//sgn
#include <sys/types.h>
#include <signal.h>
#include <thread>
#include <iostream>
#include <time.h>
#include <dirent.h>
#include <iterator>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>
#include <vector>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

#define BASE_BOT_BIN_PATH   "/home/svaithiy/sgn/projs/SGNBot/build/"
#define BASE_BOT_BIN_FILE   "BaseBot"

#define WDOG_ROOT_PATH  "/home/svaithiy/sgn/projs/SGNBot/chkkboys/"
#define WDOG_LOG_PATH   "logs/"
#define WDOG_LOG_FILE   "wdoglog_file.log"

#define MAX_TIMEOUT_INTERVAL_SECS   (10)
#define MAX_WAKEUP_INTERVAL_SECS    (5)

#define LISTEN_PORT (4950)
#define BUFSIZE     (1024)

// Stringify param x.
// Step (01): Replaces the pattern MAKE_STR(x) with MAKE_STR(value-of-x)
#define MAKE_STR(x)  _MAKE_STR(x)
// Step (02): Replaces the pattern _MAKE_STR(value-of-x) with quotes-added-param, ie "value-of-x"
#define _MAKE_STR(x) #x          // Adds quotes to the param

unsigned int last_time_stamp = 0;

void watch_dog_thread(FILE *fp) {
   std::string strBaseBotRoot = std::string(BASE_BOT_BIN_PATH);
   std::string strBaseBotFile = std::string(BASE_BOT_BIN_FILE);
   std::string strBaseBotBin  = strBaseBotRoot + strBaseBotFile;

   //  The command is "pidof -s SoapSeller"
   std::string pid_of_cmd  = std::string("pidof -s ") + strBaseBotFile;

   while(true) {
      fprintf(fp,  "WatchDogServer %ld: Last time stamp: %d, Cur Time: %ld, Delta: %ld\n",
                time(0), last_time_stamp, time(0), (time(0) - last_time_stamp)); fflush(fp);
      if(MAX_TIMEOUT_INTERVAL_SECS <= (time(0) - last_time_stamp)) {
        fprintf(fp, "WatchDogServer %ld: time elapsed, so killing %s\n", time(0), strBaseBotFile.c_str());
        char buf[512];
        FILE *cmd_pipe = popen(pid_of_cmd.c_str(), "r");
        fgets(buf, 512, cmd_pipe);
        pid_t pid = strtoul(buf, NULL, 10);
        pclose( cmd_pipe );
        if(0 < pid) {
        fprintf(fp, "WatchDogServer %ld: killing process pid %d\n", time(0), pid); fflush(fp);
           kill(pid, 9);
        } else {
           fprintf(fp, "WatchDogServer %ld: Invalid pid %d. Seems %s is not running\n",
                time(0), pid, strBaseBotFile.c_str()); fflush(fp);
        }
        fprintf(fp, "WatchDogServer %ld: Starting BaseBot %s\n", time(0), strBaseBotBin.c_str()); fflush(fp);
        sleep(1);
        std::system(strBaseBotBin.c_str());
        last_time_stamp = time(0);
      }
      sleep(MAX_WAKEUP_INTERVAL_SECS);
   }
}

int sock_receive_thread(FILE *fp) {
   int sockfd;
   sockfd = socket(AF_INET, SOCK_DGRAM, 0);

   int optval;
   setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

   struct sockaddr_in serveraddr;
   bzero((char *) &serveraddr, sizeof(serveraddr));
   serveraddr.sin_family = AF_INET;
   serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
   serveraddr.sin_port = htons((unsigned short)LISTEN_PORT);

   bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
   fprintf(fp, "WatchDogServer %ld: Socket bound to port %d\n", time(0), LISTEN_PORT); fflush(fp);

   struct sockaddr_in clientaddr;
   int clientlen, recvd;
   char buf[BUFSIZE];
   fprintf(fp, "WatchDogServer %ld: Starting loop\n", time(0)); fflush(fp);
   while(1) {
      bzero(buf, BUFSIZE);
      recvd = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *) &clientaddr, (socklen_t*)&clientlen);
      last_time_stamp = std::stoul(std::string(buf));
      fprintf(fp, "WatchDogServer %ld: WatchDog petted at: %d\n", time(0), last_time_stamp); fflush(fp);
   }
   return 0;
}

int main(void) {
   pid_t pid;

   // Fork the current process
   pid = fork();

   // The parent process continues with a process ID greater than 0
   if(pid > 0) {
      std::cout << "Exiting parent pid " << getpid() << std::endl;
      exit(EXIT_SUCCESS);
   } else if(pid < 0) {
      // A process ID lower than 0 indicates a failure in either process
      exit(EXIT_FAILURE);
   }

   // Now on it child process
   std::string wdog_log_path        = std::string(WDOG_ROOT_PATH) + std::string(WDOG_LOG_PATH);
   std::string wdog_log_file        = WDOG_LOG_FILE;
   std::string wdog_log_with_path   = wdog_log_path + wdog_log_file;
   FILE *fp = fopen(wdog_log_with_path.c_str(), "w");

   fprintf(fp, "WatchDogServer: Successfully started\n"); fflush(fp);

   last_time_stamp = time(0);
   std::thread recv_thread(&sock_receive_thread, fp);
   std::thread wdog_thread(&watch_dog_thread, fp);

   recv_thread.join();
   wdog_thread.join();

   // Close system logs for the child process
   fprintf(fp, "WatchDogServer: Stopping...\n"); fflush(fp);
   fclose(fp);

   // Terminate the child process when the daemon completes
   exit(EXIT_SUCCESS);
}


