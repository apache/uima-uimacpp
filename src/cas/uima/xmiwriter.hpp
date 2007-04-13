#ifndef UIMA_XMIWRITER_HPP
#define UIMA_XMIWRITER_HPP
/** \file xmiwriter.hpp .
-----------------------------------------------------------------------------




 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.

-----------------------------------------------------------------------------

   Description:

-----------------------------------------------------------------------------

   \brief Used to output the CAS in XMI format

-------------------------------------------------------------------------- */


/* ----------------------------------------------------------------------- */
/*       Include dependencies                                              */
/* ----------------------------------------------------------------------- */
#include "uima/pragmas.hpp"

#include <iostream>
#include <vector>
#include <set>
#include <map>

#include "uima/featurestructure.hpp"
#include "uima/caswriter_abase.hpp"
#include "uima/lowlevel_typedefs.hpp"
#include "uima/typesystem.hpp"
#include "uima/xmishareddata.hpp"

/* ----------------------------------------------------------------------- */
/*       Constants                                                         */
/* ----------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/*       Forward declarations                                              */
/* ----------------------------------------------------------------------- */

namespace uima {
  class CAS;
  namespace internal {
    class CASImpl;
  }
  namespace lowlevel {
    class IndexABase;
    class FSHeap;
    class TypeSystem;
  }
}

/* ----------------------------------------------------------------------- */
/*       Types / Classes                                                   */
/* ----------------------------------------------------------------------- */

/**
 * This file contains the XmiWriter class used 
 * to output the CAS in XMI format.
 */

namespace uima {


  class UIMA_LINK_IMPORTSPEC XmiWriter : public XMLWriterABase {
  private:
    Type iv_stringType;
    Type iv_integerType;
    Type iv_floatType;
    Type iv_byteType;
    Type iv_booleanType;
    Type iv_shortType;
    Type iv_longType;
    Type iv_doubleType;
    Type iv_arrayType;
    Type iv_intArrayType;
    Type iv_floatArrayType;
    Type iv_stringArrayType;
    Type iv_byteArrayType;
    Type iv_booleanArrayType;
    Type iv_shortArrayType;
    Type iv_longArrayType;
    Type iv_doubleArrayType;

    Type iv_sofaType;
    bool isBCCas;
    map<int, vector<int>*> enqueuedFS;
    int sofa;

    void writeFeatureValue(ostream & os, FeatureStructure const & fs, Feature const & f);
    void writeFSFlat(ostream & os, FeatureStructure const & fs, vector<int>* indexInfo);
    void findReferencedFSs(FeatureStructure const & fs, bool check=true);
    bool isReferenceType(Type const & t) const;
    bool enqueueIndexed(FeatureStructure const &fs, int sofa);
    bool enqueueUnindexed(FeatureStructure const &fs);
    bool enqueueUnindexed(int id);
    void initTypeAndNamespaceMappings();
    void writeViews(ostream & os, CAS const & crCAS);
    void writeView(ostream & os,int sofaXmiId, vector<lowlevel::TyFS>& members);
    XmlElementName * uimaTypeName2XmiElementName(UnicodeStringRef & uimaTypeName);
    int getXmiId(int addr);
    void writeArray(ostream & os, FeatureStructure const & array, char const * tag, int xmiid);
    void writeBooleanArray(ostream & os, BooleanArrayFS const & array, char const * tag, int xmiid);
    void writeStringArray(ostream & os, StringArrayFS const & array, char const * tag, int xmiid);
    string arrayToString(FeatureStructure const & fs, char const * tag);
    string listToString(FeatureStructure const & fs, char const * tag);
    void enqueueIncoming();
    void serializeOutOfTypeSystemElements(ostream & os);
    void enqueueFSListElements(FeatureStructure const & fs);
    vector<XmlElementName*> xmiTypeNames;  // array, indexed by type code, 
                                           // giving XMI names for each type
    map<string, string> nsUriToPrefixMap;
    set<string> nsPrefixesUsed;
    XmiSerializationSharedData * sharedData;
    
    static char const *  XMI_NS_URI;
    static char const *  XMI_NS_PREFIX;
    static char const *  DEFAULT_NAMESPACE_URI;
    static char const *  XSI_NS_URI;
    static char const *  XMI_TAG_LOCAL_NAME;
    static char const *  XMI_TAG_QNAME;
    static char const *  INDEXED_ATTR_NAME;
    static char const *  ID_ATTR_NAME;
    static char const *  XMI_VERSION_LOCAL_NAME;
    static char const *  XMI_VERSION_QNAME;
    static char const *  XMI_VERSION_VALUE;

  public:
    XmiWriter(CAS const & crCAS, bool bAddDocBuffer);
    XmiWriter(CAS const & crCAS, bool bAddDocBuffer, XmiSerializationSharedData * );
    ~XmiWriter();

    virtual void write(ostream& os);
  };

}


/* ----------------------------------------------------------------------- */
/*       Implementation                                                    */
/* ----------------------------------------------------------------------- */


/* ----------------------------------------------------------------------- */


#endif
