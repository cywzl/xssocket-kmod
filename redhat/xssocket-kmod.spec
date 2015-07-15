# -*- rpm-spec -*-

%define kernel_version %(uname -r)

Summary: xssocket
Name:    xssocket-kmod 
Version: 1.0.2
Release: 1 
Group:   xssocket 
License: GPL
URL:  http://www.xen.org
Source0: xssocket-kmod-%{version}.tar.bz2
BuildRoot: %{_tmppath}/%{name}-%{version}-root

# latex
ExclusiveArch: i386 i686 x86_64

%description
xssocket


%prep 
%setup -q

%build
rm -rf $RPM_BUILD_ROOT
make all

%install
make install DESTDIR=$RPM_BUILD_ROOT

%post
depmod
modprobe xssocket

if [ -e /etc/rc.modules ] ; then
    sed -i "/xssocket/d" /etc/rc.modules
fi
echo "modprobe xssocket" >> /etc/rc.modules
chmod 755 /etc/rc.modules

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/lib/modules/%{kernel_version}/kernel/drivers/xssocket.ko
/usr/local/bin/xssocket_sender
/usr/local/bin/xssocket_receiver

%changelog

