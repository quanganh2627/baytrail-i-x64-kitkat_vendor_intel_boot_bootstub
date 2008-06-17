Summary: this package provides bootstub binary for mrst power on
Name: bootstub
Version: 0.3
Release: 1%{?dist}
License: GPLv2
Group: System Environment/Shells
Source: %{name}-%{version}.tar.bz2
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)  

%description 
this package provides bootstub to create NAND boot image

%prep
%setup -q

%build
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/boot
install -m 755 bootstub $RPM_BUILD_ROOT/boot/

%clean

%files
%defattr(-,root,root,-)
/boot/bootstub

%changelog
* Mon May 26 2008 Alek Du <alek.du@intel.com > - 0.1-1
- create initial package
* Tue Jun 10 2008 Alek Du <alek.du@intel.com> - 0.2-1
- add SPI uart support
* Tue Jun 17 2008 Alek Du <alek.du@intel.com> - 0.3-1
- change SPI uart slave select to 0x2 according to ESL 2008.06 version
