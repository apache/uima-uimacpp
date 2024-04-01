#include <Python.h>
#include "uima/api.hpp"

// for u_init
#include "unicode/uclean.h"

#define MODULENAME "Pythonnator"

#ifdef LINUX
// Beacuse of UIMA not using the RTLD_GLOBAL flag when binding to this
// .so file, we do not get the exports from the dependent libraries in 
// when loading.  This flag causes the python .so file to be re-bound
// hopefully this can be removed pending a future relesase of UIMA
#define REBIND_PYTHON_SO 
#include <dlfcn.h>
#endif

#define _PY_BEGIN_BLOCK_THREADS_ \
  PyEval_RestoreThread(thread_state);
#define _PY_END_BLOCK_THREADS_ \
  thread_state = PyEval_SaveThread();

using namespace uima;
using namespace std;
using namespace icu;



class Pythonnator : public Annotator {
  PyThreadState *thread_state;
  PyObject *function[FUNCTIONCOUNT];
  int debug;
#ifdef REBIND_PYTHON_SO
      static void *library;
#endif
  static unsigned int refcount;

public:
  // We construct a python interpreter in initialize - it lives for the
  // life of the annotator - even if reconfigure happens.  Reconfigure
  // and intialize both set dirty so the source code in the source file
  // and contained in the type system are evaluated.

//    PyObject *main_module, *user_module;
  TyErrorId initialize(AnnotatorContext &ac);

  TyErrorId reconfigure();

  TyErrorId typeSystemInit(TypeSystem const &ts); 
/** 
 * call the UIMA Annotator to deinitialize itself based on a UIMA engine
 * and return a UIMA error code
 */
  TyErrorId destroy();
  

/**
 * call the UIMA Annotator to perform its duty based on a UIMA engine
 * and return a UIMA error code
 */
  TyErrorId process(CAS &_cas, ResultSpecification const & _rs);

  TyErrorId batchProcessComplete();
  

  TyErrorId collectionProcessComplete();

};

#ifdef REBIND_PYTHON_SO
void *Pythonnator::library = 0;
#endif
unsigned int Pythonnator::refcount = 0;

MAKE_AE(Pythonnator);