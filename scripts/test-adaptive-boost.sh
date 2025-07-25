#!/bin/bash

# ═══════════════════════════════════════════════════════════════════════════════
# Test Suite for Adaptive Boost Installation Script
# ═══════════════════════════════════════════════════════════════════════════════

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_LOG="/tmp/adaptive-boost-test.log"

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

test_passed=0
test_failed=0

log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $*" | tee -a "$TEST_LOG"
}

log_success() {
    echo -e "${GREEN}[PASS]${NC} $*" | tee -a "$TEST_LOG"
    ((test_passed++))
}

log_failure() {
    echo -e "${RED}[FAIL]${NC} $*" | tee -a "$TEST_LOG"
    ((test_failed++))
    # Don't return 1 here to avoid early exit with set -e
}

log_info() {
    echo -e "${YELLOW}[INFO]${NC} $*" | tee -a "$TEST_LOG"
}

# Test 1: Script exists and is executable
test_script_exists() {
    log_info "Testing script existence and permissions..."
    
    if [[ -f "$SCRIPT_DIR/adaptive-boost-install.sh" ]]; then
        log_success "Script file exists"
    else
        log_failure "Script file does not exist"
        return 1
    fi
    
    if [[ -x "$SCRIPT_DIR/adaptive-boost-install.sh" ]]; then
        log_success "Script is executable"
    else
        log_failure "Script is not executable"
        return 1
    fi
}

# Test 2: Script runs without errors
test_script_execution() {
    log_info "Testing script execution..."
    
    # Clean up any existing files
    rm -f /tmp/boost-install-metrics.json /tmp/adaptive-boost-install.log
    
    # Run the script and capture exit code
    if "$SCRIPT_DIR/adaptive-boost-install.sh" >/dev/null 2>&1; then
        log_success "Script executed successfully"
    else
        log_failure "Script execution failed"
        return 1
    fi
}

# Test 3: OS detection via direct execution
test_os_detection() {
    log_info "Testing OS detection via script output..."
    
    # Run script and check if it detects Noble
    local script_output
    script_output=$("$SCRIPT_DIR/adaptive-boost-install.sh" 2>&1 | grep "Detected OS" || true)
    
    if echo "$script_output" | grep -q "noble"; then
        log_success "Script correctly detected Noble OS"
    elif echo "$script_output" | grep -q "Detected OS"; then
        log_success "Script detected OS (not Noble): $script_output"
    else
        log_failure "Script did not detect OS properly"
        return 1
    fi
}

# Test 4: Metrics file generation (checks existing files from previous test)
test_metrics_generation() {
    log_info "Testing metrics file generation..."
    
    # Check if metrics file was created by previous test
    if [[ -f /tmp/boost-install-metrics.json ]]; then
        log_success "Metrics file was created"
    else
        log_failure "Metrics file was not created"
        return 1
    fi
    
    # Validate JSON format
    if python3 -m json.tool /tmp/boost-install-metrics.json >/dev/null 2>&1; then
        log_success "Metrics file is valid JSON"
    else
        log_failure "Metrics file is invalid JSON"
        return 1
    fi
    
    # Check for required fields
    local required_fields=("os_version" "boost_strategy" "packages_installed" "installation_duration")
    for field in "${required_fields[@]}"; do
        if python3 -c "import json; data=json.load(open('/tmp/boost-install-metrics.json')); print(data.get('$field', ''))" | grep -q .; then
            log_success "Required field '$field' present in metrics"
        else
            log_failure "Required field '$field' missing from metrics"
            return 1
        fi
    done
}

# Test 5: Log file generation
test_log_generation() {
    log_info "Testing log file generation..."
    
    if [[ -f /tmp/adaptive-boost-install.log ]]; then
        log_success "Log file was created"
    else
        log_failure "Log file was not created"
        return 1
    fi
    
    # Check for key log entries
    if grep -q "Starting Adaptive Boost Library Installation" /tmp/adaptive-boost-install.log; then
        log_success "Log contains start message"
    else
        log_failure "Log missing start message"
        return 1
    fi
    
    if grep -q "completed successfully" /tmp/adaptive-boost-install.log; then
        log_success "Log contains completion message"
    else
        log_failure "Log missing completion message"
        return 1
    fi
}

# Test 6: Package installation verification
test_package_installation() {
    log_info "Testing actual package installation..."
    
    # Check if boost packages are installed
    if dpkg -l libboost-all-dev 2>/dev/null | grep -q "^ii"; then
        log_success "libboost-all-dev is installed"
    else
        log_failure "libboost-all-dev is not installed"
    fi
    
    # Check if boost headers are available
    if [[ -d /usr/include/boost ]]; then
        log_success "Boost headers are available"
    else
        log_failure "Boost headers are not available"
        return 1
    fi
    
    # Check for t64 libraries on Noble
    local os_version
    os_version=$(lsb_release -cs 2>/dev/null || echo "unknown")
    if [[ "$os_version" == "noble" ]]; then
        if dpkg -l | grep -q "libboost.*t64"; then
            log_success "t64 libraries detected on Noble"
        else
            log_info "No t64 libraries found (may be expected depending on packages)"
        fi
    fi
}

# Main test execution
main() {
    log "═══════════════════════════════════════════════════════════════════════════════"
    log "Starting Adaptive Boost Installation Script Test Suite"
    log "═══════════════════════════════════════════════════════════════════════════════"
    
    # Clean up log file
    rm -f "$TEST_LOG"
    
    # Run tests (continue even if some fail)
    test_script_exists || true
    test_script_execution || true
    test_os_detection || true
    test_metrics_generation || true
    test_log_generation || true
    test_package_installation || true
    
    # Summary
    log "═══════════════════════════════════════════════════════════════════════════════"
    log "Test Results Summary"
    log "═══════════════════════════════════════════════════════════════════════════════"
    log_info "Tests passed: $test_passed"
    log_info "Tests failed: $test_failed"
    
    if [[ $test_failed -eq 0 ]]; then
        log_success "All tests passed! ✨"
        return 0
    else
        log_failure "Some tests failed. Check the log for details."
        return 1
    fi
}

# Execute main function
main "$@"