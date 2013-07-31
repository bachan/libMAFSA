Summary: 	MAFSA developement library
Name: 		libMAFSA-devel
Version: 	0.1.0
Release: 	0%{?dist}
License: 	LGPL
Source: 	libMAFSA-%{version}.tar.gz
Group:		Developement/Libraries
BuildRoot: 	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:	expat-devel cmake gcc-c++

%description
MAFSA developement library: Minimal Acyclic Finite State Automata

%prep
%setup -q -n libMAFSA-%{version}

%build
cmake -D SKIP_RELINK_RPATH=ON . -DCMAKE_INSTALL_PREFIX=/usr -DCFLAGS="${CFLAGS}" -DCXXFLAGS="${CXXFLAGS}"
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
mkdir %{buildroot}
make DESTDIR=%{buildroot} install
mkdir $RPM_BUILD_ROOT/usr/share

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_includedir}/MAFSA
%{_libdir}/libMAFSA.a

%changelog
* Tue Mar 26 2013 Alexander Pankov <pianist@usrsrc.ru> - 0.1.0
+ First RPM package

