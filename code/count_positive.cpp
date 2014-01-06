#include <Rcpp.h>
#include <thread>
#include <future>

using namespace Rcpp ;
typedef NumericVector::iterator Iterator ;

inline int count_positive_range(Iterator begin, Iterator end){
  return std::count_if( begin, end, 
    [](double d){ return d > 0 ;}
  ) ;
}

// [[Rcpp::export]]
int count_positive_serial(NumericVector x){
  return count_positive_range( x.begin(), x.end() ) ;
}

// [[Rcpp::export]]
int count_positive_2threads(NumericVector x){
  int n = x.size() ;
  Iterator it = x.begin() ;
                                         
  std::packaged_task<int(Iterator,Iterator)> task( &count_positive_range ) ;
  std::future<int> first_half = task.get_future() ;
  std::thread t( std::move(task), it, it+n/2 ) ;            
  
  int second_half = count_positive_range(it+n/2, x.end() ) ;
  t.join() ;
  
  return first_half.get() + second_half ;
}

typedef std::packaged_task<int(Iterator,Iterator)> Task ;

// [[Rcpp::export]]
int count_positive_threaded(NumericVector data, int nthreads){
  int n = data.size() ;
  int chunk_size = n / nthreads ; 
  
  std::vector<std::future<int>> futures(nthreads-1) ;
  std::vector<std::thread> threads(nthreads-1) ;
  
  Iterator it = data.begin() ;
  for( int i=0; i<nthreads-1; i++){
    Task task(&count_positive_range) ;
    futures[i] = task.get_future();
    threads[i] = std::thread( std::move(task), it, it + chunk_size ) ;
    it += chunk_size ;
  }
  
  int result = count_positive_range(it, data.end()); 
  
  for( int i=0; i<nthreads-1; i++) {
    threads[i].join() ;
    result += futures[i].get() ;  
  }
                            
  return result ;
}

/*** R
  v <- rnorm(1e9)
  system.time( count_positive_serial(v)   )
  system.time( count_positive_2threads(v) )
  system.time( count_positive_threaded(v,4) )
*/

