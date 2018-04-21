//
//
// Author : huziang
// This is a thread function to store data into data engine

#ifndef MDSERVER_DATAENGINETHREAD_H_
#define MDSERVER_DATAENGINETHREAD_H_

#include <csignal>
#include <fstream>
#include "tsdatastruct.h"
#include "dataparse.h"
#include "mongodbengine.h"
#include "logger.h"
#include "messagequeue.h"
#include "timer.h"
using std::map;
using std::vector;


#ifdef DEBUG
extern unsigned long long recvtime[TIMES];
extern unsigned long long sendtime[TIMES];
static long timenum = 0;
unsigned long long savetime[TIMES];
#endif

MessageQueue *que;
extern DataEngine *db;
extern Logger *logger;
vector<map<string, string>> mds;
map<string, string> md;

void writeDataEngine(TSMarketDataField *pDepthMarketData) {
  // to document
  md.clear();
  parseFrom(md, *pDepthMarketData);

  // documents insert the document
  mds.push_back(md);
#ifdef DEBUG
  if (mds.size() >= TIMES) {
    db->insert_many(mds);
    mds.clear();
  }
#else
  if (mds.size() >= 50) {
    if (db->insert_many(mds)) {
      LOG(INFO) << "insert TSMarketDataFields success!";
    } else {
      LOG(ERROR) << "Can't insert TSMarketDataFields!";
    }
    mds.clear();
  }
#endif

#ifdef DEBUG
  if (timenum == TIMES) {
    ofstream fout("../test/time/recvtime.txt");
    for (int i = 0; i < TIMES; ++i) {
      fout << recvtime[i] << endl;
    }
    fout.close();

    fout.open("../test/time/sendtime.txt");
    for (int i = 0; i < TIMES; ++i) {
      fout << sendtime[i] << endl;
    }
    fout.close();

    fout.open("../test/time/savetime.txt");
    for (int i = 0; i < TIMES; ++i) {
      fout << savetime[i] << endl;
    }
    fout.close();

    fout.open("../test/time/processinterval.txt");
    long long avg = 0;
    for (int i = 0; i < TIMES; ++i) {
      avg += getDuration(recvtime[i], savetime[i]);
    }
    fout << "AVERAGE: " << avg / TIMES << endl;
    for (int i = 0; i < TIMES; ++i) {
      fout << savetime[i] << endl;
    }
    fout.close();
    exit(0);
  }

  savetime[timenum++] = getTimeByTSC();
#endif
}

void writeThread() {
  // if recieve signal kill or ctrl+c, save remain data
  auto sig_handle = [](int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
      LOG(ERROR) << "Kill Signal recieved!";
    }

    delete que;
    db->insert_many(mds);
    exit(0);
  };
  signal(SIGINT, sig_handle);
  signal(SIGTERM, sig_handle);

  TSMarketDataField *pDepthMarketData = new TSMarketDataField;
  que = new MessageQueue(sizeof(*pDepthMarketData), 600);
  // wait to receive data
  while(1){
    if(que->receive(pDepthMarketData)){
      writeDataEngine(pDepthMarketData);
    }
  }
}

#endif // MDSERVER_DATAENGINETHREAD_H_