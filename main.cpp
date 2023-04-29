                                                                                                                              /*
 Copyright Mykola Rabchevskiy 2022.
 Distributed under the Boost Software License, Version 1.0.
 (See http://www.boost.org/LICENSE_1_0.txt)
________________________________________________________________________________________________________________________________

 Deadlock demo

 Shared data accessible for reading and writing from a few threads
 protected by shared_lock/unique_lock.

 2023.04.28 Initial version

________________________________________________________________________________________________________________________________
                                                                                                                              */
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include <atomic>
#include <thread>
#include <mutex>
#include <shared_mutex>

void g( const int& x, const int& y );

class Data{

  int X;
  int Y;

  mutable std::shared_mutex mutex;

public:

  std::atomic< int > n;

  Data(): X{ 0 }, Y{ 0 }, mutex{}, n{ 0 }{
    printf( " Data created\n" );
  }

  void get( int& x, int& y ){
   std::shared_lock< std::shared_mutex > lock( mutex );
   x = X, y = Y;
  }

  void put( const int& x, const int& y ){
    std::unique_lock< std::shared_mutex > lock( mutex );
    X = x, Y = y;
    X = 7.0*sin( X ), Y = 11.0*cos( Y );
    if( rand() % 997*17 == 0 ) g( x, y );
    n++;
  }

};//Data

Data resource;

void g( const int& x, const int& y ){
  int u;
  int v;
  resource.get( u,   v   );
  resource.put( 2*u, 2*v );
}

void f(){
  for(;;){
    resource.put( rand() % 17, rand() % 13 );
    g( rand() % 19, rand() % 11 );
  }
}

int main(){

  srand( time( 0 ) );

  std::thread t1( f );
  std::thread t2( f );
  std::thread t3( f );
  std::thread t4( f );

  int i_{ 0 };

  for(;;){
    int i = resource.n.load();
    if( i != i_ ){
      printf( " %8i \n", i );
      i_ = i;
    }
  }//forever

  return 0;
}
