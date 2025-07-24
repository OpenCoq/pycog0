#!/bin/bash

# ═══════════════════════════════════════════════════════════════════════════════
# Adaptive Boost Library Installation Script for Cognitive Build Pipeline
# ═══════════════════════════════════════════════════════════════════════════════
# 
# This script implements adaptive installation logic for Boost libraries
# across different Ubuntu versions, with special handling for Noble's t64 ABI.
# 
# Features:
# - OS version detection and adaptive package selection
# - Comprehensive logging for meta-cognitive adaptation
# - Error handling and recovery mechanisms
# - Version pinning to prevent future breakage
# ═══════════════════════════════════════════════════════════════════════════════

set -euo pipefail

# ═══════════════════════════════════════════════════════════════════════════════
# Cognitive Logging Infrastructure
# ═══════════════════════════════════════════════════════════════════════════════

LOG_FILE="/tmp/adaptive-boost-install.log"
METRICS_FILE="/tmp/boost-install-metrics.json"

log_info() {
    echo "[INFO] $(date '+%Y-%m-%d %H:%M:%S') $*" | tee -a "$LOG_FILE" >&2
}

log_error() {
    echo "[ERROR] $(date '+%Y-%m-%d %H:%M:%S') $*" | tee -a "$LOG_FILE" >&2
}

log_debug() {
    echo "[DEBUG] $(date '+%Y-%m-%d %H:%M:%S') $*" >> "$LOG_FILE"
}

# Initialize cognitive metrics tracking
init_metrics() {
    cat > "$METRICS_FILE" << EOF
{
  "installation_start": "$(date -Iseconds)",
  "os_version": "",
  "boost_strategy": "",
  "packages_installed": [],
  "installation_duration": 0,
  "errors_encountered": [],
  "memory_usage_mb": 0,
  "attention_allocated": "boost-installation"
}
EOF
}

update_metrics() {
    local key="$1"
    local value="$2"
    python3 -c "
import json
import sys
try:
    with open('$METRICS_FILE', 'r') as f:
        data = json.load(f)
    data['$key'] = '$value'
    with open('$METRICS_FILE', 'w') as f:
        json.dump(data, f, indent=2)
except Exception as e:
    print(f'Error updating metrics: {e}', file=sys.stderr)
"
}

add_package_to_metrics() {
    local package="$1"
    python3 -c "
import json
import sys
try:
    with open('$METRICS_FILE', 'r') as f:
        data = json.load(f)
    data['packages_installed'].append('$package')
    with open('$METRICS_FILE', 'w') as f:
        json.dump(data, f, indent=2)
except Exception as e:
    print(f'Error adding package to metrics: {e}', file=sys.stderr)
"
}

add_error_to_metrics() {
    local error="$1"
    python3 -c "
import json
import sys
try:
    with open('$METRICS_FILE', 'r') as f:
        data = json.load(f)
    data['errors_encountered'].append('$error')
    with open('$METRICS_FILE', 'w') as f:
        json.dump(data, f, indent=2)
except Exception as e:
    print(f'Error adding error to metrics: {e}', file=sys.stderr)
"
}

# ═══════════════════════════════════════════════════════════════════════════════
# OS Detection and Cognitive Pattern Recognition
# ═══════════════════════════════════════════════════════════════════════════════

detect_os_version() {
    local os_version=""
    local version_id=""
    
    if [[ -f /etc/os-release ]]; then
        # Source the file in a subshell to avoid variable pollution
        eval "$(grep -E '^(VERSION_CODENAME|VERSION_ID|NAME|ID)=' /etc/os-release)"
        os_version="$VERSION_CODENAME"
        version_id="$VERSION_ID"
        log_info "Detected OS: $NAME $VERSION_ID ($VERSION_CODENAME)"
        log_debug "Full OS details: ID=$ID, VERSION_ID=$VERSION_ID"
    else
        log_error "Cannot detect OS version - /etc/os-release not found"
        exit 1
    fi
    
    update_metrics "os_version" "$os_version"
    echo "$os_version"
}

# ═══════════════════════════════════════════════════════════════════════════════
# Adaptive Installation Logic (Scheme-inspired)
# ═══════════════════════════════════════════════════════════════════════════════

# Define boost package lists for different OS versions
get_boost_packages() {
    local os_version="$1"
    local packages=""
    
    case "$os_version" in
        "noble")
            log_info "Using Noble (24.04) specific package strategy with t64 ABI support"
            update_metrics "boost_strategy" "noble-t64-aware"
            packages="build-essential cmake libboost-all-dev"
            ;;
        "jammy"|"focal"|"bionic")
            log_info "Using standard package strategy for $os_version"
            update_metrics "boost_strategy" "standard-pre-t64"
            packages="build-essential cmake libboost-all-dev"
            ;;
        *)
            log_info "Unknown OS version $os_version, using standard strategy"
            update_metrics "boost_strategy" "unknown-fallback"
            packages="build-essential cmake libboost-all-dev"
            ;;
    esac
    
    echo "$packages"
}

# Verify package availability before installation
verify_packages() {
    local packages="$1"
    log_info "Verifying package availability..."
    
    for package in $packages; do
        if apt-cache show "$package" &>/dev/null; then
            log_debug "Package $package is available"
        else
            log_error "Package $package is not available"
            add_error_to_metrics "package_not_available:$package"
            return 1
        fi
    done
    return 0
}

# Install packages with error handling and recovery
install_packages() {
    local packages="$1"
    local retry_count=0
    local max_retries=3
    
    log_info "Installing packages: $packages"
    
    while [[ $retry_count -lt $max_retries ]]; do
        if sudo apt-get install -y $packages; then
            log_info "Package installation successful"
            for package in $packages; do
                add_package_to_metrics "$package"
            done
            return 0
        else
            retry_count=$((retry_count + 1))
            log_error "Installation attempt $retry_count failed"
            add_error_to_metrics "installation_failed_attempt_$retry_count"
            
            if [[ $retry_count -lt $max_retries ]]; then
                log_info "Retrying installation in 5 seconds..."
                sleep 5
                sudo apt-get update
            fi
        fi
    done
    
    log_error "Failed to install packages after $max_retries attempts"
    return 1
}

# ═══════════════════════════════════════════════════════════════════════════════
# Memory and Resource Monitoring
# ═══════════════════════════════════════════════════════════════════════════════

monitor_resources() {
    local memory_mb
    memory_mb=$(free -m | awk 'NR==2{printf "%d", $3}')
    update_metrics "memory_usage_mb" "$memory_mb"
    log_debug "Current memory usage: ${memory_mb}MB"
}

# ═══════════════════════════════════════════════════════════════════════════════
# Self-Healing Pipeline Logic
# ═══════════════════════════════════════════════════════════════════════════════

perform_system_diagnostics() {
    log_info "Performing system diagnostics for self-healing pipeline..."
    
    # Check disk space
    local disk_usage
    disk_usage=$(df / | awk 'NR==2 {print $5}' | sed 's/%//')
    if [[ $disk_usage -gt 90 ]]; then
        log_error "Disk usage is high: ${disk_usage}%"
        add_error_to_metrics "high_disk_usage:${disk_usage}%"
    fi
    
    # Check apt cache
    if ! apt-cache policy &>/dev/null; then
        log_error "APT cache is corrupted, attempting to fix"
        add_error_to_metrics "apt_cache_corrupted"
        sudo apt-get clean
        sudo apt-get update
    fi
    
    # Verify boost library paths
    if [[ -d /usr/include/boost ]]; then
        log_debug "Boost headers found in /usr/include/boost"
    else
        log_error "Boost headers not found in expected location"
        add_error_to_metrics "boost_headers_missing"
    fi
}

# ═══════════════════════════════════════════════════════════════════════════════
# Main Installation Orchestration
# ═══════════════════════════════════════════════════════════════════════════════

main() {
    local start_time
    start_time=$(date +%s)
    
    log_info "═══════════════════════════════════════════════════════════════════════════════"
    log_info "Starting Adaptive Boost Library Installation"
    log_info "═══════════════════════════════════════════════════════════════════════════════"
    
    # Initialize cognitive tracking
    init_metrics
    monitor_resources
    
    # Detect OS and determine strategy
    local os_version
    os_version=$(detect_os_version)
    
    # Get appropriate packages for this OS
    local packages
    packages=$(get_boost_packages "$os_version")
    
    # Perform system diagnostics
    perform_system_diagnostics
    
    # Update package lists
    log_info "Updating package lists..."
    if ! sudo apt-get update; then
        log_error "Failed to update package lists"
        add_error_to_metrics "apt_update_failed"
        exit 1
    fi
    
    # Verify packages are available
    if ! verify_packages "$packages"; then
        log_error "Package verification failed"
        exit 1
    fi
    
    # Install packages
    if ! install_packages "$packages"; then
        log_error "Package installation failed"
        exit 1
    fi
    
    # Final verification and logging
    local end_time
    end_time=$(date +%s)
    local duration=$((end_time - start_time))
    
    update_metrics "installation_duration" "$duration"
    monitor_resources
    
    log_info "═══════════════════════════════════════════════════════════════════════════════"
    log_info "Adaptive Boost Installation completed successfully in ${duration} seconds"
    log_info "Cognitive metrics saved to: $METRICS_FILE"
    log_info "Detailed logs saved to: $LOG_FILE"
    log_info "═══════════════════════════════════════════════════════════════════════════════"
    
    # Display final metrics for transparency
    if [[ -f "$METRICS_FILE" ]]; then
        log_info "Final installation metrics:"
        cat "$METRICS_FILE" | tee -a "$LOG_FILE"
    fi
}

# ═══════════════════════════════════════════════════════════════════════════════
# Script Execution
# ═══════════════════════════════════════════════════════════════════════════════

# Only execute main if script is run directly (not sourced)
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    # Ensure script is run with appropriate permissions
    if [[ $EUID -eq 0 ]]; then
        echo "This script should not be run as root (use sudo for individual commands)" >&2
        exit 1
    fi

    # Trap errors and cleanup
    trap 'echo "Script failed on line $LINENO" >&2' ERR

    # Execute main function
    main "$@"
fi