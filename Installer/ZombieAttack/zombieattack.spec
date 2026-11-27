# Easy script for build rpm
# This should be build by command:
# rpmbuild --target=i386 -bb zombieattack.spec

Summary: 2d platform shooter
Name: ZombieAttack
Version: 1.0
Release: 1
License: GPL
Group: Amusements/Games
Source0: ZombieAttack
Source1: zombie-data.tar.gz
Source2: zombieattack.desktop
URL: http://zombie-attack.sourceforge.net/
BuildRoot: /var/tmp/%{name}-buildroot
Requires: SDL >= 1.2

%description
Game is about zombie attack, you are no-name soldier who try survive 
attack of hundreds of zombies in a farm on middle of nowhere. 
If you are kill enough zombies, you can advance your character 
with some upgrades. Choose upgrades carefully, for you fighting style. 

%prep

%build

%install
rm -Rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/opt/ZombieAttack
cd $RPM_BUILD_ROOT/opt/ZombieAttack
cp %_topdir/SOURCES/ZombieAttack .
tar xzvf %_topdir/SOURCES/zombie-data.tar.gz

mkdir -p $RPM_BUILD_ROOT/usr/share/applications
cp %_topdir/SOURCES/zombieattack.desktop $RPM_BUILD_ROOT/usr/share/applications


%clean
rm -rf $RPM_BUILD_ROOT 

%files
%defattr(-,root,root)
/opt/ZombieAttack
/usr/share/applications/*.desktop
