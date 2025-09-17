%global         tag %{?tag}%{!?tag:v%{version}}
Name:           sigma-driver
Version:        %{?version}%{!?version:0.0.0}
Release:        1%{?dist}
Summary:        Sigma camera PTP driver and example app
License:        MIT
URL:            https://github.com/ValNyz/sigma-driver
Source0:        %{url}/archive/refs/tags/%{tag}.tar.gz#/%{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  ninja-build
BuildRequires:  pkgconfig(libusb-1.0)

%description
PTP communication library for Sigma cameras and a small example app.

%prep
%setup -q -n sigma-driver-%{version}
# Disable tests to avoid network fetch during build
sed -i 's/^enable_testing/# enable_testing/' CMakeLists.txt || :
sed -i 's/^add_subdirectory(\s*tests\s*)/# add_subdirectory(tests)/' CMakeLists.txt || :

%build
cmake -S . -B build \  -DCMAKE_BUILD_TYPE=Release \  -DBUILD_SHARED_LIBS=ON \  -G Ninja
cmake --build build --parallel %{?_smp_mflags}

%install
mkdir -p %{buildroot}%{_includedir}/sigma-driver
cp -a include/* %{buildroot}%{_includedir}/sigma-driver/

mkdir -p %{buildroot}%{_libdir}
# copy any shared or static library that was built
if [ -f build/libptp_sigma.so ]; then cp -a build/libptp_sigma.so %{buildroot}%{_libdir}/; fi
if ls build/*/libptp_sigma.so* >/dev/null 2>&1; then cp -a build/*/libptp_sigma.so* %{buildroot}%{_libdir}/ || :; fi
if [ -f build/libptp_sigma.a ]; then cp -a build/libptp_sigma.a %{buildroot}%{_libdir}/; fi

mkdir -p %{buildroot}%{_bindir}
if [ -f build/sigma_capture_example ]; then cp -a build/sigma_capture_example %{buildroot}%{_bindir}/; fi

%files
%license LICENSE
%doc README.md
%{_includedir}/sigma-driver/
%{_libdir}/libptp_sigma.so*
%{_libdir}/libptp_sigma.a
%{_bindir}/sigma_capture_example

%changelog
* Wed Sep 17 2025 Valentin Nyzam <valentin.nyzam@gmail.com> 1.0.0-1
  - Initial release: PTP control for SIGMA fp-series cameras over USB.
  - Provide C++17 library libptp_sigma using libusb-1.0.
  - Implement session and application setup (open_session, config_api).
  - Extensive tests of camera parameter groups:
    - Group1: ISO and shutter speed (Apex encode helpers).
    - Group2: exposure mode and image quality (DNG/JPEG).
    - Group3: destination to save (in-camera or PC).
  - Implement capture workflow:
  - snap(NonAFCapt), wait_completion, get_pict_file_info2, file download,
    clear_image_db_single.
  - Install headers to /usr/include/sigma-driver/, library to /usr/lib*/,
    and example tool sigma_capture_example to /usr/bin.