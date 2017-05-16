## requires the Vc library: 

#### Download 
http://code.compeng.uni-frankfurt.de/projects/vc  
https://github.com/VcDevel/Vc

#### Install
`mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/opt/Vc -DBUILD_TESTING=OFF <srcdir>
make -j16
make install`  

#### include in <YOUR_FILE>.cpp
`#include <Vc/Vc>
using namespace Vc;`

#### Run
`g++ -O3 -std=c++11 -I/opt/Vc/include -L/opt/Vc/lib  <YOUR_FILE>.cpp -o test -lVc && ./test`
