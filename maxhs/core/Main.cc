#include <errno.h>
#include <fcntl.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <signal.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#ifdef GLUCOSE
#include "glucose/utils/Options.h"
#include "glucose/utils/System.h"
#else
#include "minisat/utils/Options.h"
#include "minisat/utils/System.h"
#endif

#include "maxhs/core/BanyanSatSolver.h"
#include "maxhs/core/MaxSolver.h"
#include "maxhs/core/Wcnf.h"
#include "maxhs/utils/Params.h"
#include "maxhs/utils/ThreadPool.h"

using std::cout;

#ifdef GLUCOSE
namespace Minisat = Glucose;
#endif
using namespace Minisat;

int main(int argc, char* argv[]) {
  params.readOptions();
  banyan::Problems pbs;
  int fd_pbs = open(argv[1], O_RDONLY);
  auto parseSucc = pbs.ParseFromFileDescriptor(fd_pbs);
  if (!parseSucc) {
    printf("parse proto file error");
  }

  banyan::Results rsts;
  auto clustterRst = rsts.mutable_clusterrst();
  
  for (auto pb : pbs.problems()) {
    BanyanSatSolver solver(pb);
    (*clustterRst)[pb.clusterid()] = solver.solve();
  }

  int fd_rsts = open(argv[2], O_CREAT | O_TRUNC | O_RDWR, 0644);
  rsts.SerializeToFileDescriptor(fd_rsts);
  return 0;
}