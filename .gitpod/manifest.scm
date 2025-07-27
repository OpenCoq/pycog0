;; Gitpod-optimized Guix package manifest for OpenCog
;; This manifest is specifically designed for cloud environments like Gitpod
;; with faster builds and reduced resource usage compared to full development setup

(use-modules (gnu packages)
             (gnu packages base)
             (gnu packages build-tools)
             (gnu packages c)
             (gnu packages cpp)
             (gnu packages cmake)
             (gnu packages compression)
             (gnu packages commencement)
             (gnu packages documentation)
             (gnu packages gcc)
             (gnu packages guile)
             (gnu packages maths)
             (gnu packages pkg-config)
             (gnu packages python)
             (gnu packages python-xyz)
             (gnu packages version-control)
             (gnu packages web))

;; Core OpenCog build dependencies optimized for Gitpod
(packages->manifest
  (list
    ;; Essential build tools
    gcc-toolchain
    cmake
    make
    pkg-config
    git
    
    ;; Core libraries required for AtomSpace
    boost
    cppunit
    guile-3.0
    
    ;; Mathematical and scientific libraries
    gsl
    cblas
    lapack
    
    ;; Python development (optimized versions)
    python-3.10
    python-cython
    python-nose
    python-numpy
    python-scipy
    
    ;; Development utilities
    binutils
    tar
    gzip
    
    ;; Documentation tools (lightweight)
    doxygen
    
    ;; Web and networking (for demos)
    curl
    wget))

;; Gitpod environment variables for this manifest
;; These are automatically set when using this manifest
(setenv "GUIX_PROFILE" "/home/gitpod/.guix-profile")
(setenv "GUIX_LOCPATH" "/home/gitpod/.guix-profile/lib/locale")
(setenv "OPENCOG_BUILD_TYPE" "Release")
(setenv "OPENCOG_GITPOD_OPTIMIZED" "true")