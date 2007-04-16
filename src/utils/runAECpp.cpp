/*------------------------------------------------------------------------

  Test driver that reads text files or XCASs and calls the annotator

-------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/*       Include dependencies                                              */
/* ----------------------------------------------------------------------- */

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>


#include "xercesc/framework/LocalFileInputSource.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XMLString.hpp"

#include "uima/pragmas.hpp"
#include "uima/filename.hpp"
#include "uima/dirwalk.hpp"

#include "uima/api.hpp"
#include "uima/xmlwriter.hpp"
#include "uima/xcasdeserializer.hpp"
#include "uima/xmideserializer.hpp"

using namespace uima;


/* ----------------------------------------------------------------------- */
/*       Implementation                                                    */
/* ----------------------------------------------------------------------- */

/* Little helper routine to check and report errors.
   This routine just does a hard program exit for any failure! */
static void tafCheckError(TyErrorId utErrorId,
                          const AnalysisEngine &  crEngine);
static void tafCheckError(ErrorInfo const &);

// sofa to use for creating a tcas
bool useSofa;
const char* sofaName;

// input data types
enum dataFormats { textFormat, xcasFormat, xmiFormat };
dataFormats xcasInput;

void process (AnalysisEngine * pEngine, CAS * cas, std::string in, std::string out);
void writeXCAS (CAS & outCas, int num,  std::string in, std::string outfn);

void tell() {
  cerr << "Usage: runAECpp UimaCppDescriptor <-x> InputFileOrDir <OutputDir>" << endl
  << "                <-s Sofa>  <-l LogLevel>" << endl;
  cerr << "  UimaCppDescriptor   Analysis Engine descriptor for a CPP annotator" << endl;
  cerr << "  InputFileOrDir      Input file or directory of files to process" << endl;
  cerr << "  OutputDir           Existing directory for XCAS outputs (optional)" << endl;
  cerr << "       Options:" << endl;
  cerr << "   -x [-xmi]     Input(s) must be in XCAS [XMI] format (default is raw text)" << endl;
  cerr << "   -s Sofa       Name of a Sofa to process (input must be an XCAS)" << endl;
  cerr << "   -l logLevel   Set to 0, 1, or 2 for Message, Warning, or Error" << endl;
}

int main(int argc, char * argv[]) {

  int loglevel = -1;

  try {

    /* Access the command line arguments to get the name of the input text. */
    //if (argc != 3 && argc != 5 && argc != 7 && argc != 9 && argc != 11) {
    if (argc < 3) {
      tell();
      return 1;
    }
    useSofa = false;
    xcasInput = textFormat;
    /* input/output dir arg */
    std::string in;
    std::string out;
    std::string sofa;
    std::string pattern("*");
    const char* cnfg = NULL;

    int index = 0;
    while (++index < argc) {
      char* arg = argv[index];
      if (0 == strcmp(arg, "-x")) {
        xcasInput = xcasFormat;
      } else if (0 == strcmp(arg, "-xcas")) {
        xcasInput = xcasFormat;
      } else if (0 == strcmp(arg, "-xmi")) {
        xcasInput = xmiFormat;
      } else if (0 == strcmp(arg, "-s")) {
        if ( ++index < argc ) {
          sofaName = argv[index];
          useSofa = true;
        }
      } else if (0 == strcmp(arg, "-l")) {
        if ( ++index < argc ) {
          loglevel = atoi(argv[index]);
          if (loglevel < LogStream::EnMessage) {
            cerr << "LogLevel less than minimum value (Message) = " << LogStream::EnMessage << endl;
            return 1;
          }
          if (loglevel > LogStream::EnError) {
            cerr << "LogLevel greater than maximum value (Error) = " << LogStream::EnError << endl;
            return 1;
          }
        }
      } else { //one of the standard params - whichever we haven't read yet
        if (cnfg == NULL) {
          cnfg = arg;
        } else if (in.length() == 0) {
          in.append(arg);
        } else if (out.length() == 0) {
          out.append(arg);
        }
      }
    } //while

    if (in.length() == 0 || index > argc) {   // Too few args or no arg after -s or -l
      tell();
      return 1;
    }

    if (out == in) {
      cout << "runAECpp: ERROR: input and output file paths are the same " << endl;
      return -1;
    }

    /* Create/link up to a UIMACPP resource manager instance (singleton) */
    (void) ResourceManager::createInstance("runAECpp");

    if (loglevel >= 0) {
      ResourceManager::getInstance().setLoggingLevel((LogStream::EnEntryType)loglevel);
    }

    TyErrorId utErrorId;          // Variable to store UIMACPP return codes
    ErrorInfo errorInfo;          // Variable to stored detailed error info
    /* Initialize engine with filename of config-file */
    AnalysisEngine * pEngine =
      Framework::createAnalysisEngine(cnfg, errorInfo);
    tafCheckError(errorInfo);

    /* Get a new CAS */
    CAS* cas = pEngine->newCAS();

    /* process input xcas */
    util::DirectoryWalk dirwalker(in.c_str());
    if (dirwalker.isValid()) {
      cout << "runAECpp::processing all files in directory: " << in.c_str() << endl;
      util::Filename infile(in.c_str(),"FilenamePlaceHolder");
      while (dirwalker.isValid()) {
        // Process all files or just the ones with matching suffix
        if ( dirwalker.isFile() ) {
          infile.setNewName(dirwalker.getNameWithoutPath());
          std::string afile(infile.getAsCString());

          //process the cas
          process(pEngine,cas,afile, out);

          //reset the cas
          cas->reset();
        }
        //get the next xcas file in the directory
        dirwalker.setToNext();
      }
    } else {
      //process the cas
      process(pEngine,cas, in, out);
    }
    /* call collectionProcessComplete */
    utErrorId = pEngine->collectionProcessComplete();

    /* Free annotator */
    utErrorId = pEngine->destroy();

    delete cas;
    delete pEngine;
  } catch (Exception e) {
    cout << "runAECpp " << e << endl;
  }
  /* If we got this far everything went OK */
  cout << "runAECpp: processing finished sucessfully! " << endl;

  return(0);
}



/* Little helper routine to check and report errors.
   This routine just does a hard program exit for any failure!
*/
static void tafCheckError(TyErrorId utErrorId,
                          const AnalysisEngine &  crEngine) {
  if (utErrorId != UIMA_ERR_NONE) {
    cerr << "runAECpp:" << endl;
    cerr << "  Error number        : "
    << utErrorId << endl;
    cerr << "  Error string        : "
    << AnalysisEngine::getErrorIdAsCString(utErrorId) << endl;
    const TCHAR * errStr = crEngine.getAnnotatorContext().getLogger().getLastErrorAsCStr();
    if (errStr != NULL)
      cerr << "  Last logged message : " << errStr << endl;
    exit((int)utErrorId);
  }
}

/* Similar routine as above just with error info objects instead of err-ids.
   This routine just does a hard program exit for any failure!
*/
static void tafCheckError(ErrorInfo const & errInfo) {
  if (errInfo.getErrorId() != UIMA_ERR_NONE) {
    cerr << "runAECpp:" << endl
    << "  Error string  : "
    << AnalysisEngine::getErrorIdAsCString(errInfo.getErrorId()) << endl
    << "  UIMACPP Error info:" << endl
    << errInfo << endl;
    exit((int)errInfo.getErrorId());
  }
}

void process (AnalysisEngine * pEngine, CAS * cas, std::string in, std::string outfn) {
  cout << endl << "runAECpp::processing " << in << endl;
  try {
    if (xcasInput != textFormat) {
      /* initialize from an xcas or xmicas */
      //cout << "runAECpp::processing xml file " << in << endl;
	  LocalFileInputSource fileIS(XMLString::transcode(in.c_str()));
	  if (xcasInput == xcasFormat) {
		XCASDeserializer::deserialize(fileIS, *cas);
	  }
	  else {
		XmiDeserializer::deserialize(fileIS, *cas);
	  }
    } else {
      /* read as text file and set document text of default view */
      FILE * pFile = fopen(in.c_str(),"rb");
      int filesize;
      if (pFile == NULL) {
        cerr << "RunAECpp: Error reading file " << in << endl;
        exit(-1);
      }

      /* allocate buffer for file contents */
      struct stat fstat;
      stat(in.c_str(), &fstat);
      filesize = fstat.st_size;
      char * pBuffer = new char[filesize+1];
      if (pBuffer == NULL) {
        cerr << "RunAECpp: Error allocating buffer to hold contents of file  " << in << endl;
        exit(-1);
      }

      /* read the file */
      size_t numread = fread(pBuffer,1,filesize,pFile);
      fclose(pFile);
      /* convert to unicode and set tcas document text*/
      UnicodeString ustrInputText(pBuffer, (int32_t)numread, "utf-8");
      cas->setDocumentText(UnicodeStringRef(ustrInputText));
      delete pBuffer;
    }

    // Is the input a tcas?
    if (!useSofa && cas->isBackwardCompatibleCas()) {
      useSofa = true;
      sofaName = CAS::NAME_DEFAULT_TEXT_SOFA;
    }

    // Is a specific Sofa view specified?
    if (useSofa) {
      /* process the specified TCAS */
      SofaFS mySofa = cas->getSofa(pEngine->getAnnotatorContext().mapToSofaID(sofaName));
      if (!mySofa.isValid()) {
        cerr << "runAECpp:" << endl
        << "  Specified Sofa named " << sofaName
        << " not found in the XCAS file" << endl;
        exit(99);
      }

      CASIterator casIter = pEngine->processAndOutputNewCASes(*cas->getView(mySofa));
      int i=0;
      while (casIter.hasNext()) {
        i++;
        CAS  & outCas = casIter.next();

        //write out xcas
        if (outfn.length() > 0) {
          writeXCAS(outCas, i, in, outfn);
        }

        //release the CAS
        pEngine->getAnnotatorContext().releaseCAS(outCas);

        cout << "new Cas " << i << endl;
      }

    } else {
      /* process the CAS */
      
      CASIterator casIter = ((AnalysisEngine*)pEngine)->processAndOutputNewCASes(*cas);
      int i=0;
      while (casIter.hasNext()) {
        i++;
        CAS & outCas = casIter.next();
        //write out xcas
        if (outfn.length() > 0) {
          writeXCAS(outCas, i, in, outfn);
        }

        //release CAS
        pEngine->getAnnotatorContext().releaseCAS(outCas);

        cout << "new Cas " << i << endl;
      }

    }

    if (outfn.length() > 0) {
      util::Filename infile((TCHAR*) in.c_str());

      outfn.append("/");
      outfn.append(infile.getName());

      //open a file stream for output xcas
      ofstream file;
      file.open (outfn.c_str(), ios::out | ios::binary);
      if ( !file ) {
        cerr << "runAECpp: Error opening output xcas: " << outfn.c_str() << endl;
        exit(99);
      }

      //serialize the input cas
      cout << "runAECpp: write out xcas " << outfn << endl;
      XCASWriter writer(*cas, true);
      writer.write(file);
      file.close();
    }

  } catch (Exception e) {
    ErrorInfo errInfo = e.getErrorInfo();
    cerr << "runAECPP::Error " << errInfo.getErrorId() << " " << errInfo.getMessage() << endl;
    cerr << errInfo << endl;
  }
}

void writeXCAS (CAS & outCas, int num,  std::string in, std::string outfn)  {

  util::Filename infile((TCHAR*) in.c_str());
  std::string ofn;
  ofn.append(outfn.c_str());
  ofn.append("/");
  ofn.append(infile.getName());
  ofn.append("_seg_");
  stringstream s;
  s << num;
  ofn.append(s.str());

  //open a file stream for output xcas
  ofstream file;
  file.open (ofn.c_str(), ios::out | ios::binary);
  if ( !file ) {
    cerr << "Error opening output xcas: " << ofn.c_str() << endl;
    exit(99);
  }

  //serialize the cas
  cout << "write out xcas " << ofn << endl;
  XCASWriter writer(outCas, true);
  writer.write(file);
  file.close();
}

/* <EOF> */

