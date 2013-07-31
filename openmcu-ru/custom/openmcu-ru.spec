# spec file for openmcu-ru
%define name 		openmcu-ru
%define version		
%define release 	
%define arch		
%define buildroot %{_topdir}/BUILDROOT/%{name}-%{version}-%{release}.%{arch}

BuildRoot: %{buildroot}
Summary:	OpenMCU-ru video conference server
Vendor:		Post Increment <info@postincrement.com>
URL:		http://openmcu.ru
Packager:	
License:	MPL, GPL
Name:		%{name}
Version:	%{version}
Release:	%{release}
Group:		System Environment/Daemons
Autoprov:	0
#Provides:	openmcu-ru
Autoreq:	0
Requires:	
%description
OpenMCU-ru video conference server

#%prep

#%build
#./configure
#make

#%install
#make install

%files
%defattr(755,root,root)
/etc/rc.d/init.d/openmcu-ru
%defattr(-,root,root)
/etc/openmcu-ru
/opt/openmcu-ru

%post
DAEMON_USER="mcu"
DAEMON_NAME="openmcu-ru"
DAEMON_HOMEDIR="/var/lib/$DAEMON_NAME"
DAEMON_LOGDIR="/var/log/$DAEMON_NAME"
DAEMON_DIR="/opt/$DAEMON_NAME"
# Creating system user
adduser --home-dir $DAEMON_HOMEDIR -M -r $DAEMON_USER
# Creating home directory
mkdir -p $DAEMON_HOMEDIR
chown $DAEMON_USER: $DAEMON_HOMEDIR
chmod 0700 $DAEMON_HOMEDIR
# Creating log directory
mkdir -p $DAEMON_LOGDIR
chown $DAEMON_USER:adm $DAEMON_LOGDIR
chmod 0750 $DAEMON_LOGDIR
# chown install directory
chown -R $DAEMON_USER: $DAEMON_DIR
# Service
chkconfig --add openmcu-ru
service openmcu-ru start
exit 0

%preun
service openmcu-ru stop
exit 0

%postun
DAEMON_USER="mcu"
DAEMON_NAME="openmcu-ru"
DAEMON_HOMEDIR="/var/lib/$DAEMON_NAME"
DAEMON_LOGDIR="/var/log/$DAEMON_NAME"
DAEMON_DIR="/opt/$DAEMON_NAME"
DAEMON_PIDDIR="/var/run/$DAEMON_NAME"
# Delete system user
userdel $DAEMON_USER
# Delete home directory
rm -r $DAEMON_HOMEDIR >/dev/null 2>&1
# Delete log directory
rm -r $DAEMON_LOGDIR >/dev/null 2>&1
# Delete install directory
rm -r $DAEMON_DIR >/dev/null 2>&1
# Delete pid directory
rm -r $DAEMON_PIDDIR >/dev/null 2>&1
# Service
chkconfig --del openmcu-ru
exit 0
