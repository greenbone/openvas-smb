# openvas-smb

This is the smb module for the OpenVAS Scanner. It includes libraries
(openvas-wmiclient / openvas-wincmd) to interface with Microsoft Windows
Systems through the Windows Management Instrumentation API and a winexe
binary to execute processes remotely on that system.

For more information, please refer to http://www.greenbone.net or
to http://www.openvas.org.

Please see the file COPYING for the license information.

To build and install openvas-smb, please refer to the instructions provided in
the file INSTALL.  If you are not familiar or comfortable with building from
source code, we recommend that you use the prepared virtual machine with a
readily available setup.  Information regarding the virtual machine is available
at http://www.greenbone.net/en/community-edition/.

An overview on commercial options is available at
http://www.greenbone.net/en/product-comparison/.

You can report bugs to https://github.com/greenbone/openvas-smb/issues


## History:

This module is derived from a package distributed by Zenoss, Inc.,
named wmi-1.3.14. The original location was:
http://dev.zenoss.org/svn/trunk/inst/externallibs/wmi-1.3.14.tar.bz2

Since then, various special patches were applied for some needs of OpenVAS
Scanner. Also, anything not needed for OpenVAS Scanner was removed.

The Zenoss package actually was a copy of a code base of the Samba project
(https://www.samba.org/), forked based on the GPL v2 licensed status.
