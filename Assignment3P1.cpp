// Mathew Reilly
// COP4520
// Assignment 3
// Description:
//    The minotaur wants to send thank yous back for all of the presents he recieved. In order to do this he has
// 4 servants. The servants were tasked 3 actions:
//    Taking a present from an unordered bag and adding it to the list presents in its sorted position.
//    Writing a thank you for a present and removing it from the chain.
//    Check whether or not a present is on the chain.
// Implement a concurrent linked list that processes 500,000 presents over 4 threads (servants).
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <random>
using namespace std;


// holds information on present node and lock
class Present
{
private:
  mutex presentLock;
  bool isLocked = false;
public:
  int tag = INT_MAX;
  bool marked = false;
  Present* nextPresent = NULL;

  Present(int value)
  {
    tag = value;
  }

  void lock()
  {
    //cout << "enters lock method" << endl;
    if(presentLock.try_lock())
    {
      //isLocked = true;
      return;
    } 
    //while(isLocked)
   //{}
  }

  void unlock()
  {
    //isLocked = false;
    presentLock.unlock();
  }
};

mutex chainLock1;
mutex chainLock2;

// attemps to support lazy linked lists functions
class PresentChain
{
private:
  Present* head;

  bool validate(Present* pred, Present* curr)
  {
    return !pred->marked && !curr->marked && pred->nextPresent == curr;
  }

public:
  PresentChain()
  {
    head->tag = INT_MIN;
    head->nextPresent = new Present(INT_MAX);
  }

  bool add(int presentTag)
  {
    int key = presentTag;

    while(true)
    {
      Present* pred = head;
      Present* curr = pred->nextPresent;

      while(curr->tag < key)
      {
        pred = curr;
        curr = curr->nextPresent;
      }

      //cout << "Makes it to lock 1" << endl;
      pred->lock();
      //cout << "Locks pred 1" << endl;
      curr->lock();
      if(validate(pred, curr))
      {
        if(curr->tag == key)
        {
          curr->unlock();
          pred->unlock();
          return false;
        } else
        {
          Present* insertPresent = new Present(key);
          insertPresent->nextPresent = curr;
          pred->nextPresent = insertPresent;
          curr->unlock();
          pred->unlock();
          return true;
        }
      }
      cout << "Validate fails" << endl;
      curr->unlock();
      pred->unlock();
      //cout << "Survives lock cycle 1" << endl;
    }
  }

  bool remove(int presentTag)
  {
    int key = presentTag;

    while(true)
    {
      Present* pred = head;
      Present* curr = pred->nextPresent;

      while(curr->tag < key)
      {
        pred = curr;
        curr = curr->nextPresent;
      }

      cout << "Makes it to lock 2" << endl;

      pred->lock();
      curr->lock();
      if(validate(pred, curr))
      {
        if(curr->tag == key)
        {
          curr->marked = true;
          pred->nextPresent = curr->nextPresent;
          return true;
        } else
        {
          return false;
        }
      }
      curr->unlock();
      pred->unlock();
      //cout << "SUCCESS" << endl;
      cout << "Survives lock cycle 2" << endl;
    }
  }

  bool contains(int tag)
  {
    Present* curr = head;
    while(curr->tag < tag)
    {
      curr = curr->nextPresent;
    }
    return curr->tag == tag && !curr->marked;
  }
};


// Track Presents
vector<int> presentBag;
vector<int> thankYouTags;
mutex takePresentLock;
mutex presentTagLock;

// Supporting functions
void fillPresentBag(int presentCount)
{
  for(int i = 1; i < presentCount + 1; i++)
  {
    presentBag.push_back(i);
  }

  // random generator for shuffle
  random_device rd;
  mt19937 g(rd());

  shuffle(presentBag.begin(), presentBag.end(), g);
}

void showShuffle()
{
  for(int i = 0; i < 20; i++)
  {
    cout << presentBag.at(i) << ", "; 
  }
  cout << endl;
}

// multithreaded method for minutoar servents
void servant(PresentChain* presentChain)
{
  int randOption = 0;
  int randPresentFromBag = 0;
  int randPresentToCheck = 0;
  int tagForThankYou = 0;
  int randPresent = 0;

  while(!presentBag.empty())
  {
    //randOption = (rand() % 4);
    randOption = 1;

    //cout << "rand option: " << randOption << endl;

    switch(randOption)
    {
      case 1:         // take present from bag and add to chain (place tag in tag vector)
        takePresentLock.lock();
        randPresentFromBag = presentBag.back();
        presentBag.pop_back();
        cout << presentBag.size() << endl;
        takePresentLock.unlock();

        presentChain->add(randPresentFromBag);

        cout << "is passing add" << endl;

        presentTagLock.lock();
        thankYouTags.push_back(randPresentFromBag);
        presentTagLock.unlock();
        break;
      case 2:         // write thank you for present on chain, remove it from chain
        presentTagLock.lock();
        tagForThankYou = thankYouTags.back();
        thankYouTags.pop_back();
        presentTagLock.unlock();

        presentChain->remove(tagForThankYou);

        break;
      case 3:         // check for a specific present on the chain
        // write to file???
        randPresent = (rand() % 500000) + 1;
        presentChain->contains(randPresent);
        // cout << "does not fail here" << endl;
        break;
      default:
        break;
    }
  }
}


int main()
{
  PresentChain presentChain;
  
  fillPresentBag(10);
  // showShuffle();

  // array to hold all of the threads
  vector<thread> threads;

  // start the clock and run the threads to completion
  auto start = chrono::high_resolution_clock::now();

  // each guest is created and the number of times they queue is decided.
  for(int i = 0; i < 4; i++)
  {
    threads.emplace_back(servant, &presentChain);
  }

  // makes sure every guest finishes viewing
  for(int i = 0; i < 4; i++)
  {
    threads[i].join();
  }

  auto stop = chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

  // calculate time in seconds
  double timeSeconds = duration.count() / 1000000.0;

  // output for that run is output to a file.
  cout << timeSeconds << " seconds write thank yous with " << 4 << " servants that would randomly check for conatined presents." << endl;

  return 0;
}


