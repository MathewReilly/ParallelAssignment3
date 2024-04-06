// Mathew Reilly
// COP4520
// Assignment 3
// Description:
//    8 sensors need to collect temerature readings and store them in a shared memory space, a report is then generated every hour
//    displaying the 5 highest temperatures, the 5 lowest temperatures and the 10 minute interval where the largest temperature difference was observed.
//    No sensors can miss an interval. Simulate the temperature readings in "1 minute" intervals that such that the stats are recorded and maintained in the
//    correct interval.
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <random>
using namespace std;
const int THREAD_NUM = 8;
const int MINUTES_NUM = 10000;

//convert to bounded queue if time

// Parrallel Vars
vector<vector<int>> recordedTimes(MINUTES_NUM, vector<int>(0));  //first index denotes the minute, the vector inside denotes temperatures for that minute.
mutex recordTimeLock;
condition_variable waitForTurn;
int threadTracker = 0;
int threadsEscaped = 0;
mutex nextIterationLock;
bool threadsSynced = false;

int timer = 0;

// method all sensors must follow
void sensor(int minutesToSimulate)
{
  random_device rd;
  //mt19937 g(rd());
  int indexTracker = 0;
  int randomTemp = (rd() % 171) - 100;
  
  //unique_lock<mutex> sensorLock(recordTimeLock);
  //unique_lock<mutex> nextIterLock(nextIterationLock);

  //cout << "loc 2" << endl;
  
  while(timer < minutesToSimulate)
  {
    // wait for all threads to make it to start
    if(threadsEscaped == THREAD_NUM)
    {
      threadsEscaped = 0;
      threadsSynced = false;
    } else
    {
      while(threadsSynced)
      {}
    }

    //cout << "loc 3" << endl;

    //thread does it's work
    recordTimeLock.lock();
    recordedTimes[indexTracker].push_back(randomTemp);
    threadTracker = threadTracker + 1;
    //cout << threadTracker << endl;
    recordTimeLock.unlock();

    //cout << "loc 4" << endl;
    

    randomTemp = (rd() % 171) - 100;
    indexTracker++;


    // wait for all threads to be at the end
    if(threadTracker == THREAD_NUM)
    {
      threadTracker = 0;
      threadsSynced = true;
      nextIterationLock.lock();
      threadsEscaped++;
      timer++;
      nextIterationLock.unlock();
      
    } else
    {
      while(!threadsSynced)
      {
      }
      nextIterationLock.lock();
      threadsEscaped++;
      nextIterationLock.unlock();
    }
    
  }
}


int main()
{

  // array to hold all of the threads
  vector<thread> threads;

  vector<int> hourData(0);

  // start the clock and run the threads to completion
  auto start = chrono::high_resolution_clock::now();

  // each guest is created and the number of times they queue is decided.
  for(int i = 0; i < THREAD_NUM; i++)
  {
    threads.emplace_back(sensor, MINUTES_NUM);
  }

  // makes sure every thread completes
  for(int i = 0; i < THREAD_NUM; i++)
  {
    threads[i].join();
  }

  auto stop = chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

  
  // calculate time in seconds
  double timeSeconds = duration.count() / 1000000.0;

  
  /*for(int i = 0; i < recordedTimes.size(); i++)
  {
    for(int j = 0; j < recordedTimes[i].size(); j++)
    {
      cout << recordedTimes[i][j] << " ";
    }
    cout << endl;
  }*/

  int hours = 0;
  int entriesSize = 0;


  for(int i = 0; i < recordedTimes.size(); i++)
  {
    if(i % 60 == 0 && i > 0)
    {
      hours++;
      entriesSize = hourData.size();
      sort(hourData.begin(), hourData.end());
      cout << "Hour: " << hours << endl;
      cout << "    Lowest 5 Temps: " << hourData[0] << " " << hourData[1] << " " << hourData[2] << " " << hourData[3] << " " << hourData[4] << endl;
      cout << "    Highest 5 Temps: " << hourData[entriesSize - 5] << " " << hourData[entriesSize - 4] << " " << hourData[entriesSize - 3] << " " << hourData[entriesSize - 2] << " " << hourData[entriesSize - 1] << endl;

      hourData.clear();
    }

    for(int j = 0; j < recordedTimes[i].size(); j++)
    {
      hourData.push_back(recordedTimes[i][j]);
    }
  }

  // output for that run is output to a file.
  cout << timeSeconds << " seconds to simulate collecting data with " << THREAD_NUM << " sensors." << endl;

  return 0;
}


