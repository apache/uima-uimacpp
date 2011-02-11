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
SUBDIRS = src
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = docs data examples scriptators LICENSE buildsdk.sh README.4sdk NOTICE.4sdk RELEASE_NOTES.html


dist-hook:
	rm -rf `find $(distdir)/docs -name .svn`
	rm -rf `find $(distdir)/data -name .svn`
	rm -rf `find $(distdir)/examples -name .svn`
	rm -rf `find $(distdir)/scriptators -name .svn`
	rm -rf `find $(distdir) -name .svn`

install-data-local:
	$(INSTALL) src/base.mak $(DESTDIR)$(prefix)/lib/base.mak

