 # Licensed to the Apache Software Foundation (ASF) under one
 # or more contributor license agreements.  See the NOTICE file
 # distributed with this work for additional information
 # regarding copyright ownership.  The ASF licenses this file
 # to you under the Apache License, Version 2.0 (the
 # "License"); you may not use this file except in compliance
 # with the License.  You may obtain a copy of the License at
 # 
 #   http://www.apache.org/licenses/LICENSE-2.0
 # 
 # Unless required by applicable law or agreed to in writing,
 # software distributed under the License is distributed on an
 # "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 # KIND, either express or implied.  See the License for the
 # specific language governing permissions and limitations
 # under the License.

import re
import math

# everything in the global namespace is eval'ed during initialization
# by the Pythonnator UIMA annotator
def initialize(annotContext):
  global pattern
  global source
  global debug
  global ac
  ac = annotContext
  source = ac.extractValue("SourceFile")
  debug = ac.extractIntegerValue("DebugLevel")
  if debug > 0:
    print(source + ": initialized")


def typeSystemInit(ts):
  global source
  global debug
  global ac
  if debug > 10:
    print(source + ": Type sytem init called")
  global spantype
  spantype =ts.getType('org.apache.uima.Span')
  if not spantype.isValid():
    error = source + ": org.apache.uima.Span is NOT found in type system!"
    ac.logError(error)
    raise Exception(error)

#
# the process method is passed two parameters, the CAS and
# the ResultsSpecification
def process(tcas, rs):
  global spantype
  global source
  global debug
  global ac
  if debug > 10:
    print(source + ": This is a process function")
    ac.logMessage("process called")
  text = tcas.getDocumentText()
  index = tcas.getIndexRepository()
  lines = text.split('\n')
  inSpan = True
  offset = 0
  annotCount = 0
  start = 0
  print(len(lines))
  for line in lines:
      #print(line.encode('utf-8').decode('ascii', errors='ignore'))
      if line.strip() == "":
          if inSpan:
            if start != offset:
              fs = tcas.createAnnotation(spantype, start, offset)
              index.addFS(fs)
              annotCount += 1
            inSpan = False
          # else: pass, # all white space
      elif not inSpan:
          inSpan = True
          start = offset
      # else; pass # span grows
      offset += len(line)
  if inSpan:
      fs = tcas.createAnnotation(spantype, start, len(text))
      index.addFS(fs)
      annotCount += 1

  if debug > 0:
    print(source + ": created " + str(annotCount) + " annotations")
  if debug > 20:
    annots = 0
    iterator = tcas.getAnnotationIndex(spantype).iterator()
    while iterator.isValid():
      annots += 1
      iterator.moveToNext()
    print(source + ": found " + str(annots) + " annotations")
