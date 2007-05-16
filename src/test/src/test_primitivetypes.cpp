/** \file sofatest.cpp .

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

-------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/*       Include dependencies                                              */
/* ----------------------------------------------------------------------- */
#include "uima/api.hpp"
#include "uima/macros.h"
#include "uima/xmlwriter.hpp"
#include "uima/xmiwriter.hpp"
#include "uima/xcasdeserializer.hpp"
#include "uima/xmideserializer.hpp"
#include "uima/internal_casserializer.hpp"
#include "uima/internal_casdeserializer.hpp"
#include "uima/taespecifierbuilder.hpp"
#include "xercesc/util/Base64.hpp"
#include <fstream>
XERCES_CPP_NAMESPACE_USE
#ifndef NDEBUG
#define ASSERT_OR_THROWEXCEPTION(x) assert(x)
#else
#define ASSERT_OR_THROWEXCEPTION(x) if (!(x)) { cerr << __FILE__ << ": Error in line " << __LINE__ << endl; exit(1); }
#endif

#define LOG(x) cout << __FILE__ << __LINE__ << ": " << x << endl

using namespace uima;
icu::UnicodeString str1("string1");
icu::UnicodeString str2("string2");
icu::UnicodeString str3("string3");
icu::UnicodeString str4("string4");
icu::UnicodeString str5("string5");

UnicodeStringRef strings[] = {
                               str1, str3, str2, str5, str4
                             };

int ints[] = {
               1,2,3,4,5
             };
float floats[] = {
                   (float)1.1, (float)2.2, (float)3.3, (float)4.4, (float)5.5
                 };

char chars[] = {
                 1, 10, 'a','b','c','d','e',8,16,64,128,255
               };


int shorts[] = {
                 10,20,30,40,50
               };
int longs[] = {
                10000,20000,30000,40000,50000
              };
double doubles[] = {
                     21E10,
                     31E10,
                     41E10,
                     51E10,
                     61E10

                   };

bool val = false;
UnicodeString ustr("this beer is good");
int begin = 1;
int end = 5;
char * viewName = "EnglishDocument";
#define BOOLEAN_ARRAY_SIZE 20

void createExampleFS(CAS & cas)  {

  Type testType = cas.getTypeSystem().getType("test.primitives.Example");
  Feature intF = testType.getFeatureByBaseName("intFeature");
  Feature floatF = testType.getFeatureByBaseName("floatFeature");
  Feature stringF = testType.getFeatureByBaseName("stringFeature");
  Feature booleanF = testType.getFeatureByBaseName("boolFeature");
  Feature byteF = testType.getFeatureByBaseName("byteFeature");
  Feature shortF = testType.getFeatureByBaseName("shortFeature");
  Feature longF = testType.getFeatureByBaseName("longFeature");
  Feature doubleF = testType.getFeatureByBaseName("doubleFeature");

  Feature intArrayF = testType.getFeatureByBaseName("intArrayFeature");
  Feature floatArrayF = testType.getFeatureByBaseName("floatArrayFeature");
  Feature stringArrayF = testType.getFeatureByBaseName("stringArrayFeature");
  Feature booleanArrayF = testType.getFeatureByBaseName("boolArrayFeature");
  Feature byteArrayF = testType.getFeatureByBaseName("byteArrayFeature");
  Feature shortArrayF = testType.getFeatureByBaseName("shortArrayFeature");
  Feature longArrayF = testType.getFeatureByBaseName("longArrayFeature");
  Feature doubleArrayF = testType.getFeatureByBaseName("doubleArrayFeature");
  Feature intListF = testType.getFeatureByBaseName("intListFeature");
  Feature floatListF = testType.getFeatureByBaseName("floatListFeature");
  Feature stringListF = testType.getFeatureByBaseName("stringListFeature");
  Feature fsListF = testType.getFeatureByBaseName("fsListFeature");
  Feature fsArrayF = testType.getFeatureByBaseName("fsArrayFeature");
  //get index repository
  FSIndexRepository & indexRep = cas.getIndexRepository();

  //   Create a view
  CAS * englishView = cas.createView(viewName);
  // Set the document text
  englishView->setDocumentText(ustr);

  // create an FS of exampleType and index it
  AnnotationFS fs = englishView->createAnnotation(testType, begin, end);
  englishView->getIndexRepository().addFS(fs);

  // create Array FSs
  StringListFS stringListFS = cas.createStringListFS();
  for (size_t i=0;i < NUMBEROF(strings); i++) {
	  stringListFS.addLast(strings[i]);
  }
  StringArrayFS stringArrayFS = cas.createStringArrayFS(NUMBEROF(strings));

  for (size_t i=0; i< NUMBEROF(strings); ++i) {
    stringArrayFS.set(i, strings[i]);
  }

  IntListFS intListFS = cas.createIntListFS();
  for (size_t i=0;i < NUMBEROF(ints); i++) {
	  intListFS.addLast(ints[i]);
  }

  IntArrayFS intArrayFS = cas.createIntArrayFS(NUMBEROF(ints));
  for (size_t i=0; i< NUMBEROF(ints); ++i) {
    intArrayFS.set(i, ints[i]);
  }


  FloatListFS floatListFS = cas.createFloatListFS();
  for (size_t i=0;i < NUMBEROF(floats); i++) {
	  floatListFS.addLast(floats[i]);
  }

  FloatArrayFS floatArrayFS = cas.createFloatArrayFS(NUMBEROF(floats));
  for (size_t i=0; i< NUMBEROF(floats); ++i) {
    floatArrayFS.set(i, floats[i]);
  }

  ByteArrayFS byteArrayFS = cas.createByteArrayFS(NUMBEROF(chars));
  for (size_t i=0; i< NUMBEROF(chars); ++i) {
    byteArrayFS.set(i, chars[i]);
  }

  BooleanArrayFS boolArrayFS = cas.createBooleanArrayFS(BOOLEAN_ARRAY_SIZE);
  for (int i=0; i<20; i++) {
    boolArrayFS.set(i,val=!val);
  }

  ShortArrayFS shortArrayFS = cas.createShortArrayFS(NUMBEROF(shorts));
  for (size_t i=0; i< NUMBEROF(shorts); ++i) {
    shortArrayFS.set(i, shorts[i]);
  }

  LongArrayFS longArrayFS = cas.createLongArrayFS(NUMBEROF(longs));
  for (size_t i=0; i< NUMBEROF(longs); ++i) {
    longArrayFS.set(i, longs[i]);
  }

  DoubleArrayFS doubleArrayFS = cas.createDoubleArrayFS(NUMBEROF(doubles));
  for (size_t i=0; i< NUMBEROF(doubles); ++i) {
    doubleArrayFS.set(i, doubles[i]);
  }

  Type annot = cas.getTypeSystem().getType(CAS::TYPE_NAME_ANNOTATION);
  
  ListFS fsListFS = cas.createListFS();
  for (size_t i=0; i < 2; i++) {
	  AnnotationFS annotFS = englishView->createAnnotation(annot,i,i+5);
	  fsListFS.addFirst(annotFS);
  }
  fs.setFeatureValue(fsListF,fsListFS);

  ArrayFS fsArrayFS = cas.createArrayFS(3);
  for (size_t i=0; i < 3; i++) {
	  AnnotationFS annotFS = englishView->createAnnotation(annot,i,i+5);
	  fsArrayFS.set(i,annotFS);
  }
  fs.setFeatureValue(fsArrayF,fsArrayFS);

  // set features of fs
  fs.setStringValue(stringF, strings[0]);
  fs.setFloatValue(floatF, floats[0]);
  fs.setBooleanValue(booleanF, val);
  fs.setByteValue(byteF, chars[0]);
  fs.setByteValue(byteF, 'z');
  fs.setShortValue(shortF, shorts[0]);
  fs.setLongValue(longF, longs[0]);
  fs.setDoubleValue(doubleF, doubles[0]);

  fs.setFeatureValue(intArrayF, intArrayFS);
  fs.setFeatureValue(floatArrayF, floatArrayFS);
  fs.setFeatureValue(stringArrayF, stringArrayFS);
  fs.setFeatureValue(byteArrayF, byteArrayFS);
  fs.setFeatureValue(booleanArrayF, boolArrayFS);
  fs.setFeatureValue(shortArrayF, shortArrayFS);
  fs.setFeatureValue(longArrayF, longArrayFS);
  fs.setFeatureValue(doubleArrayF, doubleArrayFS);
  fs.setFeatureValue(intListF, intListFS);
  fs.setFeatureValue(floatListF, floatListFS);
  fs.setFeatureValue(stringListF, stringListFS);

}

void validateFS(CAS & cas)  {


  Type testType = cas.getTypeSystem().getType("test.primitives.Example");
  Feature intF = testType.getFeatureByBaseName("intFeature");
  Feature floatF = testType.getFeatureByBaseName("floatFeature");
  Feature stringF = testType.getFeatureByBaseName("stringFeature");
  Feature booleanF = testType.getFeatureByBaseName("boolFeature");
  Feature byteF = testType.getFeatureByBaseName("byteFeature");
  Feature shortF = testType.getFeatureByBaseName("shortFeature");
  Feature longF = testType.getFeatureByBaseName("longFeature");
  Feature doubleF = testType.getFeatureByBaseName("doubleFeature");

  Feature intArrayF = testType.getFeatureByBaseName("intArrayFeature");
  Feature floatArrayF = testType.getFeatureByBaseName("floatArrayFeature");
  Feature stringArrayF = testType.getFeatureByBaseName("stringArrayFeature");
  Feature booleanArrayF = testType.getFeatureByBaseName("boolArrayFeature");
  Feature byteArrayF = testType.getFeatureByBaseName("byteArrayFeature");
  Feature shortArrayF = testType.getFeatureByBaseName("shortArrayFeature");
  Feature longArrayF = testType.getFeatureByBaseName("longArrayFeature");
  Feature doubleArrayF = testType.getFeatureByBaseName("doubleArrayFeature");
  Feature intListF = testType.getFeatureByBaseName("intListFeature");
  Feature floatListF = testType.getFeatureByBaseName("floatListFeature");
  Feature stringListF = testType.getFeatureByBaseName("stringListFeature");
  Feature fsListF = testType.getFeatureByBaseName("fsListFeature");
  Feature fsArrayF = testType.getFeatureByBaseName("fsArrayFeature");

  //get index repository
  FSIndexRepository & indexRep = cas.getIndexRepository();

  //   get a view
  CAS * englishView = cas.getView(viewName);
  ASSERT_OR_THROWEXCEPTION(EXISTS(englishView));
  ASSERT_OR_THROWEXCEPTION(0==englishView->getDocumentText().compare(ustr));

  ANIndex index = englishView->getAnnotationIndex(testType);
  ANIterator iter = index.iterator();
  AnnotationFS fs = iter.get();
  ASSERT_OR_THROWEXCEPTION(fs.isValid());

  size_t num;
  
  StringListFS strListFS = fs.getStringListFSValue(stringListF);
  for (num=0; num< NUMBEROF(floats); num++) {
    ASSERT_OR_THROWEXCEPTION(strListFS.getHead()==strings[num]);
	strListFS = strListFS.getTail();
  }
  ASSERT_OR_THROWEXCEPTION(NUMBEROF(strings)==num);
  
  StringArrayFS stringArrayFS = fs.getStringArrayFSValue(stringArrayF);
  ASSERT_OR_THROWEXCEPTION(NUMBEROF(strings)==stringArrayFS.size());
  for (size_t i=0; i< NUMBEROF(strings); ++i) {
    ASSERT_OR_THROWEXCEPTION(0==stringArrayFS.get(i).compare(strings[i]));
  }
  
  IntListFS intListFS = fs.getIntListFSValue(intListF);
  for (num=0; num< NUMBEROF(ints); num++) {
    ASSERT_OR_THROWEXCEPTION(intListFS.getHead()==ints[num]);
	intListFS = intListFS.getTail();
  }
  ASSERT_OR_THROWEXCEPTION(NUMBEROF(ints)==num);
  
  IntArrayFS intArrayFS = fs.getIntArrayFSValue(intArrayF);
  ASSERT_OR_THROWEXCEPTION(NUMBEROF(ints)==intArrayFS.size());
  for (size_t i=0; i< NUMBEROF(ints); ++i) {
    ASSERT_OR_THROWEXCEPTION(intArrayFS.get(i)==ints[i]);
  }

  FloatListFS floatListFS = fs.getFloatListFSValue(floatListF);
  for (num=0; num< NUMBEROF(floats); num++) {
    ASSERT_OR_THROWEXCEPTION(floatListFS.getHead()==floats[num]);
	floatListFS = floatListFS.getTail();
  }
  ASSERT_OR_THROWEXCEPTION(NUMBEROF(floats)==num);

  FloatArrayFS floatArrayFS = fs.getFloatArrayFSValue(floatArrayF);
  ASSERT_OR_THROWEXCEPTION(NUMBEROF(floats)==floatArrayFS.size());
  for (size_t i=0; i< NUMBEROF(floats); ++i) {
    ASSERT_OR_THROWEXCEPTION(floatArrayFS.get(i)==floats[i]);
  }

  ByteArrayFS byteArrayFS = fs.getByteArrayFSValue(byteArrayF);
  ASSERT_OR_THROWEXCEPTION(NUMBEROF(chars)==byteArrayFS.size());
  for (size_t i=0; i< NUMBEROF(chars); ++i) {
    ASSERT_OR_THROWEXCEPTION(byteArrayFS.get(i)==chars[i]);
  }

  BooleanArrayFS boolArrayFS = fs.getBooleanArrayFSValue(booleanArrayF);
  ASSERT_OR_THROWEXCEPTION(BOOLEAN_ARRAY_SIZE==boolArrayFS.size());
  for (size_t i=0; i< BOOLEAN_ARRAY_SIZE; ++i) {
    val = !val;
    ASSERT_OR_THROWEXCEPTION(boolArrayFS.get(i)==val);
  }

  ShortArrayFS shortArrayFS = fs.getShortArrayFSValue(shortArrayF);
  ASSERT_OR_THROWEXCEPTION(NUMBEROF(shorts)==shortArrayFS.size());
  for (size_t i=0; i< NUMBEROF(shorts); ++i) {
    ASSERT_OR_THROWEXCEPTION(shortArrayFS.get(i)==shorts[i]);
  }

  LongArrayFS longArrayFS = fs.getLongArrayFSValue(longArrayF);
  ASSERT_OR_THROWEXCEPTION(NUMBEROF(longs)==longArrayFS.size());
  for (size_t i=0; i< NUMBEROF(longs); ++i) {
    ASSERT_OR_THROWEXCEPTION(longArrayFS.get(i)==longs[i]);
  }

  DoubleArrayFS doubleArrayFS = fs.getDoubleArrayFSValue(doubleArrayF);
  ASSERT_OR_THROWEXCEPTION(NUMBEROF(doubles)==doubleArrayFS.size());
  for (size_t i=0; i< NUMBEROF(doubles); ++i) {
    ASSERT_OR_THROWEXCEPTION(doubleArrayFS.get(i) == doubles[i]);
  }

  FeatureStructure listFS = fs.getFSValue(fsListF);
  num=0;
  ListFS curNode(listFS);
  while (!curNode.isEmpty()) {
    num++;
	FeatureStructure fs = curNode.getHead();
	ASSERT_OR_THROWEXCEPTION(fs.getType().getName().compare(CAS::TYPE_NAME_ANNOTATION)==0);
    curNode = curNode.getTail();
  }
  ASSERT_OR_THROWEXCEPTION(num==2);
  
  ArrayFS arrayFS = (ArrayFS) fs.getFSValue(fsArrayF);
  ASSERT_OR_THROWEXCEPTION(3==arrayFS.size());
  for (int i=0; i<3; i++) {
    FeatureStructure fs = arrayFS.get(i);
	ASSERT_OR_THROWEXCEPTION(fs.getType().getName().compare(CAS::TYPE_NAME_ANNOTATION)==0);
  }
  // check scalar values
  ASSERT_OR_THROWEXCEPTION(0==fs.getStringValue(stringF).compare(strings[0]));
  ASSERT_OR_THROWEXCEPTION(fs.getBeginPosition()==begin);
  ASSERT_OR_THROWEXCEPTION(fs.getEndPosition()==end);
  ASSERT_OR_THROWEXCEPTION(fs.getFloatValue(floatF)==floats[0]);
  ASSERT_OR_THROWEXCEPTION(fs.getBooleanValue(booleanF)==val);
  ASSERT_OR_THROWEXCEPTION(fs.getByteValue(byteF)=='z');
  ASSERT_OR_THROWEXCEPTION(fs.getShortValue(shortF)==shorts[0]);
  ASSERT_OR_THROWEXCEPTION(fs.getLongValue(longF)==longs[0]);
  ASSERT_OR_THROWEXCEPTION(fs.getDoubleValue(doubleF)==doubles[0]);
}







/* ----------------------------------------------------------------------- */
/*       Main routine                                                      */
/* ----------------------------------------------------------------------- */

int main(int argc, char * argv[]) /*
---------------------------------- */
{
  LOG("UIMATEST_PRIMITIVETYPES started");
  int iRetVal = 0;

  try {
    char const * config =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<typeSystemDescription xmlns=\"http://uima.apache.org/resourceSpecifier\">"
      " <types>"
      "   <typeDescription>"
      "        <name>test.primitives.Example</name>"
      "        <description></description>"
      "        <supertypeName>uima.tcas.Annotation</supertypeName>"
      "        <features>"
      "   <featureDescription>"
      "     <name>floatFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.Float</rangeTypeName>"
      "   </featureDescription>"
      "   <featureDescription>"
      "     <name>stringFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.String</rangeTypeName>"
      "   </featureDescription>"
      "   <featureDescription>"
      "     <name>boolFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.Boolean</rangeTypeName>"
      "   </featureDescription>"
      "   <featureDescription>"
      "     <name>byteFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.Byte</rangeTypeName>"
      "   </featureDescription>"
      "   <featureDescription>"
      "     <name>shortFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.Short</rangeTypeName>"
      "   </featureDescription>"
      "   <featureDescription>"
      "     <name>longFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.Long</rangeTypeName>"
      "   </featureDescription>"
      "   <featureDescription>"
      "     <name>doubleFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.Double</rangeTypeName>"
      "   </featureDescription>"
      "   <featureDescription>"
      "     <name>intArrayFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.IntegerArray</rangeTypeName>"
		//	 "     <multipleReferencesAllowed>true</multipleReferencesAllowed>"
      "   </featureDescription>"
      "   <featureDescription>"
      "     <name>floatArrayFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.FloatArray</rangeTypeName>"
      "   </featureDescription>"
      "   <featureDescription>"
      "     <name>stringArrayFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.StringArray</rangeTypeName>"
      "   </featureDescription>"
      "   <featureDescription>"
      "     <name>boolArrayFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.BooleanArray</rangeTypeName>"
		//	 "     <multipleReferencesAllowed>true</multipleReferencesAllowed>"
      "   </featureDescription>"
      "   <featureDescription>"
      "     <name>byteArrayFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.ByteArray</rangeTypeName>"
      "   </featureDescription>"
      "   <featureDescription>"
      "     <name>shortArrayFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.ShortArray</rangeTypeName>"
		//	 "     <multipleReferencesAllowed>true</multipleReferencesAllowed>"
      "   </featureDescription>"
      "   <featureDescription>"
      "     <name>longArrayFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.LongArray</rangeTypeName>"
		//	 "     <multipleReferencesAllowed>true</multipleReferencesAllowed>"
      "   </featureDescription>"
      "   <featureDescription>"
      "     <name>doubleArrayFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.DoubleArray</rangeTypeName>"
      "   </featureDescription>"
	  " <featureDescription>"
      "     <name>intListFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.IntegerList</rangeTypeName>"
      "   </featureDescription>"
	  " <featureDescription>"
      "     <name>floatListFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.FloatList</rangeTypeName>"
      "   </featureDescription>"
	  " <featureDescription>"
      "     <name>stringListFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.StringList</rangeTypeName>"
      "   </featureDescription>"
	   " <featureDescription>"
      "     <name>fsListFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.FSList</rangeTypeName>"
      "   </featureDescription>"
	  " <featureDescription>"
      "     <name>fsArrayFeature</name>"
      "     <description></description>"
      "     <rangeTypeName>uima.cas.FSArray</rangeTypeName>"
      "   </featureDescription>"
      "        </features>"
      "   </typeDescription>"
      " </types>"
      "</typeSystemDescription>";

    ResourceManager::createInstance("test");

    ErrorInfo errorInfo;
    ofstream outputStream;

    static TypeSystem *ts = Framework::createTypeSystemFromXMLBuffer(config, errorInfo );
    CAS* cas =  Framework::createCAS(*ts, errorInfo);
    ASSERT_OR_THROWEXCEPTION( EXISTS(cas) );
    
    /* add a FS */
    LOG("UIMATEST_PRIMITIVETYPES create a FS");
    createExampleFS(*cas);
    validateFS(*cas);

    /* test xcas serialization */
    CAS* trgCas =  Framework::createCAS(*ts, errorInfo);
    LOG("UIMATEST_PRIMITIVETYPES test XCAS serialization");
    outputStream.open("temp.xcas");
    ASSERT_OR_THROWEXCEPTION(outputStream);
    //LOG("serialize XCAS");
    XCASWriter writer(*cas, false);
    writer.write(outputStream);
    outputStream.close();
    //LOG("deserialize XCAS");
    XCASDeserializer::deserialize("temp.xcas", *trgCas);
    validateFS(*trgCas);

	/* test xmi serialization */
    LOG("UIMATEST_PRIMITIVETYPES test XMI serialization");
    outputStream.open("testprimitivetypes.xmi");
    ASSERT_OR_THROWEXCEPTION(outputStream);
    trgCas->reset();
    //LOG("serialize XMI");
    XmiWriter xmiwriter(*cas, false);
    xmiwriter.write(outputStream);
    outputStream.close();
    //LOG("deserialize XMI"); 
    XmiDeserializer::deserialize("testprimitivetypes.xmi", *trgCas);
    validateFS(*trgCas);
        
    /* test blob serialization */
    LOG("UIMATEST_PRIMITIVETYPES test blob serialization");
    trgCas->reset();
    internal::CASSerializer serializer(false);
    internal::CASDeserializer deserializer;
    //LOG("serialize BLOB");
    size_t blobsz = serializer.getBlobSize(*cas);
    char* blob = new char[blobsz];
    size_t blobsz2 = serializer.getBlob(*cas, blob, blobsz);
    ASSERT_OR_THROWEXCEPTION(blobsz == blobsz2);
    //LOG("deserialize BLOB");
    deserializer.deserializeBlob(blob, *trgCas);
    delete[] blob;
    validateFS(*trgCas);

    /* test binary serialization */
    LOG("UIMATEST_PRIMITIVETYPES test binary serialization");
    trgCas->reset();
    internal::SerializedCAS serializedCas;
    //LOG("serialize data");
    serializer.serializeData(*cas, serializedCas);
    //LOG("deserialize data");
    deserializer.deserializeData(serializedCas, *trgCas);
    validateFS(*trgCas);

    delete cas;
    delete trgCas;
    LOG("UIMATEST_PRIMITIVETYPES finished");
  } catch (Exception & exc) {
    cerr << exc.asString() << endl;
    iRetVal = 1;
  }
#ifdef NDEBUG
  catch (...) {
    cerr << "Unexpected exception " << endl;
    iRetVal = 1;
  }
#endif
  return iRetVal;
}

/* <EOF> */
