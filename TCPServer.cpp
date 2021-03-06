/*
 * Robot Navigation Program
 * www.robotnav.com
 *
 * (C) Copyright 2010 - 2014 Lauro Ojeda
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <cmath>
#include "Odometry.h"
#include "Archer.h"

 //TCP
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include "tcpconnector.h"
#include "tcpacceptor.h"
#include "TCPServer.h"

using namespace std;

TCPServer::TCPServer(char* pserver, int pport)
{
  server = pserver;
  port = pport;
}

void TCPServer::makeServer()
{
  acceptor = new TCPAcceptor(port, server);
  if (acceptor->start() == 0) {
    stream = acceptor->accept();
  }
}

void TCPServer::makeClient()
{
  connector = new TCPConnector();
  stream = connector->connect(server, port);
}

void TCPServer::sndMessage(string strmes)
{
	//size_t buffsize;
	//buffsize = 256;
    //char str1 [buffsize];    //char line[256];
    if (stream) {
      // 	sprintf(str1, "Current: %d X: %f Y: %f Speed: %f", archSensors->mCurrent, mX, mY, mSpeed);
      printf("Sending - %s\n", strmes.c_str());
    	stream->send(strmes.c_str(), strmes.size());
     	printf("sent - %s\n", strmes.c_str());
      	//len = stream->receive(line, sizeof(line));
      	//line[len] = 0;
      	//printf("received - %s\n", line);
    }
}

string TCPServer::rcvMessage()
{
  int len;
  char message[256];
  string strrcv = "NoMsg";
  if (stream) {
    len = stream->receive(message, sizeof(message), 100000);
    message[len] = 0;
    printf("received - %s\n", message);
    strrcv = string(message);
  }
  return strrcv;
}

void TCPServer::closeConn()
{
    delete stream;
}