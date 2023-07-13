#!/usr/bin/perl

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

use perltator;
use strict;

sub initialize {
  $main::ac = shift;
  $main::thisScript = $main::ac->extractValue("SourceFile");
  $main::debug = $main::ac->extractIntegerValue("DebugLevel");
  if ($main::debug > 0) { 
    print "$main::thisScript: debug = $main::debug \n";
  }
}

sub typeSystemInit {
  my $ts = shift;
  if ($main::debug > 10) {
    print "$main::thisScript: Type sytem init called"
  }
  my $spantype = "org.apache.uima.Span";
  $main::spantype = $ts->getType($spantype);
  if (!$main::spantype->isValid()) {
    my $error = "$main::thisScript: $spantype is NOT found in type system!";
    $main::ac->logError($error);
    # set eval error to cause annotator to exit prematurely
    $@ = $error;
  }
}

#
# the process method is passed two parameters, the CAS and
# the ResultsSpecification
sub process {
  my ($tcas, $rs) = @_;
  if ($main::debug > 10) {
    print "$main::thisScript: This is a process function\n";
  }
  my $text = $tcas->getDocumentText();
  my $index = $tcas->getIndexRepository();
  my $annotCount = 0;
  my @lines = split(/\n/, $text);
  my $offset = 0;
  my $inSpan = 1;
  my $start = 0;
  foreach my$line(@lines) {
      if ($line=~m/^\s*$/){
          if ($inSpan) {
              if ($start != $offset) {
                  my $fs = $tcas->createAnnotation($main::spantype, $start, $offset);
                  $index->addFS($fs);
                  $annotCount++;
              }
              $inSpan = 0;
          }
      } elsif (! $inSpan) {
          $start = $offset;
          $inSpan = 1;
      }
      $offset += length($line);
  }
  if ($inSpan) {
      my $fs = $tcas->createAnnotation($main::spantype, $start, length($text));
      $index->addFS($fs);
      $annotCount++;
  }
      
  if ($main::debug > 10) {
    print "$main::thisScript: created $annotCount annotations\n";
  }
  if ($main::debug > 20) {
    my $annots = 0;
    my $iterator = $tcas->getAnnotationIndex($main::spantype)->iterator();
    while ($iterator->isValid()) {
	$annots += 1;
	if ($main::debug > 30) {
	    my $anno = $iterator->get();
	    my $text = $anno->getCoveredText();
	    if (length($text)>40) {
		$text = substr($text,0,20) . "...";
	    }
	    $text =~ s/\n+/ /g;
	    print "Annotation type=", $main::spantype->getName(),": \"$text\"\n"; 
	}
	$iterator->moveToNext();
    }
    print "$main::thisScript: found $annots annotations\n";
  }
}
