![Greenbone Logo](https://www.greenbone.net/wp-content/uploads/gb_logo_resilience_horizontal.png)

# openvas-smb

[![GitHub releases](https://img.shields.io/github/release/greenbone/openvas-smb.svg)](https://github.com/greenbone/openvas-smb/releases)
[![CircleCI](https://circleci.com/gh/greenbone/openvas-smb/tree/master.svg?style=svg)](https://circleci.com/gh/greenbone/openvas-smb/tree/master)

This is the `smb` module for the OpenVAS Scanner. It includes libraries
(`openvas-wmiclient`/`openvas-wincmd`) to interface with Microsoft Windows
Systems through the Windows Management Instrumentation API and a `wmic`
binary to execute queries remotely on that system.

## Installation

This module can be configured, built and installed with following commands:

    cmake .
    make install

For detailed installation requirements and instructions, please see the file
[INSTALL.md](INSTALL.md).

If you are not familiar or comfortable building from source code, we recommend
that you use the Greenbone Community Edition, a prepared virtual machine with a
readily available setup. Information regarding the virtual machine is available
at <https://www.greenbone.net/en/community-edition/>.

## Support

For any question on the usage of `openvas-smb` please use the [Greenbone
Community Portal](https://community.greenbone.net/c/gse). If you found a
problem with the software, please [create an
issue](https://github.com/greenbone/openvas-smb/issues) on GitHub. If you are a
Greenbone customer you may alternatively or additionally forward your issue to
the Greenbone Support Portal.

## Maintainer

This project is maintained by [Greenbone Networks GmbH](https://www.greenbone.net/).

## Contributing

Your contributions are highly appreciated. Please [create a pull
request](https://github.com/greenbone/openvas-smb/pulls) on GitHub. Bigger
changes need to be discussed with the development team via the [issues section
at github](https://github.com/greenbone/openvas-smb/issues) first.

## License

Licensed under the [GNU General Public License v2.0 or later](COPYING).

## History

This module is derived from a package distributed by Zenoss, Inc.,
named wmi-1.3.14. The original location was:
http://dev.zenoss.org/svn/trunk/inst/externallibs/wmi-1.3.14.tar.bz2

Since then, various special patches were applied for some needs of OpenVAS
Scanner. Also, anything not needed for OpenVAS Scanner was removed.

The Zenoss package actually was a copy of a code base of the Samba project
(https://www.samba.org/), forked based on the GPL v2 licensed status.
