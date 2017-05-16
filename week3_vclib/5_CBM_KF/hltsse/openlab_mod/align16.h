#ifndef CBM_KF_align16_H
#define CBM_KF_align16_H

# ifdef _WIN32
#  define ALIGNMENT_PREFIX __declspec(align(16))
#  define ALIGNMENT_SUFFIX 
# else
#  define ALIGNMENT_PREFIX 
#  define ALIGNMENT_SUFFIX __attribute__ ((aligned(16)))
# endif

# include <new>
class ALIGNMENT_ALLOCATOR {
public:
  void* operator new( size_t n ) { return _mm_malloc( n, 16 ); }
  void* operator new[]( size_t n ) { return _mm_malloc( n, 16 ); }
  void operator delete( void* ptr ) { _mm_free( ptr ); }
  void operator delete[]( void* ptr ) { _mm_free( ptr ); }
};

#endif /* CBM_KF_align16_H */
