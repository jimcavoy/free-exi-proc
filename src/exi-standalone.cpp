/*
    free-exi-proc - an EXI (= "Efficient XML Interchange") processor
    Copyright (C) 2023 Tomas Prerovsky <cepsdev@hotmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <chrono>
#include <sstream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <map>
#include <algorithm>
#include <future>
#include <netinet/sctp.h> 
#include "ceps_ast.hh"
#include "v2g-guru-exi.h"
#include "free-exi-terminal.h"
#include "free-exi-proc.h"


using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cerr << "Usage: v2g-guru-exi file" << endl;
        return -1;
    }

    ifstream xmlFile(argv[1], ios::in|ios::ate);

    if (!xmlFile.is_open())
    {
        cerr << "ERROR: Failed to open file: " << argv[1] << endl;
    }

    streampos size = xmlFile.tellg();
    char* buff = new char[size];
    memset(buff, 0, size);
    xmlFile.seekg(0, ios::beg);
    xmlFile.read(buff, size);
    xmlFile.close();    

    v2g_guru_exi::Processor exiProc;

    delete [] buff;

    return 0;
}