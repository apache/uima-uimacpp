# ---------------------------------------------------------------------------
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ---------------------------------------------------------------------------

#SUBDIRS = framework utils test
AUTOMAKE_OPTIONS = foreign
SUBDIRS = src
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = docs/builddocs.sh docs/doxyheader.html docs/overview_and_setup.html docs/uimacppdocs.mak docs/uimacpp.dox data examples scriptators LICENSE buildsdk.sh README.4src README.4bin NOTICE NOTICE.4bin RELEASE_NOTES.html autogen.sh


dist-hook:
	rm -rf `find $(distdir)/data -name .svn`
	rm -rf `find $(distdir)/examples -name .svn`
	rm -rf `find $(distdir)/scriptators -name .svn`
	rm -rf `find $(distdir) -name .svn`

install-data-local:
	$(INSTALL) src/base.mak $(DESTDIR)$(prefix)/lib/base.mak
	rm -f $(DESTDIR)$(prefix)/include/*.hpp

docs: 	docs/html/index.html

docs/html/index.html: 
	cd docs && ./builddocs.sh

sdk:	$(DESTDIR)$(prefix)/bin/runAECpp docs/html/index.html $(icu_install) $(apr_install) $(xerces_install) $(activemq_install) $(apu_install)
	echo "Building SDK with $(icu_install) $(apr_install) $(xerces_install) $(activemq_install) $(apu_install)  in target directory $(TARGETDIR)"
	export APR_HOME=$(apr_install) ICU_HOME=$(icu_install) XERCES_HOME=$(xerces_install) ACTIVEMQ_HOME=$(activemq_install) APU_HOME=$(apu_install) UIMA_INSTALLDIR=$(DESTDIR)$(prefix) && ./buildsdk.sh $(TARGETDIR)
